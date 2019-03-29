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
#include "statement_py.h"

#include <memory>
#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>
#include <boost/filesystem.hpp>

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


  /// @brief Convenience function for adding a vector of statements
  /// as edges through add_edge.
  void add_edges(std::vector<Statement_Py> &statements);

  virtual bool is_module(const std::string &path_string);
  virtual bool is_package(const std::string &path_string);

 private:
  /// @brief The process paths given as a program argument.
  std::vector<std::string> process_path;

  /// @brief Legal file extensions for Python script files.
  const std::vector<std::string> file_extensions{"py", "pyw", "py3"};

  // Enum naming the different import types (corresponds to regex index)
  enum Py_Regex { IMPORT = 0, FROM_IMPORT, ALL_IMPORT, HAS_DONE_FIRST_PASS = 99 };

};  // class Solver_Py

}  // namespace INCLUDE_GARDENER

#endif // SOLVER_PY_H

// vim: filetype=cpp et ts=2 sw=2 sts=2
