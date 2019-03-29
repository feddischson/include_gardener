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

#include "statement_py.h"

#include <string>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/log/trivial.hpp>
#include <boost/range/algorithm_ext/erase.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/find.hpp>
#include <boost/regex.hpp>

namespace INCLUDE_GARDENER {
  using std::string;
  using std::vector;
  using boost::filesystem::path;

Statement_Py::Statement_Py(const std::string &src_path, const std::string &statement, unsigned int idx, unsigned int line_no)
  : source_path(src_path), original_statement(statement), line_number(line_no)
{
  regex_idx = idx;

  if (regex_idx == FROM_IMPORT && contains_star(original_statement)){
    had_star = true;
    modified_statement = get_all_before_import(original_statement);
    regex_idx = IMPORT;
  } else {
    had_star = false;
    modified_statement = original_statement;
  }

  if (regex_idx == ALL_IMPORT){
    erase_all_quotation_marks(modified_statement);

    if (!contains_comma(modified_statement)){
      add_relative_dot_in_front(modified_statement);
    }
  }

  if (contains_comma(modified_statement)){
    contains_multiple_imports = true;
    split_into_multiple_statements(src_path, modified_statement, regex_idx, line_no);
    return;
  }

  if (is_relative_import(modified_statement)){
    is_relative = true;
    directories_above = how_many_directories_above(modified_statement);
    remove_prepended_dots(modified_statement);
  }

  remove_as_statements(modified_statement);
  remove_whitespace(modified_statement);
  possible_path = dots_to_system_slash(modified_statement);
}

vector<string> Statement_Py::split_by_comma(const string &statement) {
  vector<string> separate_statements;
  boost::split(separate_statements, statement, [](char c){ return c == ','; });
  return separate_statements;
}

bool Statement_Py::is_relative_import(const std::string &statement)
{
  boost::regex r(dot_regex);
  return boost::regex_match(statement, r);
}



void Statement_Py::erase_all_quotation_marks(std::string &statement)
{
  boost::erase_all(statement, "\"");
  boost::erase_all(statement, "\'");
}

void Statement_Py::add_relative_dot_in_front(std::string &statement)
{
  statement.insert(0, ".");
}

void Statement_Py::add_package_name_in_front(std::string &statement, const std::string &package_name)
{
  statement.insert(0, package_name + ".");
}

std::string Statement_Py::get_all_before_import(const std::string &statement)
{
  string::size_type pos_of_first_delim = statement.find(" ");
  if (pos_of_first_delim != string::npos){
    string str_copy = statement.substr(0, pos_of_first_delim);
    return str_copy;
  }
  return "";
}

std::string Statement_Py::get_all_after_import(const std::string &statement)
{
  string::size_type pos_of_last_delim = statement.find_last_of(" ");
  if (pos_of_last_delim != string::npos){
    string str_copy = statement.substr(pos_of_last_delim + 1);
    return str_copy;
  }
  return "";
}

bool Statement_Py::contains_comma(const std::string &statement)
{
  return boost::contains(statement, ",");
}

bool Statement_Py::contains_star(const std::string &statement)
{
  return boost::contains(statement, "*");
}

void Statement_Py::split_into_multiple_statements(const std::string &src_path,
                                                  const std::string &statement,
                                                  unsigned int idx,
                                                  unsigned int line_no)
{
  vector<string> comma_separated_statements;

  if (idx == FROM_IMPORT){
    string before_import = get_all_before_import(statement);
    string after_import = get_all_after_import(statement);
    comma_separated_statements = split_by_comma(after_import);

    for (auto &comma_separated_statement : comma_separated_statements){
      add_package_name_in_front(comma_separated_statement, before_import);
      Statement_Py child(src_path, comma_separated_statement, idx, line_no);
      child_statements.push_back(child);
    }

  } else if (idx == IMPORT || idx == ALL_IMPORT){
    comma_separated_statements = split_by_comma(statement);

    if (idx == ALL_IMPORT){
      for (auto &comma_separated_statement : comma_separated_statements){
        add_relative_dot_in_front(comma_separated_statement);
      }
    }

    for (auto &comma_separated_statement : comma_separated_statements){
      Statement_Py child(src_path, comma_separated_statement, idx, line_no);
      child_statements.push_back(child);
    }
  }

  return;
}

bool Statement_Py::contained_multiple_imports()
{
  return contains_multiple_imports && !child_statements.empty();
}

std::vector<Statement_Py> Statement_Py::get_child_statements()
{
  return child_statements;
}

string Statement_Py::extract_dummy_node_name(const string &statement)
{
  string dummy_name;
  if (boost::contains(statement, " ")){
    dummy_name = get_all_before_import(statement);
    if (dummy_name.empty()){
      dummy_name = boost::erase_all_copy(statement, " ");
    }
  } else {
    dummy_name = statement;
  }

  return dummy_name;
}

string Statement_Py::dots_to_system_slash(const string &statement){
  const string separator(1, path::preferred_separator);
  return boost::replace_all_copy(statement, ".", separator);
}

unsigned int Statement_Py::how_many_directories_above(const string &statement){
  boost::regex r(dot_regex);
  boost::match_results<string::const_iterator> results;
  if (boost::regex_match(statement, results, r)){
    return results[1].length() - 1;  // The first dot is current directory
  }

  return 0;
}

void Statement_Py::remove_prepended_dots(std::string &statement)
{
  boost::regex r(past_dot_regex);
  boost::match_results<string::const_iterator> results;
  if (boost::regex_match(statement, results, r)){
    if (results.size() > 1){
      statement = results[1].str();
    }
  }
}

void Statement_Py::remove_whitespace(std::string &statement)
{
  boost::erase_all(statement, " ");
}

void Statement_Py::remove_as_statements(std::string &statement)
{
  string::size_type as_pos;
  string::size_type comma_pos;

  do {
    as_pos = statement.find(" as ");
    if (as_pos != string::npos){
      comma_pos = statement.find(",", as_pos);
      if (comma_pos != string::npos){
        statement.erase(as_pos, comma_pos);
      } else {
        statement.erase(as_pos, string::npos);
      }
    } else {
      break;
    }
  } while (as_pos != string::npos);
}

}  // namespace INCLUDE_GARDENER

// vim: filetype=cpp et ts=2 sw=2 sts=2
