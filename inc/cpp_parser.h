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
#ifndef CPP_PARSER_H
#define CPP_PARSER_H

#include <iostream>

namespace INCLUDE_GARDENER
{

/// @author feddischson
/// @brief A Parser realization to analyze c++ includes.
class CPP_Parser
{

public:

   /// @author feddischson
   /// @brief Ctor: not implemented!
   CPP_Parser( ) = delete;


   /// @author feddischson
   /// @brief  Copy ctor: not implemented!
   CPP_Parser(const CPP_Parser & parser ) = delete;


   /// @author feddischson
   /// @brief  Assignment operator: not implemented!
   CPP_Parser& operator= (const CPP_Parser & parser ) = delete;


   /// @author feddischson
   /// @brief  Move constructor: not implemented!
   CPP_Parser( CPP_Parser && rhs ) = delete;


   /// @author feddischson
   /// @brief  Move assignment operator: not implemented!
   CPP_Parser& operator=( CPP_Parser && rhs ) = delete;


   /// @author feddischson
   /// @brief Standard dtor
   ~CPP_Parser() = default;


   /// @author feddischson
   /// @brief Output stream operator overloading.
   friend std::ostream& operator<<( std::ostream& os, const CPP_Parser& parser);

private:


}; // class CPP_Parser

}; // namespace INCLUDE_GARDENER

#endif // CPP_PARSER_H

// vim: filetype=cpp et ts=3 sw=3 sts=3

