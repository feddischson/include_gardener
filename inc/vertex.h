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
#ifndef VERTEX_H
#define VERTEX_H

#include <memory>
#include <map>
#include <string>

namespace INCLUDE_GARDENER {

/// @brief Vertex class
class Vertex {
 public:
  /// @brief Smart pointer for Vertex
  using Ptr = std::shared_ptr<Vertex>;

  /// @brief A Map with string as key and a pointer to the Vertex as value.
  using Map = std::map< std::string, Ptr >;

  /// @brief Ctor
  /// @param abs_path absolute path
  /// @param rel_path relative path (seen from execution of this tool)
  /// @param name file name including file ending
  Vertex(const std::string &name, const std::string &abs_path = "",
         const std::string &rel_path = "");

  /// @brief  Copy ctor: not implemented!
  Vertex(const Vertex &other) = delete;

  /// @brief  Assignment operator: not implemented!
  Vertex &operator=(const Vertex &rhs) = delete;

  /// @brief  Move constructor: not implemented!
  Vertex(Vertex &&rhs) = delete;

  /// @brief  Move assignment operator: not implemented!
  Vertex &operator=(Vertex &&rhs) = delete;

  /// @brief Default dtor
  ~Vertex() = default;

 private:
  /// @brief file name, including file ending
  const std::string name;

  /// @brief Absolute path
  const std::string abs_path;

  /// @brief Relative path
  /// (seen from execution of this tool)
  const std::string rel_path;

};  // class Vertex

}  // namespace INCLUDE_GARDENER

#endif  // VERTEX_H

// vim: filetype=cpp et ts=2 sw=2 sts=2
