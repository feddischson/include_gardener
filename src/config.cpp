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
#include "config.h"

#include <stdexcept>

#include <boost/property_tree/json_parser.hpp>
#include <boost/log/trivial.hpp>

namespace pt = boost::property_tree;

namespace INCLUDE_GARDENER
{


Config::Config( const std::string & cfg_path )
   : cfg_path( cfg_path )
{
}

void Config::read_cfg( void )
{
   pt::read_json( cfg_path, root );
   for( pt::ptree::value_type &lang : root.get_child("languages"))
   {
      std::string name = lang.first;
      languages.push_back( name );
   }
   check_cfg( );
}

std::list< std::string > Config::get_languages( void ) const
{
   return languages;
}

std::string Config::get_file_detection( const std::string & language ) const
{
   return root.get< std::string >
      ( "languages." + language + ".file_detection" );
}

std::string Config::get_include_detection( const std::string & language ) const
{
   return root.get< std::string >
      ( "languages." + language + ".include_detection" );
}

std::string Config::get_cfg_path( void ) const
{
   return cfg_path;
}

void Config::check_cfg( void ) const
{
   // check if there is a 'languages' entry
   if( root.count( "languages" ) == 0 )
   {
      BOOST_LOG_TRIVIAL( error ) << "No languages are provided";
      throw std::logic_error( "No languages are provided" );
   }

   // loop over all languages and check
   // if there is a file_detection or include_detection
   for( pt::ptree::value_type const &lang : root.get_child("languages"))
   {
      std::string name   = lang.first;
      auto config        = lang.second;
      bool found_file    = false;
      bool found_include = false;

      for( pt::ptree::value_type const &cfg_entry : lang.second )
      {
         std::string cfg_name = cfg_entry.first;
         std::string cfg_data = cfg_entry.second.get_value<std::string>();


         //
         // check the config entry name
         //
         if( cfg_name == "file_detection" )
         {
            found_file = true;
         }
         else if( cfg_name == "include_detection" )
         {
            found_include = true;
         }
         else
         {
            BOOST_LOG_TRIVIAL( error ) << "Only 'file_detection' and "
                                       << "'include_detection' is supported";
            throw std::logic_error( "Invalid configuration: "
                                    + name
                                    + "."
                                    + cfg_name );
         }

         //
         // check the regex, the length must not be zero
         //
         if( cfg_data.length() == 0 )
         {
            BOOST_LOG_TRIVIAL( error ) << "Wrong configuration of"
                                       << name << "." << cfg_name << ": "
                                       << "Size of regex must be > 0";
            throw std::logic_error( "Invalid configuration setup: "
                                    + name
                                    + "."
                                    + cfg_name );
         }
      }
      if( !( found_file && found_include ) )
      {
            BOOST_LOG_TRIVIAL( error ) << "Wrong configuration of "
                                       << name << ": "
                                       << "Both, include_detection" 
                                       << " and file_detection"
                                       << " must be provided";
            throw std::logic_error( "Invalid configuration setup: "
                                    + name );

      }
   }
}


Config::Ptr Config::get_cfg( const std::string & cfg_path )
{
   Ptr p( new Config( cfg_path  ) );
   p->read_cfg( );
   return p;
}


std::ostream& operator<<( std::ostream& os, const Config & conf )
{
   os  << "Config ("  << reinterpret_cast< const void* >( &conf ) << "): "
       << std::endl;
   os << "  Languages:" << std::endl;
   for( auto l : conf.get_languages() )
   {
      os << "     - " << l << std::endl
         << "       file-detection:    "
         << conf.get_file_detection( l ) << std::endl
         << "       include-detection: " 
         << conf.get_include_detection( l ) << std::endl;
   }

   return os;
}


} // namespace INCLUDE_GARDENER

// vim: filetype=cpp et ts=3 sw=3 sts=3

