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
#include <algorithm>

#include "include_entry.h"

namespace INCLUDE_GARDENER
{


Include_Entry::Include_Entry(
      const std::string & name
      ) :
   name( name ),
   rel_paths( 0 ),
   abs_paths( 0 )
{
}

Include_Entry::Include_Entry(
      const std::string & name,
      const std::string & rel_path,
      const std::string & abs_path ) :
   name( name ),
   rel_paths( 1, rel_path ),
   abs_paths( 1, abs_path )
{
}

void Include_Entry::add_path_info( const std::string & rel_path,
                                   const std::string & abs_path )
{
   if( std::find( rel_paths.begin(),
                  rel_paths.end(),
                           rel_path) == rel_paths.end() )
   {
      rel_paths.push_back( rel_path );
   }

   if( std::find( abs_paths.begin(),
                  abs_paths.end(),
                           abs_path) == abs_paths.end() )
   {
      abs_paths.push_back( abs_path );
   }
}

std::string Include_Entry::get_name( void ) const
{
   return name;
}

std::ostream& operator<<( std::ostream& os, const Include_Entry& entry )
{
   os  << "Include_Entry :" << std::endl
       << "     name = " << entry.name;
   return os;
}


}; // namespace SVN_EXTERNALS_DISPOSER

// vim: filetype=cpp et ts=3 sw=3 sts=3

