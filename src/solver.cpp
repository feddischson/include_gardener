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
#include "solver.h"

#include <iostream>
#include <memory>
#include <string>

#include <boost/graph/graphml.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/log/trivial.hpp>
#include <boost/property_map/transform_value_property_map.hpp>

#include "solver_c.h"

using std::make_shared;
using std::string;

namespace INCLUDE_GARDENER {

void Solver::add_vertex(const std::string& name, const std::string& abs_path) {
  using std::pair;
  using std::string;
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
    auto v = make_shared<Vertex>(name, abs_path);
    vertexes.insert(pair<string, Vertex::Ptr>(key, v));
    boost::add_vertex(key, graph);
    graph[key] = v;
  }
}

void Solver::add_options(boost::program_options::options_description* options) {
  Solver_C::add_options(options);
}

Solver::Ptr Solver::get_solver(const std::string& name) {
  if (name == "c") {
    return std::dynamic_pointer_cast<Solver>(std::make_shared<Solver_C>());
  }
  return nullptr;
}

void Solver::write_graph(const string& format, ostream& os) {
  // prepare the name-map for graphviz output generation
  auto name_map = boost::make_transform_value_property_map(
      [](Vertex::Ptr v) { return v->get_name(); },
      get(boost::vertex_bundle, graph));

  if ("dot" == format) {
    write_graphviz(os, graph, make_vertex_writer(name_map),
                   make_edge_writer(boost::get(&Edge::line, graph)));
  } else if ("xml" == format || "graphml" == format) {
    boost::dynamic_properties dp;
    dp.property("line", boost::get(&Edge::line, graph));
    dp.property("name", name_map);
    write_graphml(os, graph, dp);
  }
}

}  // namespace INCLUDE_GARDENER

// vim: filetype=cpp et ts=2 sw=2 sts=2
