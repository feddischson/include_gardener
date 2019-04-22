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
#include "solver_py.h"

#include <string>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/log/trivial.hpp>
#include <boost/range/algorithm_ext/erase.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/find.hpp>

namespace INCLUDE_GARDENER {

namespace po = boost::program_options;
using std::mutex;
using std::string;
using std::unique_lock;
using std::vector;
using boost::filesystem::path;

vector<string> Solver_Py::get_statement_regex() const {
  vector<string> regex_str = {R"(^[ \t]*import[ \t]+((?:[.]*)[^\d\W](?:[\w,\. ])*)[ \t]*$)",
                             R"(^[ \t]*from[ \t]+(\.*[^\d\W](?:[\w\.]*)[ \t]+import[ \t]+(?:\*|[^\d\W](?:[\w,\. ]*)))[ \t]*$)",
                             R"(^[ \t]*__all__[ \t]*=[ \t]*\[(.*)\]$)"};
  return regex_str;
}

string Solver_Py::get_file_regex() const { return string(R"(^(?:.*[\/\\])?[^\d\W]\w*\.py[3w]?$)"); }

void Solver_Py::add_options(po::options_description *options __attribute__((unused))) {
  BOOST_LOG_TRIVIAL(trace) << "add_options in Solver_Py has not been implemented";
}

void Solver_Py::extract_options(const po::variables_map &vm) {
  if (vm.count("process-path") != 0u) {
    process_path = vm["process-path"].as<vector<string>>();
  }
}

void Solver_Py::add_edge(const string &src_path,
                         const string &statement,
                         unsigned int idx,
                         unsigned int line_no) {
  using boost::filesystem::operator/;

  BOOST_LOG_TRIVIAL(trace) << "add_edge: " << src_path << " -> " << statement
                             << ", idx = " << idx << ", line_no = " << line_no;

  Statement_Py py_statement(src_path, statement, idx, line_no);

  if (py_statement.contained_multiple_imports()){
    vector<Statement_Py> child_statements = py_statement.get_child_statements();
    add_edges(child_statements);
    return;
  }

  path parent_directory;

  if (py_statement.get_is_relative()){
    unsigned int directories_above = py_statement.get_directories_above();
    parent_directory = path(src_path).parent_path();

    for (unsigned int i = 0; i < directories_above; i++){
      parent_directory = parent_directory.parent_path();
    }
  } else {
    parent_directory = path(process_path[0]); // Absolute import
  }

  string possible_path = py_statement.get_possible_path();
  path likely_path = parent_directory / path(possible_path);
  string likely_module_name = likely_path.stem().string();
  path likely_module_parent_path = likely_path.parent_path();

  unique_lock<mutex> glck(graph_mutex);

  for (const string &file_extension : file_extensions){
    string module_with_file_extension = likely_module_name + '.' + file_extension;
    path dst_path = likely_module_parent_path / module_with_file_extension;

    if (exists(dst_path)){
      dst_path = canonical(dst_path);
      insert_edge(src_path, dst_path.string(), possible_path, line_no);
      return;
    } else if (py_statement.get_had_star() && is_package((likely_module_parent_path/likely_module_name).string())){
      possible_path += "/__init__.py";
      insert_edge(src_path, canonical((likely_module_parent_path / likely_module_name / "__init__.py")).string(), possible_path, line_no);
      return;
    } else if (is_package((likely_module_parent_path/likely_module_name).string())) {
      return; // Not a valid import!
    }
  }

  // if none of the cases above found a file:
  // -> add a dummy entry
  string dummy_name = py_statement.extract_dummy_node_name(statement);
  insert_edge(src_path, "", dummy_name, line_no);
}

void Solver_Py::insert_edge(const string &src_path,
                            const string &dst_path,
                            const string &name,
                            unsigned int line_no) {
  add_vertex(name, dst_path);
  string key;

  Edge_Descriptor edge;
  bool b;

  // Does the same edge already exist?
  if (boost::edge_by_label(src_path, name, graph).second
          || boost::edge_by_label(src_path, dst_path, graph).second){
    BOOST_LOG_TRIVIAL(trace) << "Duplicate in insert_edge: "
                               << "\n"
                               << "   src = " << src_path << "\n"
                               << "   dst = " << name << "\n"
                               << "   name = " << name;
    return;
  }

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

void Solver_Py::add_edges(vector<Statement_Py> &statements)
{
  for (Statement_Py &statement : statements){
    add_edge(statement.get_source_path(),
             statement.get_modified_statement(),
             statement.get_regex_idx(),
             statement.get_line_number());
  }
}

bool Solver_Py::is_module(const std::string &path_string)
{
  using boost::filesystem::path;
  using boost::filesystem::operator/;

  path path_to_test = path(path_string);
  string likely_module_name = path_to_test.stem().string();
  path likely_module_parent_path = path_to_test.parent_path();

  for (const string &file_extension : file_extensions){
    string module_with_file_extension = likely_module_name + '.' + file_extension;
    path dst_path = likely_module_parent_path / module_with_file_extension;

    if (exists(dst_path)){
      return true;
    }
  }

  return false;
}

bool Solver_Py::is_package(const std::string &path_string)
{
    using boost::filesystem::path;
    using boost::filesystem::operator/;

    path as_path = path(path_string);
    path init_file = path("__init__.py");
    return is_directory(as_path) && exists(as_path / init_file);
}


}  // namespace INCLUDE_GARDENER

// vim: filetype=cpp et ts=2 sw=2 sts=2
