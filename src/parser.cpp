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
                int                 recursive_limit,
                Detector::Ptr       detector,
                Include_Path::Ptr   i_path,
                Graph             * graph ) :
   file_workers       ( n_file_workers            ),
   recursive_limit    ( recursive_limit           ),
   detector           ( detector ),
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
                              int      recursive_cnt )
{

   using namespace boost::filesystem;

   path p( base_path );
   p /= sub_path;

   // return false if the root-path doesn't exist
   if( exists( p ) == false )
   {
      return false;
   }
   // return false if it is not a directory
   else if( is_directory( p ) == false )
   {
      return false;
   }

   directory_iterator end_itr;
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
         if( ( recursive_limit == -1 ) || 
             ( recursive_limit >= 0 &&
               recursive_cnt < recursive_limit ) )
         {
            // recursive call to process sub-directory
            walk_tree( base_path, sub_entry.string(), recursive_cnt+1);
         }
      }
      else if( is_regular_file( itr->status() ) )
      {

         if( false == detector->use_file( itr_path ) )
         {
            continue;
         }

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

   ifstream infile( abs_path_1 );
   using namespace boost::filesystem;
   path infile_dir = path( abs_path_1 ).parent_path();
   int line_cnt = 1;
   string line;

   while ( getline( infile, line ) )
   {
      string entry_name_2 = detector->is_include_statement( line );
      if( 0 == entry_name_2.length() )
      {
         continue;
      }

      BOOST_LOG_TRIVIAL( trace ) << "Found include entry in "
                                 << abs_path_1
                                 << " line "
                                 << line_cnt
                                 << ": \"" << line << "\"";

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
            auto existing_entry = i_path->find_abs( abs_path_2 );
            if( existing_entry == i_path->end_abs() )
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

               // overwrite the name, otherwise the path would be the name
               existing_entry->second->set_name( entry_name_2 );
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


      line_cnt++;
   }
}

} // namespace INCLUDE_GARDENER

// vim: filetype=cpp et ts=3 sw=3 sts=3
