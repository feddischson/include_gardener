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
#include <regex>

#include "file_detector.h"
#include "graph.h"
#include "solver.h"
#include "solver_rb.h"
#include "statement_detector.h"
#include "vertex.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using INCLUDE_GARDENER::Solver;
using INCLUDE_GARDENER::Solver_Rb;
using INCLUDE_GARDENER::Statement_Detector;

using std::ostringstream;
using std::string;
using std::vector;

class Solver_Rb_Test : public ::testing::Test {
  protected:
   Solver_Rb solver;
};

class Mock_Statement_Detector : public Statement_Detector {
  public:
   Mock_Statement_Detector(const Solver::Ptr &solver)
       : Statement_Detector(solver, 1) {}

   bool line_is_valid(const string &line) { return (detect(line).has_value()); }
};

// NOLINTNEXTLINE
TEST_F(Solver_Rb_Test, file_regex) {
   using INCLUDE_GARDENER::File_Detector;
   File_Detector fd(solver.get_file_regex(), vector<string>(), vector<string>(),
                    0);

   EXPECT_TRUE(fd.use_file("/path/to/ruby-file.rb"));
   EXPECT_TRUE(fd.use_file("./relative/path/to/ruby-file.rb"));
   EXPECT_FALSE(fd.use_file("/path/to/brainfk-file.bfk"));
}

// NOLINTNEXTLINE
TEST_F(Solver_Rb_Test, statement_regex) {
   Mock_Statement_Detector sd(solver.get_solver("ruby"));
   EXPECT_TRUE(sd.line_is_valid("require_relative 'banjo.rb'"));
   EXPECT_TRUE(sd.line_is_valid("require_relative 'banjo'"));
}
