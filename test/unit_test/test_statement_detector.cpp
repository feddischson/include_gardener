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
#include "statement_detector.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <sstream>

using INCLUDE_GARDENER::Solver;
using INCLUDE_GARDENER::Statement_Detector;

using std::endl;
using std::istream;
using std::make_shared;
using std::optional;
using std::pair;
using std::string;
using std::stringstream;
using std::vector;

namespace po = boost::program_options;

class Statement_Detector_Test : public ::testing::Test {};

// NOLINTNEXTLINE
class Mock_Solver : public Solver {
 public:
  Mock_Solver() = default;
  MOCK_METHOD4(add_edge, void(const string &, const string &, unsigned int,
                              unsigned int));
  vector<string> get_statement_regex() const override {
    return {R"(\s*#\s*(include|import)\s+\"(\S+)\")",
            R"(\s*#\s*(include|import)\s+<(\S+)>)"};
  }
  MOCK_CONST_METHOD0(get_file_regex, string());
  MOCK_CONST_METHOD0(name, string());
  MOCK_CONST_METHOD1(add_options, void(po::options_description *));
  MOCK_METHOD1(extract_options, void(const po::variables_map &));
};

class Mock_Statement_Detector : public Statement_Detector {
 public:
  explicit Mock_Statement_Detector(const Solver::Ptr &solver)
      : Statement_Detector(solver, 0) {}
  optional<pair<string, unsigned int>> call_detect(
      const std::string &line) const {
    return detect(line);
  }

  void call_process_stream(istream &input, const string &p) {
    process_stream(input, p);
  }
};

// NOLINTNEXTLINE
TEST_F(Statement_Detector_Test, empty_initialization) {
  auto s = make_shared<Mock_Solver>();
  auto d = make_shared<Statement_Detector>(s);
  EXPECT_EQ(d->get_statements().size(), 2);
  d->wait_for_workers();
}

// NOLINTNEXTLINE
TEST_F(Statement_Detector_Test, simple_detection) {
  auto s = make_shared<Mock_Solver>();
  auto d = make_shared<Mock_Statement_Detector>(s);
  EXPECT_EQ(d->get_statements().size(), 2);
  auto res = d->call_detect("  #include \"abc.h\"");
  EXPECT_EQ(static_cast<bool>(res), true);
  EXPECT_EQ(res->first, "abc.h");
  EXPECT_EQ(res->second, 0);
  d->wait_for_workers();
}

// NOLINTNEXTLINE
TEST_F(Statement_Detector_Test, no_detection) {
  using ::testing::_;
  auto s = make_shared<Mock_Solver>();
  auto d = make_shared<Mock_Statement_Detector>(s);

  stringstream sstream;
  sstream << "abc" << endl;
  sstream << "#incde \"abc.h\"" << endl;  // invalid statement, should not get
                                          // detected!
  sstream << "xyz" << endl;

  EXPECT_EQ(d->get_statements().size(), 2);
  EXPECT_CALL(*s, add_edge(_, _, _, _)).Times(0);
  d->call_process_stream(sstream, "id");
  d->wait_for_workers();
}

// NOLINTNEXTLINE
TEST_F(Statement_Detector_Test, detection_from_stream) {
  auto s = make_shared<Mock_Solver>();
  auto d = make_shared<Mock_Statement_Detector>(s);

  stringstream sstream;
  sstream << "abc" << endl;
  sstream << "#include \"abc.h\"" << endl;  // valid statement, should get
                                            // detected!
  sstream << "xyz" << endl;

  EXPECT_EQ(d->get_statements().size(), 2);
  EXPECT_CALL(*s, add_edge("id", "abc.h", 0, 2)).Times(1);
  d->call_process_stream(sstream, "id");
  d->wait_for_workers();
}

//
// Multi-line statement in the middle of the stream
//
// NOLINTNEXTLINE
TEST_F(Statement_Detector_Test, ml_detection_from_stream_1) {
  auto s = make_shared<Mock_Solver>();
  auto d = make_shared<Mock_Statement_Detector>(s);

  stringstream sstream;
  sstream << "abc" << endl;
  sstream << "#\\" << endl;
  sstream << "inc\\" << endl;
  sstream << "lude \\" << endl;
  sstream << "\"abc.h\"" << endl;  // valid statement, should get
                                   // detected!
  sstream << "xyz" << endl;

  EXPECT_EQ(d->get_statements().size(), 2);
  EXPECT_CALL(*s, add_edge("id", "abc.h", 0, 5)).Times(1);
  d->call_process_stream(sstream, "id");
  d->wait_for_workers();
}

