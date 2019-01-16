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
#include <regex>

#include "graph.h"
#include "solver.h"
#include "vertex.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using INCLUDE_GARDENER::Edge;
using INCLUDE_GARDENER::Edge_Descriptor;
using INCLUDE_GARDENER::Solver;
using INCLUDE_GARDENER::Vertex;

using std::ostringstream;
using std::string;
using std::vector;

class Solver_Test : public ::testing::Test {};

class Mock_Solver2 : public Solver {
  vector<string> get_statement_regex() const override { return {}; }

  string get_file_regex() const override { return string(); }

  // NOLINTNEXTLINE
  void extract_options(const boost::program_options::variables_map &) override {
  }

 public:
  Mock_Solver2() = default;
  // NOLINTNEXTLINE
  void add_edge(const std::string &src, const std::string &dst, unsigned int,
                unsigned int line_no) override {
    Edge_Descriptor edge;
    bool b;
    boost::tie(edge, b) = boost::add_edge_by_label(src, dst, graph);
    graph[edge] = Edge{static_cast<int>(line_no)};
  }

  Vertex::Ptr find_vertex(const string &key) {
    auto x = vertexes.find(key);
    if (x != vertexes.end()) {
      return x->second;
    } 
    return nullptr;
  }
};

// NOLINTNEXTLINE
TEST_F(Solver_Test, adding_vertex) {
  auto s = std::make_shared<Mock_Solver2>();
  s->add_vertex("x", "y");
  auto result = s->find_vertex("y");
  EXPECT_NE(result, nullptr);
  EXPECT_EQ(result->get_name(), "x");
}

// NOLINTNEXTLINE
TEST_F(Solver_Test, adding_vertex_with_empty_abs_path) {
  auto s = std::make_shared<Mock_Solver2>();
  s->add_vertex("x", "");
  auto result = s->find_vertex("x");
  EXPECT_NE(result, nullptr);
  EXPECT_EQ(result->get_name(), "x");
}

// NOLINTNEXTLINE
TEST_F(Solver_Test, writing_dot) {
  using ::testing::_;
  string dot_expectation = R"(digraph G {
0[label="x"];
1[label="y"];
0->1 [label="line 0"];
}
)";

  auto s = std::make_shared<Mock_Solver2>();
  s->add_vertex("x", "x");
  s->add_vertex("y", "y");

  auto r1 = s->find_vertex("x");
  EXPECT_NE(r1, nullptr);
  EXPECT_EQ(r1->get_name(), "x");

  auto r2 = s->find_vertex("y");
  EXPECT_NE(r2, nullptr);
  EXPECT_EQ(r2->get_name(), "y");

  s->add_edge("x", "y", 0, 0);
  ostringstream res;
  s->write_graph("dot", res);
  EXPECT_EQ(res.str(), dot_expectation);
}

// NOLINTNEXTLINE
TEST_F(Solver_Test, writing_graphml) {
  using ::testing::_;
  string xml_expectation = R"(<?xml version="1.0" encoding="UTF-8"?>
<graphml xmlns="http://graphml.graphdrawing.org/xmlns" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:schemaLocation="http://graphml.graphdrawing.org/xmlns http://graphml.graphdrawing.org/xmlns/1.0/graphml.xsd">
  <key id="key0" for="edge" attr.name="line" attr.type="int" />
  <key id="key1" for="node" attr.name="name" attr.type="string" />
  <graph id="G" edgedefault="directed" parse.nodeids="free" parse.edgeids="canonical" parse.order="nodesfirst">
    <node id="n0">
      <data key="key1">x</data>
    </node>
    <node id="n1">
      <data key="key1">y</data>
    </node>
    <edge id="e0" source="n0" target="n1">
      <data key="key0">0</data>
    </edge>
  </graph>
</graphml>
)";

  auto s = std::make_shared<Mock_Solver2>();
  s->add_vertex("x", "x");
  s->add_vertex("y", "y");

  auto r1 = s->find_vertex("x");
  EXPECT_NE(r1, nullptr);
  EXPECT_EQ(r1->get_name(), "x");

  auto r2 = s->find_vertex("y");
  EXPECT_NE(r2, nullptr);
  EXPECT_EQ(r2->get_name(), "y");

  s->add_edge("x", "y", 0, 0);

  ostringstream res1;
  s->write_graph("xml", res1);
  EXPECT_EQ(res1.str(), xml_expectation);

  ostringstream res2;
  s->write_graph("graphml", res2);
  EXPECT_EQ(res2.str(), xml_expectation);
}

// NOLINTNEXTLINE
TEST_F(Solver_Test, writing_unknown_format) {
  using ::testing::_;
  string expectation;

  auto s = std::make_shared<Mock_Solver2>();
  s->add_vertex("x", "x");
  s->add_vertex("y", "y");

  auto r1 = s->find_vertex("x");
  EXPECT_NE(r1, nullptr);
  EXPECT_EQ(r1->get_name(), "x");

  auto r2 = s->find_vertex("y");
  EXPECT_NE(r2, nullptr);
  EXPECT_EQ(r2->get_name(), "y");

  s->add_edge("x", "y", 0, 0);
  ostringstream res;
  s->write_graph("unknown", res);
  EXPECT_EQ(res.str(), expectation);
}

// vim: filetype=cpp et ts=2 sw=2 sts=2
