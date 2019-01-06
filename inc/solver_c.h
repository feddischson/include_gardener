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
#ifndef SOLVER_C_H
#define SOLVER_C_H

#include <memory>

#include "solver.h"

namespace INCLUDE_GARDENER {

/// @brief Solver_C class
class Solver_C : public Solver {
 public:
  /// @brief Smart pointer for Solver_C
  using Ptr = std::shared_ptr<Solver_C>;

  /// @brief Ctor: only calls super ctor.
  explicit Solver_C(Graph *graph);

  /// @brief  Copy ctor: not implemented!
  Solver_C(const Solver_C &other) = delete;

  /// @brief  Assignment operator: not implemented!
  Solver_C &operator=(const Solver_C &rhs) = delete;

  /// @brief  Move constructor: not implemented!
  Solver_C(Solver_C &&rhs) = delete;

  /// @brief  Move assignment operator: not implemented!
  Solver_C &operator=(Solver_C &&rhs) = delete;

  /// @brief Default dtor
  virtual ~Solver_C() = default;

  /// @brief Adds an edge to the graph.
  virtual void add_edge();

  /// @brief Returns the statements which shall be
  ///        detected as regex.
  virtual std::vector<std::string> get_statements();

 private:
};  // class Solver_C

}  // namespace INCLUDE_GARDENER

#endif  // SOLVER_C_H

// vim: filetype=cpp et ts=2 sw=2 sts=2
