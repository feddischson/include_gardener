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
#include <gtest/gtest.h>

#include <string>
#include <vector>

#include <boost/regex.hpp>

#include "helper.h"

using boost::regex;
using INCLUDE_GARDENER::init_regex_vector;
using std::string;
using std::vector;

class Helper_Test : public ::testing::Test {};

// NOLINTNEXTLINE
TEST(Helper_Test, test_init_regex_vector) {
  vector<string> arg = {"abc", "def"};
  vector<boost::regex> expected = {regex("abc"), regex("def")};
  EXPECT_EQ(init_regex_vector(arg), expected);
}

// vim: filetype=cpp et ts=2 sw=2 sts=2
