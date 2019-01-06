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

#include <sstream>
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using namespace INCLUDE_GARDENER;
using namespace std;

class Statement_Detector_Test : public ::testing::Test {};

class Mock_Solver : public Solver {
 public:
  Mock_Solver() : Solver(nullptr) {}
  MOCK_METHOD0(add_edge, void());
  MOCK_METHOD0(get_statements, std::vector<string>());
};

class Mock_Statement_Detector : public Statement_Detector {
 public:
  Mock_Statement_Detector(const vector<string> &v, Solver::Ptr solver)
      : Statement_Detector(v, solver, 0) {}
  std::string call_detect(const std::string &line) const {
    return detect(line);
  }

  void call_process_stream(istream &input) { process_stream(input); }
};

TEST_F(Statement_Detector_Test, empty_initialization) {
  vector<string> string_list;
  auto s = std::make_shared<Mock_Solver>();
  auto d = std::make_shared<Statement_Detector>(string_list, s);
  EXPECT_EQ(d->get_statements().size(), 0);
  d->wait_for_workers();
}

TEST_F(Statement_Detector_Test, simple_detection) {
  vector<string> string_list;
  string_list.push_back("\\s*#\\s*(include|import)\\s+\"(\\S+)\"");
  auto s = std::make_shared<Mock_Solver>();
  auto d = std::make_shared<Mock_Statement_Detector>(string_list, s);
  EXPECT_EQ(d->get_statements().size(), 1);
  EXPECT_EQ(d->call_detect("  #include \"abc.h\""), "abc.h");
  d->wait_for_workers();
}

TEST_F(Statement_Detector_Test, no_detection) {
  vector<string> string_list;
  string_list.push_back("\\s*#\\s*(include|import)\\s+\"(\\S+)\"");
  auto s = std::make_shared<Mock_Solver>();
  auto d = std::make_shared<Mock_Statement_Detector>(string_list, s);

  stringstream sstream;
  sstream << "abc" << endl;
  sstream << "#incde \"abc.h\"" << endl;  // invalid statement, should not get
                                          // detected!
  sstream << "xyz" << endl;

  EXPECT_EQ(d->get_statements().size(), 1);
  EXPECT_CALL(*s, add_edge()).Times(0);
  d->call_process_stream(sstream);
  d->wait_for_workers();
}

TEST_F(Statement_Detector_Test, detection_from_stream) {
  vector<string> string_list;
  string_list.push_back("\\s*#\\s*(include|import)\\s+\"(\\S+)\"");
  auto s = std::make_shared<Mock_Solver>();
  auto d = std::make_shared<Mock_Statement_Detector>(string_list, s);

  stringstream sstream;
  sstream << "abc" << endl;
  sstream << "#include \"abc.h\"" << endl;  // valid statement, should get
                                            // detected!
  sstream << "xyz" << endl;

  EXPECT_EQ(d->get_statements().size(), 1);
  EXPECT_CALL(*s, add_edge()).Times(1);
  d->call_process_stream(sstream);
  d->wait_for_workers();
}

//
// Multi-line statement in the middle of the stream
//
TEST_F(Statement_Detector_Test, ml_detection_from_stream_1) {
  vector<string> string_list;
  string_list.push_back("\\s*#\\s*(include|import)\\s+\"(\\S+)\"");
  auto s = std::make_shared<Mock_Solver>();
  auto d = std::make_shared<Mock_Statement_Detector>(string_list, s);

  stringstream sstream;
  sstream << "abc" << endl;
  sstream << "#\\" << endl;
  sstream << "inc\\" << endl;
  sstream << "lude \\" << endl;
  sstream << "\"abc.h\"" << endl;  // valid statement, should get
                                   // detected!
  sstream << "xyz" << endl;

  EXPECT_EQ(d->get_statements().size(), 1);
  EXPECT_CALL(*s, add_edge()).Times(1);
  d->call_process_stream(sstream);
  d->wait_for_workers();
}

