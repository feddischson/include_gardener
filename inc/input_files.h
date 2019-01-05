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
#ifndef INPUT_FILES_H
#define INPUT_FILES_H

#include <vector>
#include <string>

namespace INCLUDE_GARDENER {

/// @brief Input_Files class
class Input_Files {
 public:
  /// @brief Ctor: not implemented!
  Input_Files() = default;

  /// @brief  Copy ctor: not implemented!
  Input_Files(const Input_Files &other) = default;

  /// @brief  Assignment operator: not implemented!
  Input_Files &operator=(const Input_Files &rhs) = default;

  /// @brief  Move constructor: not implemented!
  Input_Files(Input_Files &&rhs) = default;

  /// @brief  Move assignment operator: not implemented!
  Input_Files &operator=(Input_Files &&rhs) = default;

  /// @brief Default dtor
  ~Input_Files() = default;

  /// @brief Shall put all input files in the private storage files.
  virtual void get() = 0;

  /// @brief Processes all input files of the private storage files.
  void run();

 private:
  /// @brief File storage vector.
  std::vector<std::string> files;

};  // class Input_Files

}  // namespace INCLUDE_GARDENER

#endif  // INPUT_FILES_H

// vim: filetype=cpp et ts=2 sw=2 sts=2
