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


#include "include_entry.h"
#include "graph.h"

namespace INCLUDE_GARDENER
{

/// @author feddischson
/// @brief A general parser class.
class Parser
{

public:

   /// @author feddischson
   /// @brief Ctor: not implemented!
   Parser( ) = default;

   /// @author feddischson
   /// @brief  Copy ctor: not implemented!
   Parser(const Parser & parser ) = delete;


   /// @author feddischson
   /// @brief  Assignment operator: not implemented!
   Parser& operator= (const Parser & parser ) = delete;


   /// @author feddischson
   /// @brief  Move constructor: not implemented!
   Parser( Parser && rhs ) = delete;


   /// @author feddischson
   /// @brief  Move assignment operator: not implemented!
   Parser& operator=( Parser && rhs ) = delete;


   /// @author feddischson
   /// @brief Standard dtor
   ~Parser() = default;


   /// @author feddischson
   /// @brief  Runs through a given file path and proceedes all include files.
   bool walk_tree( const std::string & base_path,
                   const std::string & sub_path,
                   const std::string & pattern,
                   Include_Entry::Map & i_map,
                   Graph & graph );


private:

   /// @brief Processes a file to detect all include entries.
   bool walk_file( const std::string & path,
                   Include_Entry::Ptr entry,
                   Include_Entry::Map & i_map,
                   Graph & graph );

}; // class Parser

}; // namespace INCLUDE_GARDENER

#endif // PARSER_H

// vim: filetype=cpp et ts=3 sw=3 sts=3

