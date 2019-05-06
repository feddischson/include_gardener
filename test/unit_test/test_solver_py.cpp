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

#include "graph.h"
#include "solver.h"
#include "solver_py.h"
#include "statement_detector.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>
#include <sstream>

using INCLUDE_GARDENER::Edge;
using INCLUDE_GARDENER::Edge_Descriptor;
using INCLUDE_GARDENER::Solver;
using INCLUDE_GARDENER::Solver_Py;
using INCLUDE_GARDENER::Statement_Detector;
using INCLUDE_GARDENER::Vertex;

using std::endl;
using std::istream;
using std::make_shared;
using std::optional;
using std::pair;
using std::string;
using std::stringstream;
using std::vector;
using testing::_;
using testing::Contains;
using testing::Ge;
using testing::HasSubstr;
using testing::Not;
using testing::Return;
using testing::SizeIs;
using testing::StrEq;
using testing::StrNe;

using boost::filesystem::path;

class SolverPyTest : public ::testing::Test, public Solver_Py {
 public:
  using Solver_Py::is_module;
  using Solver_Py::is_package;
};

class Mock_Solver_Py : public Solver_Py {
 public:
  Mock_Solver_Py() = default;
  MOCK_METHOD1(is_module, bool(const std::string &path_string));
  MOCK_METHOD1(is_package, bool(const std::string &path_string));
  MOCK_METHOD4(add_edge, void(const string &, const string &, unsigned int,
                              unsigned int));
};

class Mock_Statement_Detector : public Statement_Detector {
 public:
  explicit Mock_Statement_Detector(const Solver::Ptr &solver)
      : Statement_Detector(solver, 0) {}
  optional<pair<string, unsigned int>> call_detect(const string &line) const {
    return detect(line);
  }

  void call_process_stream(istream &input, const string &p) {
    process_stream(input, p);
  }
};

// NOLINTNEXTLINE
TEST_F(SolverPyTest, EmptyInitialization) {
  auto s = make_shared<Mock_Solver_Py>();
  auto d = make_shared<Statement_Detector>(s);
  EXPECT_GT(d->get_statements().size(), 0);
  d->wait_for_workers();
}

// NOLINTNEXTLINE
TEST_F(SolverPyTest, SimpleDetection) {
  auto s = make_shared<Mock_Solver_Py>();
  auto d = make_shared<Mock_Statement_Detector>(s);
  auto res = d->call_detect("  import xyz");
  EXPECT_EQ(static_cast<bool>(res), true);
  EXPECT_EQ(res->first, "xyz");
  EXPECT_EQ(res->second, 0);
  d->wait_for_workers();
}

// NOLINTNEXTLINE
TEST_F(SolverPyTest, NoMatch) {
  auto s = make_shared<Mock_Solver_Py>();
  auto d = make_shared<Mock_Statement_Detector>(s);

  stringstream sstream;
  sstream << "def fooimport:" << endl;
  sstream << "iomport xxx" << endl;  // invalid statement
  sstream << "xyz" << endl;

  EXPECT_CALL(*s, add_edge(_, _, _, _)).Times(0);
  d->call_process_stream(sstream, "id");
  d->wait_for_workers();
}

// NOLINTNEXTLINE
TEST_F(SolverPyTest, MultiMatch) {
  auto s = make_shared<Mock_Solver_Py>();
  auto d = make_shared<Mock_Statement_Detector>(s);

  stringstream sstream;

  // Implicit relative or standard library imports

  sstream << "import yyy" << endl;
  EXPECT_CALL(*s, add_edge("id", "yyy", 0, _));

  sstream << "from abc import xxx" << endl;
  EXPECT_CALL(*s, add_edge("id", "abc import xxx", 1, _));

  // Absolute imports

  sstream << "import package.yyy" << endl;
  EXPECT_CALL(*s, add_edge("id", "package.yyy", 0, _));

  sstream << "from package.abc import xxx" << endl;
  EXPECT_CALL(*s, add_edge("id", "package.abc import xxx", 1, _));

  // Relative imports

  sstream << "import .yyy" << endl;
  EXPECT_CALL(*s, add_edge("id", ".yyy", 0, _));

  sstream << "from .abc import xxx" << endl;
  EXPECT_CALL(*s, add_edge("id", ".abc import xxx", 1, _));

  sstream << "import ..yyy" << endl;
  EXPECT_CALL(*s, add_edge("id", "..yyy", 0, _));

  sstream << "from ..abc import xxx" << endl;
  EXPECT_CALL(*s, add_edge("id", "..abc import xxx", 1, _));

  sstream << "import ...yyy" << endl;
  EXPECT_CALL(*s, add_edge("id", "...yyy", 0, _));

  sstream << "from ...abc import xxx" << endl;
  EXPECT_CALL(*s, add_edge("id", "...abc import xxx", 1, _));

  sstream << "import ...package.yyy" << endl;
  EXPECT_CALL(*s, add_edge("id", "...package.yyy", 0, _));

  sstream << "from ...package.abc import xxx" << endl;
  EXPECT_CALL(*s, add_edge("id", "...package.abc import xxx", 1, _));

  d->call_process_stream(sstream, "id");
  d->wait_for_workers();
}

// NOLINTNEXTLINE
TEST_F(SolverPyTest, MultiLineMatch) {
  auto s = make_shared<Mock_Solver_Py>();
  auto d = make_shared<Mock_Statement_Detector>(s);

  string multi_line_string = R"(from yyy\
                             import xxx
                             import\
                             \
                             \
                             \
                             zzz)";
  stringstream sstream(multi_line_string);

  EXPECT_CALL(*s, add_edge(_, _, _, Ge(2))).Times(2);
  d->call_process_stream(sstream, "id");
  d->wait_for_workers();
}

// vim: filetype=cpp et ts=2 sw=2 sts=2
