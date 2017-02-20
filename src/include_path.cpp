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
#include "include_path.h"

#include "boost/filesystem.hpp"

namespace INCLUDE_GARDENER
{

Include_Path::Include_Path( const std::vector< std::string > & paths ) :
   paths( paths )
{

}

Include_Path::I_Map::const_iterator
Include_Path::find( const std::string & name )
{
   return name_map.find( name );
}

Include_Path::I_Map::const_iterator Include_Path::end( void )
{
   return name_map.end( );
}

std::pair< Include_Path::I_Map::iterator, bool > Include_Path::insert(
   const std::pair< const std::string, Include_Entry::Ptr > & value )
{
   return name_map.insert( value );
}

std::ostream& operator<<( std::ostream& os, const Include_Path & _p )
{
   os  << "Include Path ("  << reinterpret_cast< const void* >( &_p ) << "): ";
   for( auto p : _p.paths )
   {
      os << "   " << p << std::endl;
   }
   return os;
}


} // namespace SVN_EXTERNALS_DISPOSER

// vim: filetype=cpp et ts=3 sw=3 sts=3

