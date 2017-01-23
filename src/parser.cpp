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

bool Parser::walk_tree( const std::string & p,
                        const std::string & pattern )
{

   std::regex file_regex( pattern,
            std::regex_constants::ECMAScript | std::regex_constants::icase);

   using namespace boost::filesystem;

   path root_path( p );

   // return false if the root-path doesn't exist
   if( exists( root_path ) == false )
   {
      return false;
   }

   directory_iterator end_itr; // default construction yields past-the-end
   for ( directory_iterator itr( root_path );
         itr != end_itr;
         ++itr )
   {
      if( is_directory( itr->status() ) )
      {
         walk_tree( itr->path().string(), pattern );
      }
      else if( is_regular_file( itr->status() ) )
      {
         if( std::regex_search( itr->path().string(), file_regex ) )
         {
            walk_file( itr->path().string() );
            std::cout << "found file " << itr->path() << std::endl;
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

