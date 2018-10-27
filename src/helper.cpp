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
#include "helper.h"

using namespace std;

namespace INCLUDE_GARDENER {

/// @details
///    This function returns a regex vector, initialized by a string-vector.
vector<regex> init_regex_vector(const vector<string>& string_vector) {
  vector<regex> regex_vector;

  for (auto i = string_vector.begin(); i != string_vector.end(); i++) {
    if (i->size() > 0) {
      regex_vector.push_back(
          regex(*i, regex_constants::ECMAScript | regex_constants::icase));
    }
  }
  return regex_vector;
}

}  // namespace INCLUDE_GARDENER

// vim: filetype=cpp et ts=2 sw=2 sts=2
