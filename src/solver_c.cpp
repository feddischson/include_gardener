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
#include "solver_c.h"

#include <string>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/log/trivial.hpp>

namespace INCLUDE_GARDENER {

namespace po = boost::program_options;
using std::mutex;
using std::string;
using std::unique_lock;
using std::vector;

vector<string> Solver_C::get_statement_regex() const {
  vector<string> regex_str = {R"(\s*#\s*(?:include|import)\s+\"(\S+)\")",
                              R"(\s*#\s*(?:include|import)\s+<(\S+)>)"};
  return regex_str;
}

string Solver_C::get_file_regex() const { return string("(.*)\\.(c|h)$"); }

void Solver_C::add_options(po::options_description *options) {
  options->add_options()("c-include-path,I",
                         po::value<vector<string> >()->composing(),
                         "c-include path");
}

void Solver_C::extract_options(const po::variables_map &vm) {
  if (vm.count("c-include-path") != 0u) {
    include_paths = vm["c-include-path"].as<vector<string> >();
  }
  BOOST_LOG_TRIVIAL(trace) << "c-include-paths:   ";
  for (const auto &p : include_paths) {
    BOOST_LOG_TRIVIAL(trace) << "    " << p;
  }
}

void Solver_C::add_edge(const string &src_path, const string &statement,
                        unsigned int idx, unsigned int line_no) {
  using boost::filesystem::path;
  using boost::filesystem::operator/;
  unique_lock<mutex> glck(graph_mutex);
  BOOST_LOG_TRIVIAL(trace) << "add_edge: " << src_path << " -> " << statement
                           << ", idx = " << idx << ", line_no = " << line_no;

  if (0 == idx) {
    // construct relative path from the same directory as
    // the file that contains the #include statement.
    path base = path(src_path).parent_path();
    path dst_path = base / statement;
    if (exists(dst_path)) {
      dst_path = canonical(dst_path);
      BOOST_LOG_TRIVIAL(trace) << "   |>> Relative Edge";
      insert_edge(src_path, dst_path.string(), statement, line_no);
      return;
    }
  }

  // search in preconfigured list of standard system directories
  for (const auto &i_path : include_paths) {
    path dst_path = i_path / statement;
    if (exists(dst_path)) {
      dst_path = canonical(dst_path);
      BOOST_LOG_TRIVIAL(trace) << "   |>> Absolute Edge";
      insert_edge(src_path, dst_path.string(), statement, line_no);
      return;
    }
  }

  // if non of the cases above found a file:
  // -> add an dummy entry
  insert_edge(src_path, "", statement, line_no);
}

void Solver_C::insert_edge(const std::string &src_path,
                           const std::string &dst_path, const std::string &name,
                           unsigned int line_no) {
  add_vertex(name, dst_path);
  string key;

  Edge_Descriptor edge;
  bool b;

  if (0 == dst_path.length()) {
    BOOST_LOG_TRIVIAL(trace) << "insert_edge: "
                             << "\n"
                             << "   src = " << src_path << "\n"
                             << "   dst = " << name << "\n"
                             << "   name = " << name;
    boost::tie(edge, b) = boost::add_edge_by_label(src_path, name, graph);
  } else {
    BOOST_LOG_TRIVIAL(trace) << "insert_edge: "
                             << "\n"
                             << "   src = " << src_path << "\n"
                             << "   dst = " << dst_path << "\n"
                             << "   name = " << name;
    boost::tie(edge, b) = boost::add_edge_by_label(src_path, dst_path, graph);
  }

  graph[edge] = Edge{static_cast<int>(line_no)};
}

}  // namespace INCLUDE_GARDENER

// vim: filetype=cpp et ts=2 sw=2 sts=2
