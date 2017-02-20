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

namespace INCLUDE_GARDENER
{

Parser::Parser( int                  n_file_workers,
                const std::string  & exclude_regex,
                Include_Path::Ptr    i_path,
                Graph              * graph ) :
   file_workers       ( n_file_workers ),
   use_exclude_regex  ( exclude_regex.size() > 0  ),
   exclude_regex      ( exclude_regex,
         std::regex_constants::ECMAScript |
         std::regex_constants::icase  ),
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
      file_workers[ i ] = std::thread( &Parser::do_work, this, i );
   }
}


void Parser::add_file_info( const std::string & name,
                            const boost::filesystem::path & path )
{
   std::unique_lock<std::mutex> glck( graph_mutex );
   auto e_itr = i_path->find( name );
   if( e_itr != i_path->end() )
   {
      auto e = e_itr->second;
      e->add_path_info(
            path.relative_path().string(),
            canonical( path ).string()
            );
   }
   else
   {
      Include_Entry::Ptr e = Include_Entry::Ptr (
            new Include_Entry( name,
                               path.relative_path().string(),
                               canonical( path ).string() ) );
      i_path->insert( std::make_pair( name,  e ) );
      boost::add_vertex( name, *graph );
      (*graph)[ name ] = e;
   }
}


bool Parser::walk_tree( const std::string & base_path,
                        const std::string & sub_path,
                        const std::string & pattern )
{

   std::regex file_regex( pattern,
            std::regex_constants::ECMAScript | std::regex_constants::icase);

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
      std::string itr_path = itr->path().string();

      if( is_directory( itr->status() ) )
      {
         // recursive call to process sub-directory
         walk_tree( base_path, sub_entry.string(), pattern );
      }
      else if( is_regular_file( itr->status() ) )
      {

         if( true == use_exclude_regex &&
             std::regex_search( itr_path, exclude_regex ) )
         {
            BOOST_LOG_TRIVIAL( trace ) << "Excluding " << itr_path;
            continue;
         }

         // check if this is a file which we should process.
         if( std::regex_search( itr_path, file_regex ) )
         {
            BOOST_LOG_TRIVIAL( trace ) << "Considering "
                                       << itr_path;

            // update the i_path and graph
            add_file_info( name, itr->path() );

            // Add an entry to the queue and notify the workers,
            // one of them will do the file processing
            {
               std::unique_lock<std::mutex> lck(job_queue_mutex);
               job_queue.push_front( std::pair< std::string, std::string>(
                     itr_path,
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
      std::unique_lock<std::mutex> lck( job_queue_mutex );
      job_queue_condition.wait( lck, [this](){return job_queue.size()==0;});
      lck.unlock();
      job_queue_condition.notify_all();
   }

   // set the done flag
   {
      std::unique_lock<std::mutex> lck( job_queue_mutex );
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
      std::unique_lock<std::mutex> lck( job_queue_mutex );
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


void Parser::walk_file( const std::string & path,
                        const std::string & entry_name_1 )
{
   std::ifstream infile( path );
   int line_cnt = 1;
   std::string line;
   std::smatch match;

   std::regex  re( "#include\\s+[\"|<](.*?)[\"|>]",
         std::regex_constants::ECMAScript | std::regex_constants::icase);

   while ( std::getline( infile, line ) )
   {
      if( std::regex_search( line, match, re ) && match.size() > 1 )
      {
         BOOST_LOG_TRIVIAL( trace ) << "Found include entry in "
                                    << path
                                    << " line "
                                    << line_cnt
                                    << ": \"" << line << "\"";
         std::unique_lock<std::mutex> glck( graph_mutex );

         auto entry_name_2 = match.str(1);
         auto e_itr = i_path->find( entry_name_2 );

         if( e_itr == i_path->end() )
         {
            // there is no such entry in our data-structures:
            //  -> create it
            Include_Entry::Ptr e = Include_Entry::Ptr(
                  new Include_Entry( entry_name_2 ) );
            i_path->insert( std::make_pair( entry_name_2, e ) );

            boost::add_vertex( e->get_name(), *graph );
            (*graph)[ e->get_name() ]  = e;
         }

         // add the edge between the two vertices
         Edge_Descriptor edge;
         bool   b;
         boost::tie( edge, b ) = boost::add_edge_by_label(
               entry_name_1,
               entry_name_2,
               *graph );
         (*graph)[ edge ] = Edge{ line_cnt };
      }

      line_cnt++;
   }
}

} // namespace SVN_EXTERNALS_DISPOSER

// vim: filetype=cpp et ts=3 sw=3 sts=3

