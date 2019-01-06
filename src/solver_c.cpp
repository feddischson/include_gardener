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
#include "solver_c.h"

#include <string>
#include <vector>

namespace INCLUDE_GARDENER {

Solver_C::Solver_C(Graph* graph) : Solver(graph) {}

std::vector<std::string> Solver_C::get_statements() {
  std::vector<std::string> statements = {
      "\\s*#\\s*(include|import)\\s+\"(\\S+)\"",
      "\\s*#\\s*(include|import)\\s+<(\\S+)>"};
  return statements;
}

void Solver_C::add_edge(){

}


}  // namespace INCLUDE_GARDENER

// vim: filetype=cpp et ts=2 sw=2 sts=2
