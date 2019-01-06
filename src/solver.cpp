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
#include "solver.h"

#include <boost/log/trivial.hpp>

namespace INCLUDE_GARDENER {

Solver::Solver(Graph* graph) : graph(graph) {}

void Solver::add_vertex(const std::string& name, const std::string& abs_path,
                        const std::string& rel_path) {
  // TODO: handle empty abs-path

  if (vertexes.find(abs_path) != vertexes.end()) {
    BOOST_LOG_TRIVIAL(warning)
        << "No need to add a new vertex, "
        << "vertex already exists: " << name << ", " << abs_path;

  } else {
    Vertex::Ptr v(new Vertex(name, abs_path, rel_path));
    vertexes.insert(std::pair<std::string,Vertex::Ptr>(abs_path, v));

  }
}

}  // namespace INCLUDE_GARDENER

// vim: filetype=cpp et ts=2 sw=2 sts=2
