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

namespace INCLUDE_GARDENER {

/// @brief Solver class for Python language.
/// @details
///   To be implemented.

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

  /// @brief Returns the regex which
  ///        detects the statements.
  std::vector<std::string> get_statement_regex() const override;

  /// @brief Returns the regex which
  ///        detects the files.
  std::string get_file_regex() const override;

  /// @brief Extracts solver-specific options (variables).
  void extract_options(
      const boost::program_options::variables_map &vm) override;

  /// @brief Adds solver-specific options.
  static void add_options(boost::program_options::options_description *options);

 protected:
  /// @brief Adds an edge
  /// @param src_path Path the the source path (where the statement is detected).
  /// @param dst_path Path of the destination file (the file which is included).
  /// @param name The statement (mostly the name of the file).
  /// @param line_no The line number where the statement is detected.
  virtual void insert_edge(const std::string &src_path,
                           const std::string &dst_path, const std::string &name,
                           unsigned int line_no);

 private:
  /// @brief Search path for include statements.
  std::vector<std::string> include_paths;

  /// @brief Legal file extensions for Python script files.
  std::vector<std::string> file_extensions{"py", "pyw", "py3"};

  /// @brief Returns the final substring separated by a delimiter.
  /// @param statement The statement to extract substring from.
  /// @param delimiter The final delimiter from which to splitting.
  /// @return The substring between the final delimiter and end of string.
  /// @pre String contains delimiter.
  std::string get_final_substring(const std::string &statement,
                                  const std::string &delimiter);

  /// @brief Gets the first substring before the first occurrence
  /// of a delimiter.
  /// @param statement The string to substring.
  /// @param delimiter The delimiter to split string by.
  /// @return String with everything before delimiter in statement.
  /// @pre statement is not an empty string.
  std::string get_first_substring(const std::string &statement,
                                  const std::string &delimiter);

  /// @brief Adds multiple edges to the graph.
  /// @param src_path The source path (where the statement is detected).
  /// @param statements The detected statements.
  /// @param idx The index of the regular expression, which matched.
  /// @param line_no The line number where the statement is detected.
  void add_edges(const std::string &src_path,
                 const std::vector<std::string> &statements,
                 unsigned int idx, unsigned int line_no);

  /// @brief Converts dots in a string to slashes used in paths
  /// in the current system and returns a copy.
  /// @param statement The statement to have this occur in.
  /// @return Copy of statement where the modification has been done.
  std::string dots_to_system_slash(const std::string &statement);

  std::string from_import_statement_to_path(const std::string &statement);

  std::string import_statement_to_path(const std::string &statement);
};  // class Solver_Py

}  // namespace INCLUDE_GARDENER

#endif // SOLVER_PY_H

// vim: filetype=cpp et ts=2 sw=2 sts=2
