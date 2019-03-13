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

vector<string> Solver_Py::get_statement_regex() const {
  vector<string> regex_str = {R"(^[ \t]*import[ \t]+([^\d\W](?:[\w,\.])*)[ \t]*$)",
                             R"(^[ \t]*from[ \t]+([^\d\W](?:[\w\.]*))[ \t]+import[ \t]+([^\d\W](?:[\w,\. ]*)|\*)[ \t]*$)"};
  return regex_str;
}

string Solver_Py::get_file_regex() const { return string(R"(^(?:.*[\/\\])?[^\d\W]\w*\.py[3w]?$)"); }

void Solver_Py::add_options(po::options_description *options __attribute__((unused))) {

}

void Solver_Py::extract_options(const po::variables_map &vm __attribute__((unused))) {

}

void Solver_Py::add_edge(const string &src_path,
                         const string &statement,
                         unsigned int idx,
                         unsigned int line_no) {
  using boost::filesystem::path;
  using boost::filesystem::operator/;

  BOOST_LOG_TRIVIAL(trace) << "add_edge: " << src_path << " -> " << statement
                             << ", idx = " << idx << ", line_no = " << line_no;

  if (statement == "*") return;
  if (statement.empty()) return;

  // Handle each comma-separated part separately
  if (boost::contains(statement, ",")) {
    vector<string> comma_separated_statements;
    boost::split(comma_separated_statements, statement, [](char c){ return c == ','; });
    add_edges(src_path, comma_separated_statements, idx, line_no);
    return;
  }

  string module_name = statement;

  if (boost::contains(statement, ".")) {
    if (is_likely_local_package(get_first_substring(statement, "."))){
      module_name = get_final_substring(statement, ".");
    }
  }

  if (boost::contains(statement, " as ")){

    module_name = get_first_substring(statement, " as ");
  }

  // Remove whitespace
  boost::remove_erase_if(module_name, ::isspace);

  unique_lock<mutex> glck(graph_mutex);

  path base = path(src_path).parent_path();

  for (const string &file_extension : file_extensions){
    string module_with_file_extension = module_name + '.' + file_extension;
    path dst_path = base / module_with_file_extension;
    if (exists(dst_path)) {
      dst_path = canonical(dst_path);
      BOOST_LOG_TRIVIAL(trace) << "   |>> Relative Edge";
      insert_edge(src_path, dst_path.string(), module_name, line_no);
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

  // if none of the cases above found a file:
  // -> add an dummy entry
  insert_edge(src_path, "", module_name, line_no);
}

void Solver_Py::add_edges(const std::string &src_path,
                          const std::vector<std::string> &statements,
                          unsigned int idx,
                          unsigned int line_no){  
  for (auto statement: statements){
    add_edge(src_path, statement, idx, line_no);
  }
}

void Solver_Py::insert_edge(const std::string &src_path,
                            const std::string &dst_path,
                            const std::string &name,
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

std::string Solver_Py::get_final_substring(const std::string &statement,
                                           const std::string &delimiter){
  size_t pos_of_last_delim = statement.find_last_of(delimiter);
  return statement.substr(pos_of_last_delim+1);
}

std::string Solver_Py::get_first_substring(const std::string &statement, const std::string &delimiter)
{
  return statement.substr(0, statement.find_first_of(delimiter));
}

bool Solver_Py::is_likely_local_package(const std::string &statement)
{
  unique_lock<mutex> glck(graph_mutex);

  for (Vertex::Map::iterator it = vertexes.begin(); it != vertexes.end(); ++it){
    if (boost::contains(it->second->get_abs_path(), statement)){
      return true;
    }
  }

  return false;
}


}  // namespace INCLUDE_GARDENER

// vim: filetype=cpp et ts=2 sw=2 sts=2
