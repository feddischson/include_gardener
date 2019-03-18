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

const int py_import_regex_idx = 0;
const int py_from_import_regex_idx = 1;
const int has_done_first_pass = 99;

vector<string> Solver_Py::get_statement_regex() const {
  vector<string> regex_str = {R"(^[ \t]*import[ \t]+((?:[.]*)[^\d\W](?:[\w,\. ])*)[ \t]*$)",
                             R"(^[ \t]*from[ \t]+([^\d\W](?:[\w\.]*)[ \t]+import[ \t]+(?:\*|[^\d\W](?:[\w,\. ]*)))[ \t]*$)"};
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

<<<<<<< HEAD
  if (boost::contains(statement, "*")) return; // Import * is not supported yet
  // from (x import y)
  if (idx == FROM_IMPORT){

    if (boost::contains(statement, ",")){
      std::string before_import = get_first_substring(statement, " ");
      std::string after_import = get_final_substring(statement, " ");
      vector<string> comma_separated_statements;
=======
  std::string import_line_to_path = "";

  if (boost::contains(statement, "*")){ // Note: Imports with * are not yet supported
    import_line_to_path = get_first_substring(statement, " ");
    idx = py_import_regex_idx;
  } else {
    import_line_to_path = statement;
  }

  if (idx != has_done_first_pass){
    // from (x import y), handle comma separate items separately
    if (idx == 1 && boost::contains(import_line_to_path, ",")){

      std::string before_import = get_first_substring(import_line_to_path, " ");
      std::string after_import = get_final_substring(import_line_to_path, " ");
>>>>>>> remove-add-edges

      vector<string> comma_separated_statements;
      boost::split(comma_separated_statements, after_import, [](char c){ return c == ','; });

      // Handle each comma-separated part separately as a package name
      for (auto comma_separated_statement : comma_separated_statements){
        add_edge(src_path, before_import + "." + comma_separated_statement,
                 has_done_first_pass, line_no);
      }

      return;

<<<<<<< HEAD
  // import (x)
  } else if (idx == IMPORT){
    if (boost::contains(statement, ",")){
=======
    // import (x), handle comma separated items separately
    } else if (idx == py_import_regex_idx && boost::contains(import_line_to_path, ",")){
>>>>>>> remove-add-edges
      vector<string> comma_separated_statements;
      boost::split(comma_separated_statements, statement, [](char c){ return c == ','; });

      for (auto comma_separated_statement: comma_separated_statements){
        add_edge(src_path, comma_separated_statement,
               has_done_first_pass, line_no);
      }

      return;
    }
  }

  import_line_to_path = remove_as_statements(import_line_to_path);
  path parent_directory = path(src_path).parent_path();

  // Relative import
  if (begins_with_dot(import_line_to_path)){

    // Go up directories in src-path
    unsigned int directories_above = how_many_directories_above(import_line_to_path);
    for (unsigned int i = 0; i < directories_above; i++){
      parent_directory = parent_directory.parent_path();
    }

    // Remove prepended dots
    import_line_to_path = without_prepended_dots(import_line_to_path);
  }

  // Check if the package import is referring to the current directory and go up if so
  if (parent_directory.stem().string() == get_first_substring(import_line_to_path, ".")){
    parent_directory = parent_directory.parent_path();
  }

  if (boost::contains(import_line_to_path, " import ")){
    import_line_to_path = from_import_statement_to_path(import_line_to_path);
  } else {
    import_line_to_path = import_statement_to_path(import_line_to_path);
  }

  path likely_path = parent_directory / path(import_line_to_path);
  std::string likely_module_name = likely_path.stem().string();
  path likely_module_parent_path = likely_path.parent_path();

  unique_lock<mutex> glck(graph_mutex);

  for (const string &file_extension : file_extensions){
    std::string module_with_file_extension = likely_module_name + '.' + file_extension;
    path dst_path = likely_module_parent_path / module_with_file_extension;

    if (exists(dst_path)){
      dst_path = canonical(dst_path);
      BOOST_LOG_TRIVIAL(trace) << "   |>> Relative Edge";
      insert_edge(src_path, dst_path.string(), import_line_to_path, line_no);
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
  insert_edge(src_path, "", get_first_substring(statement, " "), line_no); //module_name

}

std::string Solver_Py::dots_to_system_slash(const std::string &statement)
{
  std::string separator(1, boost::filesystem::path::preferred_separator);
  return boost::replace_all_copy(statement, ".", separator);
}

std::string Solver_Py::from_import_statement_to_path(const std::string &statement)
{
  std::string from_field = get_first_substring(statement, " ");
  std::string import_field = get_final_substring(statement, " ");
  boost::erase_all(from_field, " ");

  std::string path_concatenation = dots_to_system_slash(
    from_field
    + boost::filesystem::path::preferred_separator
    + import_field);

  boost::erase_all(path_concatenation, " ");
  return path_concatenation;
}

std::string Solver_Py::import_statement_to_path(const std::string &statement){
  std::string import_field = statement;
  std::string path_concatenation = dots_to_system_slash(
              boost::filesystem::path::preferred_separator
                                + import_field);
  boost::erase_all(path_concatenation, " ");
  return path_concatenation;
}

unsigned int Solver_Py::how_many_directories_above(const std::string &statement){
  boost::regex r(dot_regex);
  boost::match_results<std::string::const_iterator> results;
  if (boost::regex_match(statement, results, r)){
    return results[1].length() - 1;  // The first dot is current directory
  }

  return 0;
}

bool Solver_Py::begins_with_dot(const std::string &statement)
{
  boost::regex r(dot_regex);
  return boost::regex_match(statement, r);
}

std::string Solver_Py::without_prepended_dots(const std::string &statement)
{
  boost::regex r(past_dot_regex);
  boost::match_results<std::string::const_iterator> results;
  if (boost::regex_match(statement, results, r)){
    if (results.size() > 1){
      return results[1].str();
    }
  }

  return statement;
}

std::string Solver_Py::remove_as_statements(const std::string &statement)
{
  std::string string_copy = statement;
  std::string::size_type as_pos;
  std::string::size_type comma_pos;

  do {
    as_pos = string_copy.find(" as ");
    if (as_pos != std::string::npos){
      comma_pos = string_copy.find(",", as_pos);
      if (comma_pos != std::string::npos){
        string_copy.erase(as_pos, comma_pos);
      } else {
        string_copy.erase(as_pos, std::string::npos);
      }
    } else {
      break;
    }
  } while (as_pos != std::string::npos);

  return string_copy;
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

std::string Solver_Py::get_first_substring(const std::string &statement, const std::string &delimiter)
{
  std::string::size_type pos_of_first_delim = statement.find(delimiter);
  if (pos_of_first_delim != std::string::npos){
    std::string str_copy = statement.substr(0, pos_of_first_delim);
    return str_copy;
  }
  return statement;
}

std::string Solver_Py::get_final_substring(const std::string &statement,
                                           const std::string &delimiter){
  std::string::size_type pos_of_last_delim = statement.find_last_of(delimiter);
  return statement.substr(pos_of_last_delim+1);
}
}  // namespace INCLUDE_GARDENER

// vim: filetype=cpp et ts=2 sw=2 sts=2
