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
/// @brief Template class
class Include_Path
{

   /// @brief A Map with string as key and a pointer to the entry as value.
   using I_Map = std::map< std::string, Include_Entry::Ptr >;

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


   /// @brief Forwards call to name_map.find()
   I_Map::const_iterator find( const std::string & name );

   /// @brief Forwards call to name_map.end()
   I_Map::const_iterator end( void );

   /// @brief Forwards call to name_map.insert()
   std::pair< I_Map::iterator, bool > insert(
         const std::pair< const std::string, Include_Entry::Ptr > & value );

private:

   /// @brief Holds all incpude / input paths.
   std::vector< std::string > paths;

   /// @brief A associative array which maps names to the include entries.
   I_Map name_map;

}; // class Include_Path

} // namespace INCLUDE_GARDENER

#endif // INCLUDE_PATH_H

// vim: filetype=cpp et ts=3 sw=3 sts=3

