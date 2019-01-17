// Include-Gardener
//
// Copyright (C) 2019  Christian Haettich [feddischson]
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
#ifndef INPUT_FILES_H
#define INPUT_FILES_H

#include <list>
#include <string>

#include "solver.h"

namespace INCLUDE_GARDENER {

/// @brief Abstract Input_Files class.
/// @details
///     Derived classes shall give the possibility to provide a list of
///     input files, e.g. by searching the file system.
/// @author feddischson
class Input_Files {
 public:
  /// @brief String-list iterator alias.
  using Itr = std::list<std::string>::const_iterator;

  /// @brief Initializes the files member only.
  explicit Input_Files(std::list<std::string> files = {});

  /// @brief Copy ctor: not implemented!
  Input_Files(const Input_Files &other) = default;

  /// @brief Assignment operator: not implemented!
  Input_Files &operator=(const Input_Files &rhs) = default;

  /// @brief Move constructor: not implemented!
  Input_Files(Input_Files &&rhs) = default;

  /// @brief Move assignment operator: not implemented!
  Input_Files &operator=(Input_Files &&rhs) = default;

  /// @brief Default dtor
  virtual ~Input_Files() = default;

  /// @brief Shall put all input files in the private storage files.
  virtual void get(Solver::Ptr solver) = 0;

  /// @brief Returns the beginning of the files.
  Itr begin() const;

  /// @brief Returns the end of the files.
  Itr end() const;

 protected:
  /// @brief List of detected files.
  std::list<std::string> files;

};  // class Input_Files

}  // namespace INCLUDE_GARDENER

#endif  // INPUT_FILES_H

// vim: filetype=cpp et ts=2 sw=2 sts=2
