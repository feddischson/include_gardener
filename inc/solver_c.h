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
#ifndef SOLVER_C_H
#define SOLVER_C_H

#include <memory>

#include "solver.h"

namespace INCLUDE_GARDENER {

/// @brief Solver class for C/C++/Obj-C language.
/// @details
///   Instances of this class behave simlar to the C-pre-processor.
///   When an include statement is detected:
///   Depending if it is a #include "" or #include <> statement,
///   the file is searched relative to the source file first, or
///   in the include paths, provided by the option c-include-path list.
class Solver_C : public Solver {
 public:
  /// @brief Smart pointer for Solver_C
  using Ptr = std::shared_ptr<Solver_C>;

  /// @brief Default ctor.
  Solver_C() = default;

  /// @brief Copy ctor: not implemented!
  Solver_C(const Solver_C &other) = delete;

  /// @brief Assignment operator: not implemented!
  Solver_C &operator=(const Solver_C &rhs) = delete;

  /// @brief Move constructor: not implemented!
  Solver_C(Solver_C &&rhs) = delete;

  /// @brief Move assignment operator: not implemented!
  Solver_C &operator=(Solver_C &&rhs) = delete;

  /// @brief Default dtor
  ~Solver_C() override = default;

  /// @brief Adds an edge to the graph.
  /// @param src_path Path the the source path (where the statement is detected.
  /// @param statement The detected statement
  /// @param idx The index of the regular expression, which matched.
  /// @param line_no The line number where the statement is detected.
  void add_edge(const std::string &src_path, const std::string &statement,
                unsigned int idx, unsigned int line_no) override;

  /// @brief Returns the regex which
  ///        detectes the statements.
  std::vector<std::string> get_statement_regex() const override;

  /// @brief Returns the regex which
  ///        detectes the files.
  std::string get_file_regex() const override;

  /// @brief Extracts solver-specific options (variables).
  void extract_options(
      const boost::program_options::variables_map &vm) override;

  /// @brief Adds solver-specific options.
  static void add_options(boost::program_options::options_description *options);

 protected:
  /// @brief Adds an edge
  /// @param src_path Path the the source path (where the statement is detected.
  /// @param dst_path Path of the destination file (the file which is included).
  /// @param name The statement (mostly the name of the file).
  /// @param line_no The line number where the statement is detected.
  virtual void insert_edge(const std::string &src_path,
                           const std::string &dst_path, const std::string &name,
                           unsigned int line_no);

 private:
  /// @brief Search path for include statements.
  std::vector<std::string> include_paths;
};  // class Solver_C

}  // namespace INCLUDE_GARDENER

#endif  // SOLVER_C_H

// vim: filetype=cpp et ts=2 sw=2 sts=2
