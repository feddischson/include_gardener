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

#include <regex>
#include <fstream>

#include "boost/filesystem.hpp"


namespace INCLUDE_GARDENER
{

bool Parser::walk_tree( const std::string & base_path,
                        const std::string & sub_path,
                        const std::string & pattern,
                        Include_Entry::Map & i_map )
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
      if( is_directory( itr->status() ) )
      {
         std::cout << "recursive call " << sub_entry.string()
                   << std::endl;
         walk_tree( base_path, sub_entry.string(), pattern, i_map );
      }
      else if( is_regular_file( itr->status() ) )
      {
         if( std::regex_search( itr->path().string(), file_regex ) )
         {
            auto e = i_map.find( sub_entry.string() );
            if( e != i_map.end() )
            {
               // TODO add path info to e
            }
            else
            {
               Include_Entry::Ptr e(
                     new Include_Entry( sub_entry.string(),
                                        itr->path().relative_path().string(),
                                        canonical( itr->path() ).string() ) );
               i_map[ sub_entry.string() ] = e;
               std::cout << "found file " << itr->path() << std::endl;
            }

            walk_file( itr->path().string() );
         }
      }
      else
      {
         // ignore all other files
      }
   }
}

bool Parser::walk_file( const std::string & path )
{
   std::ifstream infile( path );
   std::string line;

   std::regex line_regex( "#include\\s+[\"|<].*?[\"|>]",
         std::regex_constants::ECMAScript | std::regex_constants::icase);

   while ( std::getline( infile, line ) )
   {
      if( std::regex_search( line, line_regex ) )
      {
         std::cout << "found include:" << line << std::endl;

      }
   }

}


std::ostream& operator<<( std::ostream& os, const Parser& parser )
{
   os  << "Parser ("  << reinterpret_cast< const void* >( &parser ) << "): "
       << "TODO=" << "Add more content of this instance here @ "
       << __FILE__ << ":" << __LINE__;
   return os;
}


}; // namespace SVN_EXTERNALS_DISPOSER

// vim: filetype=cpp et ts=3 sw=3 sts=3

