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
#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include <string>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/labeled_graph.hpp>

#include "vertex.h"

namespace INCLUDE_GARDENER {

static constexpr int START_LINE = -1;

/// @brief Edge Properties.
/// @author feddischson
struct Edge {
   Edge() = default;
   explicit Edge(int l) : line(l) {}
   int line = START_LINE;  ///< The line number where
};                         ///  the include statement is defined.

/// @brief Graph definition.
/// @author feddischson
using Graph_ = boost::adjacency_list<boost::listS, boost::vecS,
                                     boost::directedS, Vertex::Ptr, Edge>;

/// @brief Labeled-graph definition
/// @author feddischson
using Graph = boost::labeled_graph<Graph_, std::string>;

/// @brief Aliases for handling vertices and edges of a graph
/// @author feddischson
//
/// @{
using Vertex_Descriptor = boost::graph_traits<Graph>::vertex_descriptor;
using Edge_Descriptor = boost::graph_traits<Graph>::edge_descriptor;
using Edge_Iterator = boost::graph_traits<Graph>::edge_iterator;
/// @}

/// @brief Graphviz writer class for vertices.
/// @author feddischson
template <class T>
class Vertex_Writer {
  public:
   explicit Vertex_Writer(T t) : t(t) {}
   template <class T_v>
   // NOLINTNEXTLINE(fuchsia-overloaded-operator)
   void operator()(std::ostream& out, const T_v& vertex) const {
      out << "[label=\"" << t[vertex] << "\"]";
   }

  private:
   T t;
};

/// @brief Helper function for graphviz vertices writer class
/// @author feddischson
template <class T>
inline Vertex_Writer<T> make_vertex_writer(T t) {
   return Vertex_Writer<T>(t);
}

/// @brief Graphviz writer class for edges.
/// @author feddischson
template <class T>
class Edge_Writer {
  public:
   explicit Edge_Writer(T t) : t(t) {}
   template <class T_Edge>
   // NOLINTNEXTLINE(fuchsia-overloaded-operator)
   void operator()(std::ostream& out, const T_Edge& e) const {
      out << "[label=\""
          << "line " << t[e] << "\"]";
   }

  private:
   T t;
};

/// @brief Helper function for graphviz edge writer class.
/// @author feddischson
template <class T>
inline Edge_Writer<T> make_edge_writer(T t) {
   return Edge_Writer<T>(t);
}

}  // namespace INCLUDE_GARDENER

#endif  // GRAPH_H

// vim: filetype=cpp et ts=3 sw=3 sts=3
