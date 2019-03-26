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
#ifndef SOLVER_PY_H
#define SOLVER_PY_H

#include "solver.h"

#include <memory>
#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>

namespace INCLUDE_GARDENER {

/// @brief Solver class for Python language.
/// @details
///   An instance of this class attempts to convert
///   Python import strings to paths that are inserted
///   in the internal Graph with import-relations
///   between the files.
///
///   The class should support relative imports that
///   are prepended with dots (ex. import ..helloworld).
///   It should also support imports on the following forms:
///
///   1. import x
///   2. from x import y
///
///   Both work even if as-statements are included,
///   but these are removed when it is converted
///   to a path.
///
///   1. x can contain comma-separated items.
///      x can contain prepended dots.
///   2. y can contain comma-separated items.
///      x can contain prepended dots.
///      y can contain *, but results are not
///      guaranteed.
///
/// @note Imports with * are not yet fully supported.
/// @note Options have not been implemented.

class Solver_Py : public Solver {

 public:
  /// @brief Smart pointer for Solver_Py
  using Ptr = std::shared_ptr<Solver_Py>;

  /// @brief Default ctor.
  Solver_Py() = default;

  /// @brief Copy ctor: not implemented!
  Solver_Py(const Solver_Py &other) = delete;

  /// @brief Assignment operator: not implemented!
  Solver_Py &operator=(const Solver_Py &rhs) = delete;

  /// @brief Move constructor: not implemented!
  Solver_Py(Solver_Py &&rhs) = delete;

  /// @brief Move assignment operator: not implemented!
  Solver_Py &operator=(Solver_Py &&rhs) = delete;

  /// @brief Default dtor
  ~Solver_Py() override = default;

  /// @brief Adds an edge to the graph.
  /// @param src_path Path the the source path (where the statement is detected).
  /// @param statement The detected statement
  /// @param idx The index of the regular expression, which matched.
  /// @param line_no The line number where the statement is detected.
  void add_edge(const std::string &src_path, const std::string &statement,
                unsigned int idx, unsigned int line_no) override;

  /// @brief Returns the regex which detects the import statements.
  std::vector<std::string> get_statement_regex() const override;

  /// @brief Returns the regex which detects the files.
  std::string get_file_regex() const override;

  /// @brief Extracts solver-specific options (variables).
  /// @note Not implemented yet.
  void extract_options(
      const boost::program_options::variables_map &vm) override;

  /// @brief Adds solver-specific options.
  /// @note Not implemented yet.
  static void add_options(
      boost::program_options::options_description *options);

 protected:
  /// @brief Adds an edge
  /// @param src_path Path the the source path (where the statement is detected).
  /// @param dst_path Path of the destination file (the file which is included).
  /// @param name The statement (mostly the name of the file).
  /// @param line_no The line number where the statement is detected.
  virtual void insert_edge(const std::string &src_path,
                           const std::string &dst_path, const std::string &name,
                           unsigned int line_no);

  /// @brief Returns the final substring separated by a delimiter.
  /// @param statement The statemestatementnt to extract substring from.
  /// @param delimiter The final delimiter char from which to begin splitting.
  /// @return The substring between the final delimiter and end of string.
  /// On failure an empty string is returned.
  /// @pre statement contains delimiter.
  /// @pre Delimiter is one character.
  virtual std::string get_final_substring(const std::string &statement,
                                  const std::string &delimiter);

  /// @brief Gets the first substring before the first occurrence
  /// of a delimiter.
  /// @param statement The string to substring.
  /// @param delimiter The delimiter to split string by.
  /// @return String with everything before delimiter in statement.
  /// On failure an empty string is returned.
  /// @pre statement is not an empty string.
  /// @pre statement contains delimiter.
  /// @pre Delimiter is one character.
  virtual std::string get_first_substring(const std::string &statement,
                                  const std::string &delimiter);

  /// @brief Converts dots in a string to system slashes.
  /// @param statement The statement to copy and modify.
  /// @return String with dots converted to system slashes.
  /// @note The slashes that should be used are gotten from
  /// boost::filesystem::path::preferred_separator.
  virtual std::string dots_to_system_slash(const std::string &statement);

  /// @brief Converts a Python "(from) x import y" statement to
  /// a system path string.
  /// @param statement The statement to convert.
  /// @return String with converted statement as system path.
  /// @pre statement contains the string " import "
  virtual std::string from_import_statement_to_path(const std::string &statement);

  /// @brief Converts a Python "import (x)" statement to a
  /// system path string.
  /// @param statement The statement to convert.
  /// @return String with converted statement as system path.
  /// @pre statement does not contain the statement " import "
  virtual std::string import_statement_to_path(const std::string &statement);

  /// @brief Counts how many dots a string is prepended with.
  /// @param statement The statement to test.
  /// @return How many dots the string was prepended with minus one.
  /// @note Uses regex in variable dot_regex.
  /// @note The first relative dot in Python is the current directory,
  /// and such this function returns zero for one dot.
  virtual unsigned int how_many_directories_above(const std::string &statement);

  /// @brief Checks if a string begins with a dot.
  /// @param statement The statement to test.
  /// @return If the statement begins with a dot not
  /// worrying about whitespace or tab.
  /// @note Regex used is in variable dot_regex.
  virtual bool begins_with_dot(const std::string &statement);

  /// @brief Removes prepended dots from string.
  /// @param statement to remove dots from.
  /// @return The modified string.
  /// @pre That the string begins with dot(s), not worrying
  /// about tabs or whitespaces.
  /// @note Regex used is in variable past_dot_regex.
  virtual std::string without_prepended_dots(const std::string &statement);

  /// @brief Removes " as x" statements from Python import string.
  /// @param statement to remove " as "-statements from.
  /// @return The modified string.
  virtual std::string remove_as_statements(const std::string &statement);

  /// @brief Splits a comma separated string into separate parts
  /// @param comma separated string
  /// @return a vector with each substring represented
  virtual std::vector<std::string> split_comma_string(const std::string &statement);


  virtual bool is_relative_import(const std::string &statement);
  virtual bool is_module(const std::string &path_string);
  virtual bool is_package(const std::string &path_string);

 private:
  /// @brief The process paths given as a program argument.
  std::vector<std::string> process_path;

  /// @brief Legal file extensions for Python script files.
  const std::vector<std::string> file_extensions{"py", "pyw", "py3"};

  /// @brief Regex to match the first dot(s) in a string
  const std::string dot_regex = "^[ \\t]*([.]+).*$";

  /// @brief Regex to match everything past the first dot(s) in a string
  const std::string past_dot_regex = "^[ \\t]*[.]+(.*)$";

  // Enum naming the different import types (corresponds to regex index)
  enum Py_Regex { IMPORT = 0, FROM_IMPORT, HAS_DONE_FIRST_PASS = 99 };

};  // class Solver_Py

}  // namespace INCLUDE_GARDENER

#endif // SOLVER_PY_H

// vim: filetype=cpp et ts=2 sw=2 sts=2