//
// Multi-line statement at the end of the stream
//
// NOLINTNEXTLINE
TEST_F(Statement_Detector_Test, ml_detection_from_stream_2) {
  auto s = make_shared<Mock_Solver>();
  auto d = make_shared<Mock_Statement_Detector>(s);

  stringstream sstream;
  sstream << "abc" << endl;
  sstream << "#\\" << endl;
  sstream << "inc\\" << endl;
  sstream << "lude \\" << endl;
  sstream << "\"xyz.h\"";  // valid statement, should get
                           // detected!
  EXPECT_EQ(d->get_statements().size(), 2);
  EXPECT_CALL(*s, add_edge("other_id", "xyz.h", 0, 5)).Times(1);
  d->call_process_stream(sstream, "other_id");
  d->wait_for_workers();
}

//
// Multi-line statement at the end of the stream with
// expecting a further line.
// This should not happend in real-world, but we can handle it.
//
// NOLINTNEXTLINE
TEST_F(Statement_Detector_Test, ml_detection_from_stream_3) {
  auto s = make_shared<Mock_Solver>();
  auto d = make_shared<Mock_Statement_Detector>(s);

  stringstream sstream;
  sstream << "abc" << endl;
  sstream << "#\\" << endl;
  sstream << "inc\\" << endl;
  sstream << "lude \\" << endl;
  sstream << R"("abc.h"\)";  // valid statement, should get
                             // detected!
  EXPECT_EQ(d->get_statements().size(), 2);
  EXPECT_CALL(*s, add_edge("id", "abc.h", 0, 6)).Times(1);
  d->call_process_stream(sstream, "id");
  d->wait_for_workers();
}

//
// Different statements within the stream,
// separated by one line
//
// NOLINTNEXTLINE
TEST_F(Statement_Detector_Test, ml_detection_from_stream_4) {
  using ::testing::_;
  using ::testing::Ge;
  auto s = make_shared<Mock_Solver>();
  auto d = make_shared<Mock_Statement_Detector>(s);

  stringstream sstream;
  sstream << "abc" << endl;
  sstream << "#\\" << endl;
  sstream << "inc\\" << endl;
  sstream << "lude \\" << endl;
  sstream << "\"abc.h\"" << endl;  // valid statement, should get
                                   // detected!
  sstream << "xyz" << endl;
  sstream << "#include <xyz.h>" << endl;  // next valid statement
  sstream << "xyz" << endl;

  EXPECT_EQ(d->get_statements().size(), 2);
  EXPECT_CALL(*s, add_edge("id", _, _, Ge(5))).Times(2);
  d->call_process_stream(sstream, "id");
  d->wait_for_workers();
}

//
// Different statements within the stream,
// not separated (direct following)
//
// NOLINTNEXTLINE
TEST_F(Statement_Detector_Test, ml_detection_from_stream_5) {
  using ::testing::_;
  using ::testing::Ge;
  auto s = make_shared<Mock_Solver>();
  auto d = make_shared<Mock_Statement_Detector>(s);

  stringstream sstream;
  sstream << "abc" << endl;
  sstream << "#\\" << endl;
  sstream << "inc\\" << endl;
  sstream << "lude \\" << endl;
  sstream << "\"abc.h\"" << endl;           // valid statement, should get
                                            // detected!
  sstream << "#include \"abc.h\"" << endl;  // next valid statement
  sstream << "xyz" << endl;

  EXPECT_EQ(d->get_statements().size(), 2);
  EXPECT_CALL(*s, add_edge("id", _, _, Ge(5))).Times(2);
  d->call_process_stream(sstream, "id");
  d->wait_for_workers();
}

//
// Different statements within the stream,
// not separated (direct following), single-line
// statement first.
//
// NOLINTNEXTLINE
TEST_F(Statement_Detector_Test, ml_detection_from_stream_6) {
  using ::testing::_;
  using ::testing::Ge;
  auto s = make_shared<Mock_Solver>();
  auto d = make_shared<Mock_Statement_Detector>(s);

  stringstream sstream;
  sstream << "abc" << endl;
  sstream << "#include \"abc.h\"" << endl;  // first valid statement
  sstream << "#\\" << endl;
  sstream << "inc\\" << endl;
  sstream << "lude \\" << endl;
  sstream << "\"abc.h\"" << endl;  // next valid statement
  sstream << "xyz" << endl;

  EXPECT_EQ(d->get_statements().size(), 2);
  EXPECT_CALL(*s, add_edge("id", _, _, Ge(2))).Times(2);
  d->call_process_stream(sstream, "id");
  d->wait_for_workers();
}

// vim: filetype=cpp et ts=2 sw=2 sts=2
