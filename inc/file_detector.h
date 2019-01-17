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
#ifndef FILE_DETECTOR_H
#define FILE_DETECTOR_H

#include <string>
#include <vector>

#include <boost/regex.hpp>

#include "input_files.h"

namespace INCLUDE_GARDENER {

/// @brief Instances of this class are used to search for specific input files.
/// @details
///   A regular expression is provided to define the search pattern.
///   In addition, a list of regular expressions can be given to define,
///   which files shall be excluded.
/// @author feddischson
class File_Detector : public Input_Files {
 public:
  /// @brief Ctor: Initializes all members.
  /// @param file_regex The regex which defines the input files
  /// @param exlucde_regex The regex which defines the exluded files
  /// @param base_paths All paths in this vector are processed.
  /// @param recursive_limit Defines the recursive search limit when != 0
  File_Detector(const std::string &file_regex,
                const std::vector<std::string> &exclude_regex,
                std::vector<std::string> process_paths,
                int recursive_limit = 0);

  /// @brief Deleted copy ctor!
  File_Detector(const File_Detector &other) = delete;

  /// @brief Deleted assignment operator!
  File_Detector &operator=(const File_Detector &rhs) = delete;

  /// @brief Deleted move constructor!
  File_Detector(File_Detector &&rhs) = delete;

  /// @brief Deleted move assignment operator!
  File_Detector &operator=(File_Detector &&rhs) = delete;

  /// @brief Default dtor
  ~File_Detector() override = default;

  /// @brief Returns exlcude regex list
  std::vector<boost::regex> get_exclude_regex();

  /// @brief Returns true if the file shall be considered, otherwise false.
  bool use_file(const std::string &file) const;

  /// @brief Puts all input files in the private storage files.
  void get(Solver::Ptr solver) override;

 private:
  /// @brief  Runs through a given file path and proceedes all include files.
  /// @return True on success, false if the path doesn't exist.
  /// @param base_path The base path in which the search is started.
  /// @param solver Pointer to the solver instance
  /// @param sub_path The sub_path (within base_path) which is processed
  /// @param recusive_cnt The current recursive counter.
  bool walk_tree(const std::string &base_path, const Solver::Ptr &solver,
                 const std::string &sub_path = "", int recursive_cnt = 0);

  /// @brief Helper function to check if a file should be excluded.
  bool exclude_check(const std::string &path_string) const;

  /// @brief Regular expression to check if a file shall be used.
  const boost::regex file_regex;

  /// @brief Vector of regular expressions to check if a file shall not
  ///        be used.
  const std::vector<boost::regex> exclude_regex;

  /// @brief Paths of the base directories.
  const std::vector<std::string> process_paths;

  /// @brief Indicates if excludes are used.
  const bool use_exclude_regex;

  /// @brief Limit for the recursive file search.
  const int recursive_limit;

};  // class File_Detector

}  // namespace INCLUDE_GARDENER

#endif  // FILE_DETECTOR_H

// vim: filetype=cpp et ts=2 sw=2 sts=2
