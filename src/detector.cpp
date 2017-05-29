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
#include "detector.h"

#include <boost/log/trivial.hpp>

using namespace std;

namespace INCLUDE_GARDENER
{


Detector::Detector(
               const string               & include_regex,
               const string               & file_regex,
               const vector<string>       & exclude_regex,
               const vector<unsigned int> & include_group_select
      )
   :
   include_regex           ( include_regex, regex_constants::ECMAScript |
                                            regex_constants::icase ),
   file_regex              ( file_regex,    regex_constants::ECMAScript | 
                                            regex_constants::icase ),
   exclude_regex           ( init_exclude_regex( exclude_regex ) ),
   use_exclude_regex       ( exclude_regex.size() > 0  ),
   include_group_select    ( include_group_select )
{


}


/// @details
///    The method returns a regex list, initialized by a string-list.
vector< regex >
Detector::init_exclude_regex( const vector<string> & exclude_regex_list )
{

   vector<regex> return_regex_list;

   for( auto i  = exclude_regex_list.begin();
             i != exclude_regex_list.end();
             i++ )
   {
      if( i->size() > 0 )
      {
         return_regex_list.push_back(regex( *i,
                                            regex_constants::ECMAScript |
                                            regex_constants::icase) );
      }
   }
   return return_regex_list;
}



/// @details
///   If one of the exclude_regexes matches, false is returned.
///   If non of the eclude_regexes matches, but the file_regex, 
///   true is returned. In all other cases, false is returned.
///
bool Detector::use_file( const std::string & file ) const
{
   if( true == use_exclude_regex &&
       exclude_regex_search( file ) )
   {
      BOOST_LOG_TRIVIAL( trace ) << "Excluding " << file;
      return false;
   }
   else if( !regex_search( file, file_regex )  )
   {
      BOOST_LOG_TRIVIAL( trace ) << "Ignoring " << file;
      return false;
   }
   else
   {
      BOOST_LOG_TRIVIAL( trace ) << "Considering " << file;
      return true;
   }
}

bool Detector::exclude_regex_search( std::string path_string ) const
{
    for( auto i = exclude_regex.begin(); i != exclude_regex.end(); i++)
    {
        if(regex_search( path_string, *i ))
        {
            return true;
        }
    }
    return false;
}


std::string Detector::is_include_statement( const std::string & line ) const
{
   smatch match;
   if( regex_search( line, match, include_regex ) )
   {
      if( match.size() == 0 )
      {
         return "";
      }
      else if( include_group_select.size() == 0 )
      {
         return match[ match.size()-1 ];
      }
      else
      {
         for( unsigned int select : include_group_select )
         {
            if( select >= match.size() )
            {
               continue;
            }
            else if( match[ select ].length() > 0 )
            {
               return match[ select ];
            }

         }
         return "";
      }

      BOOST_LOG_TRIVIAL( trace ) << match.size();
      for( unsigned int i=0; i < match.size(); i++ )
      {
         BOOST_LOG_TRIVIAL( trace ) << match[i];
      }

   }
   return "";
}


} // namespace INCLUDE_GARDENER

// vim: filetype=cpp et ts=3 sw=3 sts=3

