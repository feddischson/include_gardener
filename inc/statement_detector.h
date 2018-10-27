// Include-Gardener
//
// Copyright (C) 2018  Christian Haettich [feddischson]
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
#ifndef STATEMENT_DETECTOR_H
#define STATEMENT_DETECTOR_H

#include <regex>
#include <string>
#include <vector>

namespace INCLUDE_GARDENER {

/// @brief Statement_Detector class
class Statement_Detector {
 public:
  /// @brief Default ctor.
  Statement_Detector(const std::vector<std::string> &statement_list);

  /// @brief  Default copy ctor.
  Statement_Detector(const Statement_Detector &other) = default;

  /// @brief  Default assignment operator.
  Statement_Detector &operator=(const Statement_Detector &rhs) = default;

  /// @brief  Default move constructor.
  Statement_Detector(Statement_Detector &&rhs) = default;

  /// @brief  Default move assignment operator.
  Statement_Detector &operator=(Statement_Detector &&rhs) = default;

  /// @brief Default dtor
  ~Statement_Detector() = default;

  /// @brief Returns list of statements (as regex)
  std::vector<std::regex> get_statements();

 private:
  /// @brief Internal vector of statements.
  std::vector<std::regex> statements;

};  // class Statement_Detector

}  // namespace INCLUDE_GARDENER

#endif  // STATEMENT_DETECTOR_H

// vim: filetype=cpp et ts=2 sw=2 sts=2
