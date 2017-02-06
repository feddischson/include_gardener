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
#ifndef INCLUDE_ENTRY
#define INCLUDE_ENTRY

#include <memory>
#include <map>
#include <iostream>
#include <vector>

namespace INCLUDE_GARDENER
{

/// @author feddischson
/// @brief An include entry representation.
class Include_Entry
{

public:

   /// @brief Smart pointer for Include_Entries
   using Ptr = std::shared_ptr< Include_Entry >;

   /// @brief A Map with string as key and a pointer to the entry as value.
   using Map = std::map< std::string, Ptr >;

   /// @author feddischson
   /// @brief Ctor: Initializes name. Rel_paths and abs_paths with
   ///              is initialized with no entries.
   Include_Entry( const std::string & name );

   /// @author feddischson
   /// @brief Ctor: Initializes name as well as rel_paths and abs_paths with
   ///              real_path and abs_path.
   Include_Entry( const std::string & name,
                  const std::string & rel_path,
                  const std::string & abs_path );


   /// @author feddischson
   /// @brief  Copy ctor: not implemented!
   Include_Entry(const Include_Entry & entry ) = delete;


   /// @author feddischson
   /// @brief  Assignment operator: not implemented!
   Include_Entry& operator= (const Include_Entry & entry ) = delete;


   /// @author feddischson
   /// @brief  Move constructor: not implemented!
   Include_Entry( Include_Entry && rhs ) = delete;


   /// @author feddischson
   /// @brief  Move assignment operator: not implemented!
   Include_Entry& operator=( Include_Entry && rhs ) = delete;


   /// @author feddischson
   /// @brief Standard dtor
   ~Include_Entry() = default;

   /// @brief Adds the relative and absolute path to the internal
   ///        vector.
   void add_path_info( const std::string & rel_path,
                       const std::string & abs_path );

   /// @author feddischson
   /// @brief Output stream operator overloading.
   friend std::ostream& operator<<( std::ostream& os,
         const Include_Entry& entry);

   /// @brief Returns the name
   std::string get_name( ) const;

   /// @brief Name of the include entry
   std::string name;

private:


   /// @brief All found relative paths to this entry
   std::vector< std::string > rel_paths;

   /// @brief All found absolute paths to this entry
   std::vector< std::string > abs_paths;

}; // class Include_Entry

}; // namespace INCLUDE_GARDENER

#endif // INCLUDE_ENTRY

// vim: filetype=cpp et ts=3 sw=3 sts=3

