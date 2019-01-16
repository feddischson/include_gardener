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
#include "file_detector.h"
#include "helper.h"

#include <boost/filesystem.hpp>
#include <boost/log/trivial.hpp>
#include <boost/regex.hpp>

using boost::regex;
using std::string;
using std::vector;

namespace INCLUDE_GARDENER {

File_Detector::File_Detector(const string& file_regex,
                             const vector<string>& exclude_regex,
                             vector<string> process_paths, int recursive_limit)
    : file_regex(file_regex, boost::regex::icase),
      exclude_regex(init_regex_vector(exclude_regex)),
      process_paths(move(process_paths)),
      use_exclude_regex(!exclude_regex.empty()),
      recursive_limit(recursive_limit) {}

vector<regex> File_Detector::get_exclude_regex() { return exclude_regex; }

/// @details
///   If one of the exclude_regexes matches, false is returned.
///   If non of the eclude_regexes matches, but the file_regex,
///   true is returned. In all other cases, false is returned.
///
bool File_Detector::use_file(const std::string& file) const {
  if (use_exclude_regex && exclude_check(file)) {
    BOOST_LOG_TRIVIAL(trace) << "Excluding " << file;
    return false;
  }

  if (!regex_search(file, file_regex)) {
    BOOST_LOG_TRIVIAL(trace) << "Ignoring " << file;
    return false;
  }

  BOOST_LOG_TRIVIAL(trace) << "Considering " << file;
  return true;
}

bool File_Detector::exclude_check(const std::string& path_string) const {
  for (const auto& r : exclude_regex) {
    if (regex_search(path_string, r)) {
      return true;
    }
  }
  return false;
}

void File_Detector::get(Solver::Ptr solver) {
  using boost::filesystem::current_path;
  using boost::filesystem::exists;
  using boost::filesystem::path;
  using boost::filesystem::operator/;
  for (const auto& p : process_paths) {
    BOOST_LOG_TRIVIAL(info) << "Processing sources from " << p;
    // Check if we got a relative path.
    // If yes, convert it to an absolute path.
    path rel_path = current_path() / p;
    if (exists(rel_path)) {
      walk_tree(rel_path.string(), solver);
    } else if (exists(p)) {
      walk_tree(p, solver);
    }
  }
}

/// @details
///   Runs through all entries. If an entry is a file,  it is processed.
///   In case of an directory, a recursive call is done.
bool File_Detector::walk_tree(const string& base_path,
                              const Solver::Ptr& solver, const string& sub_path,
                              int recursive_cnt) {
  using boost::filesystem::path;
  using boost::filesystem::operator/;
  using boost::filesystem::directory_iterator;
  using boost::filesystem::exists;
  using boost::filesystem::is_directory;
  using boost::filesystem::is_regular_file;

  path p(base_path);
  p /= sub_path;

  // return false if the root-path doesn't exist
  if (!exists(p)) {
    return false;
  }

  // return false if it is not a directory
  if (!is_directory(p)) {
    return false;
  }

  directory_iterator end_itr;
  for (directory_iterator itr(p); itr != end_itr; ++itr) {
    path sub_entry(sub_path);
    sub_entry /= itr->path().filename().string();

    auto name = sub_entry.string();
    string itr_path = canonical(itr->path()).string();

    if (is_directory(itr->status())) {
      if ((recursive_limit == -1) ||
          (recursive_limit >= 0 && recursive_cnt < recursive_limit)) {
        // recursive call to process sub-directory
        walk_tree(base_path, solver, sub_entry.string(), recursive_cnt + 1);
      }
    } else if (is_regular_file(itr->status())) {
      if (!use_file(itr_path)) {
        continue;
      }

      BOOST_LOG_TRIVIAL(trace) << "(Absolute path=" << itr_path << ")";
      solver->add_vertex(name, itr_path);
      files.push_back(itr_path);
    } else {
      // ignore all other files
      BOOST_LOG_TRIVIAL(trace) << "Ignoring " << itr_path;
    }
  }
  return true;
}

}  // namespace INCLUDE_GARDENER

// vim: filetype=cpp et ts=2 sw=2 sts=2
