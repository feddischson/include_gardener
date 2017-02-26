// Include-Gardener
//
// Copyright (C) 2017  Christian Haettich [feddischson]
//
// This program is free software; you can redistribute it
// and/or modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation;
// either version 3 of the License, or (at your option)
// any later version.
//
// This program is distributed in the hope that it will
// be useful, but WITHOUT ANY WARRANTY; without even the
// implied warranty of MERCHANTABILITY or FITNESS FOR A
// PARTICULAR PURPOSE. See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General
// Public License along with this program; if not, see
// <http://www.gnu.org/licenses/>.
//
#include "parser.h"

#include <fstream>

#include <boost/log/trivial.hpp>

using namespace std;

namespace INCLUDE_GARDENER
{

Parser::Parser( int                 n_file_workers,
                const string      & exclude_regex,
                Include_Path::Ptr   i_path,
                Graph             * graph ) :
   file_workers       ( n_file_workers ),
   use_exclude_regex  ( exclude_regex.size() > 0  ),
   exclude_regex      ( exclude_regex,
                        regex_constants::ECMAScript |
                        regex_constants::icase  ),
   job_queue          ( ),
   job_queue_mutex    ( ),
   job_queue_condition( ),
   i_path             ( i_path ),
   graph              ( graph  ),
   graph_mutex        ( ),
   all_work_is_done   ( false )
{
   // Start all worker threads.
   for( int i=0; i < n_file_workers; i++ )
   {
      file_workers[ i ] = thread( &Parser::do_work, this, i );
   }
}


void Parser::add_file_info( const string & name,
                            const boost::filesystem::path & path )
{
   unique_lock<mutex> glck( graph_mutex );
   auto abs_path = canonical( path ).string();
   auto e_itr = i_path->find_abs( abs_path );
   if( e_itr == i_path->end_abs() )
   {
      Include_Entry::Ptr e = Include_Entry::Ptr (
            new Include_Entry( name,
                               path.relative_path().string(),
                               abs_path ) );
      i_path->insert_abs( make_pair( abs_path,  e ) );

      boost::add_vertex( abs_path, *graph );
      (*graph)[ abs_path ] = e;
   }
}


bool Parser::walk_tree( const string & base_path,
                        const string & sub_path,
                        const string & pattern )
{

   regex file_regex( pattern,
            regex_constants::ECMAScript | regex_constants::icase);

   using namespace boost::filesystem;

   path p( base_path );
   p /= sub_path;

   // return false if the root-path doesn't exist
   if( exists( p ) == false )
   {
      return false;
   }

   directory_iterator end_itr; // default construction yields past-the-end
   for ( directory_iterator itr( p );
         itr != end_itr;
         ++itr )
   {
      path sub_entry( sub_path );
      sub_entry /= itr->path().filename().string();


      auto name = sub_entry.string();
      string itr_path = itr->path().string();

      if( is_directory( itr->status() ) )
      {
         // recursive call to process sub-directory
         walk_tree( base_path, sub_entry.string(), pattern );
      }
      else if( is_regular_file( itr->status() ) )
      {

         if( true == use_exclude_regex &&
             regex_search( itr_path, exclude_regex ) )
         {
            BOOST_LOG_TRIVIAL( trace ) << "Excluding " << itr_path;
            continue;
         }

         // check if this is a file which we should process.
         if( regex_search( itr_path, file_regex ) )
         {
            BOOST_LOG_TRIVIAL( trace ) << "Considering "
                                       << itr_path;

            // update the i_path and graph
            add_file_info( name, itr->path() );

            // Add an entry to the queue and notify the workers,
            // one of them will do the file processing
            {
               unique_lock<mutex> lck(job_queue_mutex);
               job_queue.push_front( pair< string, string>(
                     canonical( path( itr_path ) ).string(),
                     name ) );
               job_queue_condition.notify_all();
            }
         }
      }
      else
      {
         // ignore all other files
         BOOST_LOG_TRIVIAL( trace ) << "Ignoring " << itr_path;
      }
   }
   return true;
}

void Parser::wait_for_workers( void )
{
   // wait until the queue is empty
   {
      unique_lock<mutex> lck( job_queue_mutex );
      job_queue_condition.wait( lck, [this](){return job_queue.size()==0;});
      lck.unlock();
      job_queue_condition.notify_all();
   }

   // set the done flag
   {
      unique_lock<mutex> lck( job_queue_mutex );
      all_work_is_done = true;
      lck.unlock();
      job_queue_condition.notify_all();
   }

   for( size_t i = 0; i < file_workers.size(); i++  )
   {
      file_workers[i].join();
   }
   BOOST_LOG_TRIVIAL( debug ) << "All threads are done";

}


void Parser::do_work( int id )
{

   BOOST_LOG_TRIVIAL( debug ) << "Started worker [" << id << "]";

   while( true )
   {
      unique_lock<mutex> lck( job_queue_mutex );
      job_queue_condition.wait( lck, [this]()
            {
               return job_queue.size()>0 || all_work_is_done;
            });
      if( all_work_is_done )
      {
         BOOST_LOG_TRIVIAL( debug ) << "[" << id << "] All work is done";
         return;
      }

      auto entry = job_queue.back();
      job_queue.pop_back();
      lck.unlock();
      job_queue_condition.notify_all();

      BOOST_LOG_TRIVIAL( debug ) << "["
                                 << id
                                 << "]"
                                 << " processing"
                                 << entry.first;

      walk_file( entry.first, entry.second );
   }
}


void Parser::walk_file( const string & abs_path_1,
                        const string & )
{
   using namespace boost::filesystem;
   ifstream infile( abs_path_1 );
   path infile_dir = path( abs_path_1 ).parent_path();
   int line_cnt = 1;
   string line;
   smatch match;

   regex  re( "#include\\s+[\"|<](.*?)[\"|>]",
         regex_constants::ECMAScript | regex_constants::icase);

   while ( getline( infile, line ) )
   {
      if( regex_search( line, match, re ) && match.size() > 1 )
      {
         BOOST_LOG_TRIVIAL( trace ) << "Found include entry in "
                                    << abs_path_1
                                    << " line "
                                    << line_cnt
                                    << ": \"" << line << "\"";
         string entry_name_2 = match.str(1);


         // construct relative path and check, if the file exists
         path rel_path;


         unique_lock<mutex> glck( graph_mutex );


         // 1) check if in cache
         //    If an cache entry exists, an Include_Entry has already been
         //    created.
         string abs_path_2 = i_path->find_cached( entry_name_2 );

         if( abs_path_2.length() > 0 )
         {
            BOOST_LOG_TRIVIAL( trace ) << "Entry "
                                       << abs_path_2
                                       << " exists and was cached";
         }
         // 2) check if relative
         else if( exists( rel_path = infile_dir / entry_name_2 ) )
         {
            BOOST_LOG_TRIVIAL( trace ) << "Found relative include: "
                                       << entry_name_2;

            // It seems this is a relative include statement.
            // Check if an entry has already been added
            abs_path_2 = canonical( rel_path ).string();

            if( i_path->find_abs( abs_path_2 ) == i_path->end_abs() )
            {
               // no entry exists: add one
               Include_Entry::Ptr e = Include_Entry::Ptr(
                    new Include_Entry( entry_name_2 ) );
               i_path->insert_abs( make_pair( abs_path_2, e ) );
               boost::add_vertex( abs_path_2, *graph );
               (*graph)[ abs_path_2 ] = e;
            }
            else
            {
               BOOST_LOG_TRIVIAL( trace ) << "Entry "
                                          << abs_path_2
                                          << " already exists";
            }
         }

         // check if we should add a new entry
         else
         {
            // we haven't got such an entry before,
            // it is not a relative statement,
            // so check if it is available
            abs_path_2 = i_path->is_available( entry_name_2 );
            if( abs_path_2.length() > 0 )
            {
               // the file exists in the serach path
               // add a new entry if necessary
               if( i_path->find_abs( abs_path_2 ) == i_path->end_abs() )
               {

                  BOOST_LOG_TRIVIAL( trace ) << "Adding a new entry,"
                                             << " file exists";

                  // no entry exists: add one
                  Include_Entry::Ptr e = Include_Entry::Ptr(
                       new Include_Entry( entry_name_2 ) );
                  i_path->insert_abs( make_pair( abs_path_2, e ) );
                  i_path->insert_cache( make_pair( entry_name_2, abs_path_2 ) );
                  boost::add_vertex( abs_path_2, *graph );
                  (*graph)[ abs_path_2 ] = e;
               }
               else
               {
                  BOOST_LOG_TRIVIAL( trace )<<"No need to add a new entry, "
                                            <<"file exists but was not cached: "
                                            << entry_name_2;
                  i_path->insert_cache( make_pair( entry_name_2, abs_path_2 ) );
               }
            }
            else
            {
               BOOST_LOG_TRIVIAL( trace ) << "Adding a new entry,"
                                          << " file doesn't exists";
               abs_path_2 = entry_name_2;
               Include_Entry::Ptr e = Include_Entry::Ptr(
                     new Include_Entry( entry_name_2 ) );
               i_path->insert_blind( make_pair( abs_path_2, e ) );
               boost::add_vertex( abs_path_2, *graph );
               (*graph)[ abs_path_2 ] = e;
            }
         }

         // add the edge between the two vertices
         Edge_Descriptor edge;
         bool   b;
         boost::tie( edge, b ) = boost::add_edge_by_label(
               abs_path_1,
               abs_path_2,
               *graph );
         (*graph)[ edge ] = Edge{ line_cnt };
      }

      line_cnt++;
   }
}

} // namespace INCLUDE_GARDENER

// vim: filetype=cpp et ts=3 sw=3 sts=3

