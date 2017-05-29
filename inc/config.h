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
#ifndef CONFIG_H
#define CONFIG_H

#include <memory>
#include <list>

#include <boost/property_tree/ptree.hpp>
#include <iostream>

namespace INCLUDE_GARDENER
{

/// @author feddischson
/// @brief  Provides abstract access to the config data.
///
/// @details The data format must be json.
///          This class expects a language node which has at least one
///          entry. Each entry must have a file_detection and include_detection
///          entry.
///          Example:
///             {
///                  "languages":
///                  {
///                      "cpp" :
///                      {
///                         "file_detection" : ".*\.(h|hpp|hh|c|cc|cpp)$",
///                         "include_detection" : "#[\s+]?include\\s+[\"|<](.*?)[\"|>]"
///                      },
///                      "python":
///                      {
///                         "file_detection" : ".*\.py$",
///                         "include_detection" : "(from)?[\s+]?(.*?)import[\s+]?(.*)?"
///                      }
///                  }
///             }
///
class Config
{

public:

   /// @brief Smart pointer for Config
   using Ptr = std::shared_ptr< Config >;


   /// @author feddischson
   /// @brief Ctor: Default implementation.
   Config( ) = delete;


   /// @author feddischson
   /// @brief  Copy ctor: Default implementation.
   Config(const Config & cfg) = default;


   /// @author feddischson
   /// @brief  Assignment operator: Default implementation.
   Config& operator= (const Config & cfg) = default;


   /// @author feddischson
   /// @brief  Move constructor: not implemented!
   Config( Config && rhs ) = default;


   /// @author feddischson
   /// @brief  Move assignment operator: Default implementation.
   Config& operator=( Config && rhs ) = default;


   /// @author feddischson
   /// @brief Standard dtor
   ~Config() = default;


   /// @author feddischson
   /// @brief Output stream operator overloading.
   friend std::ostream& operator<<( std::ostream& os, const Config& tpl);


   /// @brief Returns config instance for a given path.
   static Ptr get_cfg( const std::string & cfg_path );

   /// @brief Returns a list of configured  languages
   std::list< std::string > get_languages( void ) const;

   /// @brief Access function to the file_detection configuration.
   std::string get_file_detection( const std::string & language ) const;

   /// @brief Access function to the include_detection configuration.
   std::string get_include_detection( const std::string & language ) const;

   /// @brief Returns the configuration path.
   std::string get_cfg_path( void ) const;

   /// @brief Access function to the include_group_select configuration.
   std::vector<unsigned int> get_include_group_select(
         const std::string & language ) const;

   /// @brief Returns true if the langauge is supported by the config.
   bool supports_language( const std::string & language ) const;


protected:
   /// @brief This ctor only initialiyes cfg_path.
   Config( const std::string & cfg_path );

   /// @brief Validates the configuration for correctness and completeness
   void check_cfg( void ) const;

   /// @brief Reads the configuration from cfg_path,calls check_cfg and fills
   ///       languages.
   virtual void read_cfg( void );

   /// @brief List of supported languages, filled by read_cfg.
   std::list< std::string > languages;

   /// @brief The root node of the configuration
   boost::property_tree::ptree root;
private:

   /// @brief File path to the configuration file.
   std::string cfg_path;


}; // class Config

} // namespace INCLUDE_GARDENER

#endif // CONFIG_H

// vim: filetype=cpp et ts=3 sw=3 sts=3

