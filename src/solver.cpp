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

#include "solver_c.h"

#include <memory>

#include <boost/log/trivial.hpp>

namespace INCLUDE_GARDENER {

Solver::Solver(Graph* graph) : graph(graph) {}

void Solver::add_vertex(const std::string& name, const std::string& abs_path) {
  using namespace std;
  // if abs_path is empty, take the name as key!
  const string& key = abs_path.length() == 0 ? name : abs_path;

  if (vertexes.find(key) != vertexes.end()) {
    BOOST_LOG_TRIVIAL(trace) << "No need to add a new vertex, "
                             << "vertex already exists: "
                             << "\n"
                             << "    key = " << key << ", "
                             << "\n"
                             << "    abs_path = " << abs_path << "\n"
                             << "    name = " << name;

  } else {
    Vertex::Ptr v(new Vertex(name, abs_path));
    vertexes.insert(pair<string, Vertex::Ptr>(key, v));
    boost::add_vertex(key, *graph);
    (*graph)[key] = v;
  }
}

void Solver::add_options(boost::program_options::options_description* options) {
  Solver_C::add_options(options);
}

Solver::Ptr Solver::get_solver(const std::string& name, Graph* g) {
  if (name == "c") {
    return std::dynamic_pointer_cast<Solver>(std::make_shared<Solver_C>(g));
  }
  return nullptr;
}

}  // namespace INCLUDE_GARDENER

// vim: filetype=cpp et ts=2 sw=2 sts=2
