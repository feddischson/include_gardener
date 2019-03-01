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

namespace INCLUDE_GARDENER {

namespace po = boost::program_options;
using std::mutex;
using std::string;
using std::unique_lock;
using std::vector;

vector<string> Solver_Py::get_statement_regex() const {
  vector<string> regex_str = {};
  return regex_str;
}

string Solver_Py::get_file_regex() const { return string("^[^\\d\\W]\\w*\\.(?i)py[3w]?$"); }

void Solver_Py::add_options(po::options_description *options __attribute__((unused))) {

}

void Solver_Py::extract_options(const po::variables_map &vm __attribute__((unused))) {

}

void Solver_Py::add_edge(const string &src_path __attribute__((unused)),
                         const string &statement __attribute__((unused)),
                         unsigned int idx __attribute__((unused)),
                         unsigned int line_no __attribute__((unused))) {
}

void Solver_Py::insert_edge(const std::string &src_path __attribute__((unused)),
                            const std::string &dst_path __attribute__((unused)),
                            const std::string &name __attribute__((unused)),
                            unsigned int line_no __attribute__((unused))) {
}

}  // namespace INCLUDE_GARDENER

// vim: filetype=cpp et ts=2 sw=2 sts=2
