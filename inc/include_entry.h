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

#include <iostream>
#include <map>

namespace INCLUDE_GARDENER
{

/// @author feddischson
/// @brief An include entry representation.
class Include_Entry
{

public:

   /// @author feddischson
   /// @brief Ctor: not implemented!
   Include_Entry( ) = delete;


   /// @author feddischson
   /// @brief  Copy ctor: not implemented!
   Include_Entry(const Include_Entry & a_template_) = delete;


   /// @author feddischson
   /// @brief  Assignment operator: not implemented!
   Include_Entry& operator= (const Include_Entry & a_template_) = delete;


   /// @author feddischson
   /// @brief  Move constructor: not implemented!
   Include_Entry( Include_Entry && rhs ) = delete;


   /// @author feddischson
   /// @brief  Move assignment operator: not implemented!
   Include_Entry& operator=( Include_Entry && rhs ) = delete;


   /// @author feddischson
   /// @brief Standard dtor
   ~Include_Entry() = default;


   /// @author feddischson
   /// @brief Output stream operator overloading.
   friend std::ostream& operator<<( std::ostream& os,
         const Include_Entry& entry);

private:

   std::string name;

   std::string rel_path;

   std::string abs_path;

   std::map< std::string, std::string > children;

}; // class Include_Entry

}; // namespace INCLUDE_GARDENER

#endif // INCLUDE_ENTRY

// vim: filetype=cpp et ts=3 sw=3 sts=3

