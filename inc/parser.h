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
#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <thread>
#include <mutex>
#include <deque>
#include <vector>
#include <condition_variable>
#include <regex>

#include "boost/filesystem.hpp"


#include "include_path.h"
#include "include_entry.h"
#include "graph.h"

namespace INCLUDE_GARDENER
{

/// @brief A parser class to seach for include statements.
/// @details
/// At the moment, only C/C++ preprocessor include statements
/// are supported.
///
/// The most important method is walk_tree. By calling this method,
/// a given path is recsively processed (all files in all sub-folders)
/// are passed to walk_file, which then processes each line.
/// @author feddischson
class Parser
{

public:

   /// @brief Ctor: not implemented!
   Parser( int                  n_file_workers,
           const std::string  & exclude_regex,
           Include_Path::Ptr    i_path,
           Graph              * g
           );

   /// @brief  Copy ctor: not implemented!
   Parser(const Parser & parser ) = delete;


   /// @brief  Assignment operator: not implemented!
   Parser& operator= (const Parser & parser ) = delete;


   /// @brief  Move constructor: not implemented!
   Parser( Parser && rhs ) = delete;


   /// @brief  Move assignment operator: not implemented!
   Parser& operator=( Parser && rhs ) = delete;


   /// @brief Standard dtor
   ~Parser() = default;


   /// @brief  Runs through a given file path and proceedes all include files.
   /// @return True on success, false if the path doesn't exist.
   bool walk_tree( const std::string & base_path,
                   const std::string & sub_path,
                   const std::string & pattern
                   );


   /// @brief Waits until work is done (job_queue is empty), ends all threads.
   void wait_for_workers( void );


private:


   /// @brief Processes a file to detect all include entries.
   void walk_file( const std::string & file_path,
                   const std::string & entry_name_1
                   );


   /// @brief If not already in the i_path and graph: this method adds an entry.
   void add_file_info( const std::string & name,
                       const boost::filesystem::path & path );

   /// @brief Threading method: takes an entry from job_queue to processes it.
   void do_work( int id );

   /// @brief Vector of threads, each calling do_work.
   std::vector< std::thread > file_workers;

   /// @brief Flag which indicates, if the exclude-regex is used
   const bool use_exclude_regex;

   /// @brief Regular expression which is used to exclude files.
   std::regex exclude_regex;

   /// @brief Each entry includes the name and path of the include entry.
   /// @details
   ///        The queue is protected by job_queue_mutex and job_queue_condition.
   std::deque< std::pair< std::string, std::string > > job_queue;

   /// @brief Protects job_queue (all worker threads and main-thread).
   std::mutex              job_queue_mutex;

   /// @brief Condition variable to sleep thread until jobs are available.
   std::condition_variable job_queue_condition;


   /// @brief Pointer to the global map holding all include entries.
   Include_Path::Ptr       i_path;

   /// @brief Pointer to the global graph instance.
   Graph                 * graph;

   /// @brief Protects i_path and graph (and all it's content),
   std::mutex              graph_mutex;

   /// @brief Flag to end worker threads.
   bool                    all_work_is_done;

}; // class Parser

} // namespace INCLUDE_GARDENER

#endif // PARSER_H

// vim: filetype=cpp et ts=3 sw=3 sts=3

