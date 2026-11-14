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
#ifndef %GUARD%
#define %GUARD%

namespace INCLUDE_GARDENER {

/// @brief %CLASS% class
class %CLASS% {
 public:
  /// @brief Ctor: not implemented!
  %CLASS% () = delete;

  /// @brief  Copy ctor: not implemented!
  %CLASS% (const %CLASS% &other) = delete;

  /// @brief  Assignment operator: not implemented!
  %CLASS% &operator=(const %CLASS% &rhs) = delete;

  /// @brief  Move constructor: not implemented!
  %CLASS% (%CLASS% &&rhs) = delete;

  /// @brief  Move assignment operator: not implemented!
  %CLASS% &operator=(%CLASS% &&rhs) = delete;

  /// @brief Default dtor
  ~ %CLASS% () = default;

 private:
};  // class %CLASS%

}  // namespace INCLUDE_GARDENER

#endif  // %GUARD%

// vim: filetype=cpp et ts=2 sw=2 sts=2
