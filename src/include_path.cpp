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
#include <boost/log/trivial.hpp>

using namespace std;

namespace INCLUDE_GARDENER
{

Include_Path::Include_Path( const vector< string > & paths ) :
   paths( paths )
{

}

string Include_Path::find_cached( const string & name )
{
   auto tmp = cache.find( name );
   if( tmp != cache.end() )
   {
      return tmp->second;
   }
   else
   {
      auto tmp2 = blind_map.find( name );
      if( tmp2 != blind_map.end() )
      {
         return tmp2->first;
      }
   }
   return string( "" );
}

Include_Path::I_Map::const_iterator
Include_Path::find_abs( const string & name )
{
   return absolute_map.find( name );
}


Include_Path::I_Map::const_iterator Include_Path::end_abs( void )
{
   return absolute_map.end( );
}


void Include_Path::insert_abs( const I_Pair & value )
{
   absolute_map.insert( value );
}

void Include_Path::insert_blind( const I_Pair  & value )
{
   blind_map.insert( value );
}

void Include_Path::insert_cache( const S_Pair & value )
{
   cache.insert( value );
}

ostream& operator<<( ostream& os, const Include_Path & _p )
{
   os  << "Include Path ("  << reinterpret_cast< const void* >( &_p ) << "): ";
   for( auto p : _p.paths )
   {
      os << "   " << p << endl;
   }
   return os;
}


string Include_Path::is_available( const string & name ) const
{
   using namespace boost::filesystem;
   for( auto p : paths )
   {
      path base( p );
      base /= name;
      if( exists( base ) )
      {
         return canonical( base ).string();
      }
   }
   return string( "" );
}

} // namespace INCLUDE_GARDENER

// vim: filetype=cpp et ts=3 sw=3 sts=3

