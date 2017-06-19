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
#ifndef INCLUDE_PATH_H
#define INCLUDE_PATH_H

#include <iostream>
#include <vector>
#include <memory>
#include <map>

#include "include_entry.h"

namespace INCLUDE_GARDENER
{

/// @author feddischson
/// @brief This class combines severals Maps, which are used during parsing. 
/// @details
///        The map 'cache' is a cache which holds name->path entries.
///        The map 'absolute_map' holds absolute_path->Include_Entry entries.
///        And the map 'blind_map' holds entries which can't be found in the
///        search path.
class Include_Path
{

   /// @brief A Map with string as key and a pointer to the entry as value.
   using I_Map = std::map< std::string, Include_Entry::Ptr >;

   /// @brief A Map with string as key and a string as value.
   using S_Map = std::map< std::string, std::string >;

   /// @brief Pair definition for I_Map.
   using I_Pair = std::pair< const std::string, Include_Entry::Ptr >;

   /// @brief Pair definition for S_Map.
   using S_Pair = std::pair< const std::string, const std::string >;

public:

   /// @brief Smart pointer for Include_Path instances.
   using Ptr = std::shared_ptr< Include_Path >;

   /// @author feddischson
   /// @brief Ctor: not implemented!
   explicit Include_Path( const std::vector< std::string > & paths );


   /// @author feddischson
   /// @brief  Copy ctor: not implemented!
   Include_Path(const Include_Path & p) = delete;


   /// @author feddischson
   /// @brief  Assignment operator: not implemented!
   Include_Path& operator= (const Include_Path & p) = delete;


   /// @author feddischson
   /// @brief  Move constructor: not implemented!
   Include_Path( Include_Path && rhs ) = delete;


   /// @author feddischson
   /// @brief  Move assignment operator: not implemented!
   Include_Path& operator=( Include_Path && rhs ) = delete;


   /// @author feddischson
   /// @brief Standard dtor
   ~Include_Path() = default;


   /// @author feddischson
   /// @brief Output stream operator overloading.
   friend std::ostream& operator<<( std::ostream& os, const Include_Path& p );


   /// @brief Searches within the cache and blind_map if an entry already 
   ///        exists. If exists, the absolute path or blind-name.
   std::string find_cached( const std::string & name );

   /// @brief Forwards call to absolute_map.find()
   I_Map::const_iterator find_abs( const std::string & name );

   /// @brief Forwards call to absolute_map.end()
   I_Map::const_iterator end_abs( void );

   /// @brief Adds cache entry
   void insert_cache( const S_Pair & value );

   /// @brief Adds entry by knowing the absolute file path
   void insert_abs( const I_Pair & value );

   /// @brief Adds entry by not knowing the absolute file path
   void insert_blind( const I_Pair & value );

   /// @brief Returs absolute path if the file can be found, otherwise "".
   std::string is_available( const std::string & name ) const;

private:

   /// @brief Holds all incpude / input paths.
   std::vector< std::string > paths;

   /// @brief Associative array which maps names to the include entrie's paths.
   S_Map cache;

   /// @brief Associative array which maps absolute paths to entries.
   I_Map absolute_map;

   /// @brief Ass. array which maps names to entries which unknown location.
   I_Map blind_map;

}; // class Include_Path

} // namespace INCLUDE_GARDENER

#endif // INCLUDE_PATH_H

// vim: filetype=cpp et ts=3 sw=3 sts=3

