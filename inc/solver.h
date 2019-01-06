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
#ifndef SOLVER_H
#define SOLVER_H

#include <memory>

#include "graph.h"
#include "vertex.h"

namespace INCLUDE_GARDENER {

/// @brief Solver class
class Solver {
 public:
  /// @brief Smart pointer for Solver
  using Ptr = std::shared_ptr<Solver>;

  /// @brief Default ctor.
  explicit Solver(Graph *graph);

  /// @brief  Copy ctor: not implemented!
  Solver(const Solver &other) = delete;

  /// @brief  Assignment operator: not implemented!
  Solver &operator=(const Solver &rhs) = delete;

  /// @brief  Move constructor: not implemented!
  Solver(Solver &&rhs) = delete;

  /// @brief  Move assignment operator: not implemented!
  Solver &operator=(Solver &&rhs) = delete;

  /// @brief Default dtor
  virtual ~Solver() = default;

  /// @brief Adds a vertex / entry.
  virtual void add_vertex(const std::string &name,
                          const std::string &abs_path = "",
                          const std::string &rel_path = "");

  /// @brief Shall add an edge.
  virtual void add_edge() = 0;

  /// @brief Shall return the statements which shall be
  ///        detected as regex.
  virtual std::vector<std::string> get_statements() = 0;

 protected:
  /// @brief Pointer to the global graph instance.
  Graph *graph;

  /// @brief Storage of all added vertexes.
  Vertex::Map vertexes;

 private:
};  // class Solver

}  // namespace INCLUDE_GARDENER

#endif  // SOLVER_H

// vim: filetype=cpp et ts=2 sw=2 sts=2