//
// Multi-line statement at the end of the stream
//
TEST_F(Statement_Detector_Test, ml_detection_from_stream_2) {
  vector<string> string_list;
  string_list.push_back("\\s*#\\s*(include|import)\\s+\"(\\S+)\"");
  auto s = std::make_shared<Mock_Solver>();
  auto d = std::make_shared<Mock_Statement_Detector>(string_list, s);

  stringstream sstream;
  sstream << "abc" << endl;
  sstream << "#\\" << endl;
  sstream << "inc\\" << endl;
  sstream << "lude \\" << endl;
  sstream << "\"abc.h\"";  // valid statement, should get
                           // detected!
  EXPECT_EQ(d->get_statements().size(), 1);
  EXPECT_CALL(*s, add_edge()).Times(1);
  d->call_process_stream(sstream);
  d->wait_for_workers();
}

//
// Multi-line statement at the end of the stream with
// expecting a further line.
// This should not happend in real-world, but we can handle it.
//
TEST_F(Statement_Detector_Test, ml_detection_from_stream_3) {
  vector<string> string_list;
  string_list.push_back("\\s*#\\s*(include|import)\\s+\"(\\S+)\"");
  auto s = std::make_shared<Mock_Solver>();
  auto d = std::make_shared<Mock_Statement_Detector>(string_list, s);

  stringstream sstream;
  sstream << "abc" << endl;
  sstream << "#\\" << endl;
  sstream << "inc\\" << endl;
  sstream << "lude \\" << endl;
  sstream << "\"abc.h\"\\";  // valid statement, should get
                             // detected!
  EXPECT_EQ(d->get_statements().size(), 1);
  EXPECT_CALL(*s, add_edge()).Times(1);
  d->call_process_stream(sstream);
  d->wait_for_workers();
}

//
// Different statements within the stream,
// separated by one line
//
TEST_F(Statement_Detector_Test, ml_detection_from_stream_4) {
  vector<string> string_list;
  string_list.push_back("\\s*#\\s*(include|import)\\s+\"(\\S+)\"");
  string_list.push_back("\\s*#\\s*(include|import)\\s+<(\\S+)>");
  auto s = std::make_shared<Mock_Solver>();
  auto d = std::make_shared<Mock_Statement_Detector>(string_list, s);

  stringstream sstream;
  sstream << "abc" << endl;
  sstream << "#\\" << endl;
  sstream << "inc\\" << endl;
  sstream << "lude \\" << endl;
  sstream << "\"abc.h\"" << endl;  // valid statement, should get
                                   // detected!
  sstream << "xyz" << endl;
  sstream << "#include <abc.h>" << endl;  // next valid statement
  sstream << "xyz" << endl;

  EXPECT_EQ(d->get_statements().size(), 2);
  EXPECT_CALL(*s, add_edge()).Times(2);
  d->call_process_stream(sstream);
  d->wait_for_workers();
}

//
// Different statements within the stream,
// not separated (direct following)
//
TEST_F(Statement_Detector_Test, ml_detection_from_stream_5) {
  vector<string> string_list;
  string_list.push_back("\\s*#\\s*(include|import)\\s+\"(\\S+)\"");
  string_list.push_back("\\s*#\\s*(include|import)\\s+<(\\S+)>");
  auto s = std::make_shared<Mock_Solver>();
  auto d = std::make_shared<Mock_Statement_Detector>(string_list, s);

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
  EXPECT_CALL(*s, add_edge()).Times(2);
  d->call_process_stream(sstream);
  d->wait_for_workers();
}

//
// Different statements within the stream,
// not separated (direct following), single-line
// statement first.
//
TEST_F(Statement_Detector_Test, ml_detection_from_stream_6) {
  vector<string> string_list;
  string_list.push_back("\\s*#\\s*(include|import)\\s+\"(\\S+)\"");
  string_list.push_back("\\s*#\\s*(include|import)\\s+<(\\S+)>");
  auto s = std::make_shared<Mock_Solver>();
  auto d = std::make_shared<Mock_Statement_Detector>(string_list, s);

  stringstream sstream;
  sstream << "abc" << endl;
  sstream << "#include \"abc.h\"" << endl;  // first valid statement
  sstream << "#\\" << endl;
  sstream << "inc\\" << endl;
  sstream << "lude \\" << endl;
  sstream << "\"abc.h\"" << endl;  // next valid statement
  sstream << "xyz" << endl;

  EXPECT_EQ(d->get_statements().size(), 2);
  EXPECT_CALL(*s, add_edge()).Times(2);
  d->call_process_stream(sstream);
  d->wait_for_workers();
}

// vim: filetype=cpp et ts=2 sw=2 sts=2
