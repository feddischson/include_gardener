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

#include "input_files.h"
#include "solver.h"

using INCLUDE_GARDENER::Input_Files;
using INCLUDE_GARDENER::Solver;
using std::list;
using std::string;

class Input_Files_Test : public ::testing::Test {};

class Mock_Input_Files : public Input_Files {
 public:
  explicit Mock_Input_Files(list<string> val) : Input_Files(std::move(val)) {}
  // NOLINTNEXTLINE
  void get(Solver::Ptr) override {}
};

// NOLINTNEXTLINE
TEST(Input_Files_Test, test_looping) {
  list<string> val = {"abc", "def", "xyz", "123", "4", "5"};
  list<string> res;
  Mock_Input_Files input_files(val);

  for (const auto& x : input_files) {
    res.emplace_back(x);
  }
  EXPECT_EQ(val, res);
}

// vim: filetype=cpp et ts=2 sw=2 sts=2
