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

#include <gtest/gtest.h>

using INCLUDE_GARDENER::File_Detector;

using boost::regex;
using std::string;
using std::vector;

class File_Detector_Test : public ::testing::Test {};

// NOLINTNEXTLINE
TEST_F(File_Detector_Test, empty_initialization) {
  string file_regex;
  vector<string> exclude_regex;
  vector<string> base_paths;

  File_Detector d = File_Detector(file_regex, exclude_regex, base_paths);
  EXPECT_EQ(d.get_exclude_regex().size(), 0);
}

// NOLINTNEXTLINE
TEST_F(File_Detector_Test, exclude_list) {
  string file_regex;
  vector<string> exclude_regex;
  vector<string> base_paths;
  exclude_regex.emplace_back("abc");
  exclude_regex.emplace_back("def");

  File_Detector d = File_Detector(file_regex, exclude_regex, base_paths);
  auto l = d.get_exclude_regex();

  regex r1 = regex("abc");
  regex r2 = regex("def");

  EXPECT_EQ(l.size(), 2);
  EXPECT_EQ(r1.flags(), l[0].flags());
  EXPECT_EQ(r2.flags(), l[1].flags());
  // note: it is not possible to compare the regex it self
}

// NOLINTNEXTLINE
TEST_F(File_Detector_Test, test_c_file_detection_without_exclude) {
  string file_regex = "(.*)\\.(c|cc|h|hh|cpp|hpp)";
  vector<string> exclude_regex;
  vector<string> base_paths;
  File_Detector d = File_Detector(file_regex, exclude_regex, base_paths);

  EXPECT_EQ(d.use_file("abc.H"), true);
  EXPECT_EQ(d.use_file("abc.h"), true);
  EXPECT_EQ(d.use_file("def.hh"), true);
  EXPECT_EQ(d.use_file(".c"), true);
  EXPECT_EQ(d.use_file(".C"), true);
  EXPECT_EQ(d.use_file("1234567890.cc"), true);
  EXPECT_EQ(d.use_file("x.hpp"), true);
  EXPECT_EQ(d.use_file(".c"), true);
  EXPECT_EQ(d.use_file("y z.cpp"), true);
  EXPECT_EQ(d.use_file("y z.cPp"), true);
  EXPECT_EQ(d.use_file("y z.py"), false);
  EXPECT_EQ(d.use_file("x."), false);
}

// NOLINTNEXTLINE
TEST_F(File_Detector_Test, test_c_file_detection_with_exclude) {
  string file_regex = "(.*)\\.(c|cc|h|hh|cpp|hpp)";
  vector<string> exclude_regex;
  vector<string> base_paths;

  // exclude all *_tmp.c and *-k.c files
  exclude_regex.emplace_back("(.*)_tmp\\.(c|cc|h|hh|cpp|hpp)");
  exclude_regex.emplace_back("(.*)-k\\.(c|cc|h|hh|cpp|hpp)");

  File_Detector d = File_Detector(file_regex, exclude_regex, base_paths);

  EXPECT_EQ(d.use_file("abc.h"), true);
  EXPECT_EQ(d.use_file("def.hh"), true);
  EXPECT_EQ(d.use_file(".c"), true);
  EXPECT_EQ(d.use_file("1234567890.cc"), true);
  EXPECT_EQ(d.use_file("x.hpp"), true);
  EXPECT_EQ(d.use_file("t.c"), true);
  EXPECT_EQ(d.use_file("y z.cpp"), true);
  EXPECT_EQ(d.use_file("y z.py"), false);
  EXPECT_EQ(d.use_file("x."), false);

  // the same as the positive cases above but with
  // the a "_tmp" and "-k"
  EXPECT_EQ(d.use_file("abc_tmp.h"), false);
  EXPECT_EQ(d.use_file("def_tmp.hh"), false);
  EXPECT_EQ(d.use_file("_tmp.c"), false);
  EXPECT_EQ(d.use_file("1234567890_tmp.cc"), false);
  EXPECT_EQ(d.use_file("x_tmp.hpp"), false);
  EXPECT_EQ(d.use_file("t_tmp.c"), false);
  EXPECT_EQ(d.use_file("y z_tmp.cpp"), false);

  EXPECT_EQ(d.use_file("abc-k.h"), false);
  EXPECT_EQ(d.use_file("def-k.hh"), false);
  EXPECT_EQ(d.use_file("-k.c"), false);
  EXPECT_EQ(d.use_file("1234567890-k.cc"), false);
  EXPECT_EQ(d.use_file("x-k.hpp"), false);
  EXPECT_EQ(d.use_file("t-k.c"), false);
  EXPECT_EQ(d.use_file("y z-k.cpp"), false);
}

// vim: filetype=cpp et ts=2 sw=2 sts=2
