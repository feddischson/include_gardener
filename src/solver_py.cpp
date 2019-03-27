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
                             R"(^[ \t]*from[ \t]+([^\d\W](?:[\w\.]*)[ \t]+import[ \t]+(?:\*|[^\d\W](?:[\w,\. ]*)))[ \t]*$)",
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

  string import_line_to_path = "";
  bool contains_star = false;

  if (idx == FROM_IMPORT && boost::contains(statement, "*")){
    contains_star = true;
    import_line_to_path = get_first_substring(statement, " ");
    idx = IMPORT;
  } else {
    import_line_to_path = statement;
  }

  if (idx == ALL_IMPORT){
    import_line_to_path = boost::erase_all_copy(import_line_to_path, "\"");
    import_line_to_path = boost::erase_all_copy(import_line_to_path, "\'");

    if (!boost::contains(import_line_to_path, ",")){
      import_line_to_path.insert(0, "."); // __all__ imports are relative
    }
  }

  // Handle comma separated parts separately by calling add_edge() again
  if (idx != HAS_DONE_FIRST_PASS && boost::contains(import_line_to_path, ",")){
    handle_comma_separated_import(src_path, import_line_to_path, idx, line_no);
    return;
  }

  path parent_directory;

  if (is_relative_import(import_line_to_path)){
    modify_path_and_statement_to_relative_import(import_line_to_path, src_path, parent_directory);
  } else {
    parent_directory = path(process_path[0]); // Absolute import
  }

  import_line_to_path = convert_import_statement_to_path_str(import_line_to_path);
  path likely_path = parent_directory / path(import_line_to_path);
  string likely_module_name = likely_path.stem().string();
  path likely_module_parent_path = likely_path.parent_path();

  unique_lock<mutex> glck(graph_mutex);

  for (const string &file_extension : file_extensions){
    string module_with_file_extension = likely_module_name + '.' + file_extension;
    path dst_path = likely_module_parent_path / module_with_file_extension;

    if (exists(dst_path)){
      dst_path = canonical(dst_path);
      insert_edge(src_path, dst_path.string(), import_line_to_path, line_no);
      return;
    } else if (contains_star && is_package((likely_module_parent_path/likely_module_name).string())){
      import_line_to_path += "/__init__.py";
      insert_edge(src_path, canonical((likely_module_parent_path / likely_module_name / "__init__.py")).string(), import_line_to_path, line_no);
      return;
    } else if (is_package((likely_module_parent_path/likely_module_name).string())) {
      return; // Not a valid import!
    }
  }

  // if none of the cases above found a file:
  // -> add a dummy entry
  string dummy_name = extract_dummy_node_name(statement);
  insert_edge(src_path, "", dummy_name, line_no);
}

vector<string> Solver_Py::split_comma_string(const std::string &statement) {
    vector<string> separate_statements;
    boost::split(separate_statements, statement, [](char c){ return c == ','; });
    return separate_statements;
}

void Solver_Py::add_edges(const std::vector<std::string> &statements, string src_path,
  unsigned int idx, unsigned int line_no)
{
    for (const string &statement : statements){
        add_edge(src_path, statement, idx, line_no);
    }
}

