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
#include "helper.h"

using boost::regex;
using std::string;
using std::vector;

namespace INCLUDE_GARDENER {

/// @details
///    This function returns a regex vector, initialized by a string-vector.
vector<regex> init_regex_vector(const vector<string>& string_vector) {
  vector<regex> regex_vector;

  for (const auto& entry : string_vector) {
    if (!entry.empty()) {
      regex_vector.emplace_back(regex(entry));
    }
  }
  return regex_vector;
}

}  // namespace INCLUDE_GARDENER

// vim: filetype=cpp et ts=2 sw=2 sts=2
