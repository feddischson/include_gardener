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
#ifndef DETECTOR_H
#define DETECTOR_H

#include <iostream>
#include <vector>
#include <regex>

namespace INCLUDE_GARDENER
{

/// @author feddischson
/// @brief This class does all the pattern detection to check,
///        if a file shall be considered or not, and to serach
///        for include statements.
class Detector
{

public:

   /// @brief Smart pointer for Detector
   using Ptr = std::shared_ptr< Detector >;


   /// @author feddischson
   /// @brief  Copy ctor: default implementation is fine!
   Detector(const Detector & dtc ) = default;


   /// @author feddischson
   /// @brief  Assignment operator: not implemented!
   Detector& operator= (const Detector & dtc ) = delete;


   /// @author feddischson
   /// @brief  Move constructor: not implemented!
   Detector( Detector && rhs ) = delete;


   /// @author feddischson
   /// @brief  Move assignment operator: not implemented!
   Detector& operator=( Detector && rhs ) = delete;


   /// @author feddischson
   /// @brief Standard dtor
   ~Detector() = default;


   /// @brief Returns true if the file shall be considered, otherwise false.
   bool use_file( const std::string & file ) const;


   /// @brief Returns true if the statement is an include statement.
   std::string is_include_statement( const std::string & line ) const;

   /// @brief Returns a detector instance.
   static Ptr get_detector(
               const std::string                & include_regex,
               const std::string                & file_regex,
               const std::vector<std::string>   & exclude_regex,
               const std::vector<unsigned int>  & include_group_select
         );

private:

   /// @author feddischson
   /// @brief Ctor: not implemented!
   Detector(   const std::string                & include_regex,
               const std::string                & file_regex,
               const std::vector<std::string>   & exclude_regex,
               const std::vector<unsigned int>  & include_group_select
         );


   /// @brief Helper function to check if a file should be excluded.
   bool exclude_regex_search( std::string ) const;

   /// @brief Helper function to initialize the exclude_regex vector.
   std::vector< std::regex > init_exclude_regex(
         const std::vector< std::string > & exclude_regex_list );


   const std::regex include_regex;

   /// @brief Regular expression to check if a file shall be used.
   const std::regex file_regex;

   /// @brief List of regular expressions to check if a file shall not
   ///        be used.
   const std::vector<std::regex> exclude_regex;

   bool use_exclude_regex;

   std::vector< unsigned int > include_group_select;


}; // class Detector

} // namespace INCLUDE_GARDENER

#endif // DETECTOR_H

// vim: filetype=cpp et ts=3 sw=3 sts=3