bool Solver_Py::is_relative_import(const std::string &statement)
{
    return begins_with_dot(statement);
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

void Solver_Py::handle_comma_separated_import(const std::string &src_path, const std::string &statement, unsigned int idx, unsigned int line_no)
{
  vector<string> comma_separated_statements;

  if (idx == FROM_IMPORT){
    string before_import = get_first_substring(statement, " ");
    string after_import = get_final_substring(statement, " ");
    comma_separated_statements = split_comma_string(after_import);

    // Append package name to item being imported
    for (auto &comma_separated_statement : comma_separated_statements){
      comma_separated_statement.insert(0, before_import + ".");
    }

    add_edges(comma_separated_statements, src_path, HAS_DONE_FIRST_PASS, line_no);

  } else if (idx == IMPORT){
    comma_separated_statements = split_comma_string(statement);
  } else if (idx == ALL_IMPORT){
    comma_separated_statements = split_comma_string(statement);

    // __all__ imports are always relative to current directory
    for (auto &comma_separated_statement : comma_separated_statements){
      comma_separated_statement.insert(0, ".");
    }
  }

  add_edges(comma_separated_statements, src_path, HAS_DONE_FIRST_PASS, line_no);

  return;
}

void Solver_Py::modify_path_and_statement_to_relative_import(
    string &statement, const string &src_path,
    boost::filesystem::path &parent_directory){

  parent_directory = path(src_path).parent_path();
  unsigned int directories_above = how_many_directories_above(statement);
  for (unsigned int i = 0; i < directories_above; i++){
    parent_directory = parent_directory.parent_path();
  }

  statement = without_prepended_dots(statement);
}

string Solver_Py::extract_dummy_node_name(const string &statement)
{
  string dummy_name;
  if (boost::contains(statement, " ")){
    dummy_name = get_first_substring(statement, " ");
    if (dummy_name.empty()){
      dummy_name = boost::erase_all_copy(statement, " ");
    }
  } else {
    dummy_name = statement;
  }

  return dummy_name;
}

string Solver_Py::dots_to_system_slash(const string &statement){
  const string separator(1, path::preferred_separator);
  return boost::replace_all_copy(statement, ".", separator);
}

string Solver_Py::convert_import_statement_to_path_str(const string &statement){
  string as_statements_removed = remove_as_statements(statement);

  string from_field;
  string import_field;
  path path_concatenation;
  auto dirs_above = 0;

  if (begins_with_dot(as_statements_removed)){
    dirs_above = how_many_directories_above(as_statements_removed);
    as_statements_removed =  without_prepended_dots(as_statements_removed);
  }
  if (boost::contains(as_statements_removed, " ")){
    from_field = get_first_substring(as_statements_removed, " "); // Before import
    import_field = get_final_substring(as_statements_removed, " "); // After import
    boost::erase_all(from_field, " ");
    boost::erase_all(import_field, " ");

    path_concatenation = path{dots_to_system_slash(from_field)}
                        / path{dots_to_system_slash(import_field)};
  } else {
    path_concatenation = dots_to_system_slash(as_statements_removed);
  }

  if (dirs_above > 0){
    for (auto i = 0; i < dirs_above; i++){
      path_concatenation = path{".."} / path_concatenation;
    }
  }

  return boost::erase_all_copy(path_concatenation.string(), " ");
}

unsigned int Solver_Py::how_many_directories_above(const string &statement){
  boost::regex r(dot_regex);
  boost::match_results<string::const_iterator> results;
  if (boost::regex_match(statement, results, r)){
    return results[1].length() - 1;  // The first dot is current directory
  }

  return 0;
}

bool Solver_Py::begins_with_dot(const string &statement)
{
  boost::regex r(dot_regex);
  return boost::regex_match(statement, r);
}

string Solver_Py::without_prepended_dots(const string &statement)
{
  boost::regex r(past_dot_regex);
  boost::match_results<string::const_iterator> results;
  if (boost::regex_match(statement, results, r)){
    if (results.size() > 1){
      return results[1].str();
    }
  }

  return statement;
}

string Solver_Py::remove_as_statements(const string &statement)
{
  string string_copy = statement;
  string::size_type as_pos;
  string::size_type comma_pos;

  do {
    as_pos = string_copy.find(" as ");
    if (as_pos != string::npos){
      comma_pos = string_copy.find(",", as_pos);
      if (comma_pos != string::npos){
        string_copy.erase(as_pos, comma_pos);
      } else {
        string_copy.erase(as_pos, string::npos);
      }
    } else {
      break;
    }
  } while (as_pos != string::npos);

  return string_copy;
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

string Solver_Py::get_first_substring(const string &statement, const string &delimiter)
{
  string::size_type pos_of_first_delim = statement.find(delimiter);
  if (pos_of_first_delim != string::npos){
    string str_copy = statement.substr(0, pos_of_first_delim);
    return str_copy;
  }
  return "";
}

string Solver_Py::get_final_substring(const string &statement,
                                           const string &delimiter){
  string::size_type pos_of_last_delim = statement.find_last_of(delimiter);
  if (pos_of_last_delim != string::npos){
    string str_copy = statement.substr(pos_of_last_delim + 1);
    return str_copy;
  }
  return "";
}
}  // namespace INCLUDE_GARDENER

// vim: filetype=cpp et ts=2 sw=2 sts=2
