// Include-Gardener
//
// Copyright (C) 2018  Christian Haettich [feddischson]
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
#include "statement_detector.h"
#include "helper.h"

using namespace std;

namespace INCLUDE_GARDENER {

Statement_Detector::Statement_Detector(const vector<string>& statement_vector)
    : statements(init_regex_vector(statement_vector)) {}

std::vector<std::regex> Statement_Detector::get_statements() {
  return statements;
}

}  // namespace INCLUDE_GARDENER
   // vim: filetype=cpp et ts=2 sw=2 sts=2
