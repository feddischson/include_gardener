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
#include "solver_py.h"

#include <gtest/gtest.h>

using INCLUDE_GARDENER::File_Detector;
using INCLUDE_GARDENER::Solver_Py;

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

// Python
// NOLINTNEXTLINE
TEST_F(File_Detector_Test, test_py_file_detection_without_exclude) {
  // Use the Python solver to import the file regex
  Solver_Py solver = Solver_Py();
  string file_regex = solver.get_file_regex();
  vector<string> exclude_regex;
  vector<string> base_paths;
  File_Detector d = File_Detector(file_regex, exclude_regex, base_paths);

  EXPECT_EQ(d.use_file("abc.py"), true);
  EXPECT_EQ(d.use_file("abc.Py"), true);
  EXPECT_EQ(d.use_file("def.pY"), true);
  EXPECT_EQ(d.use_file("ghi.PY"), true);
  EXPECT_EQ(d.use_file("jkl.py3"), true);
  EXPECT_EQ(d.use_file("mno.pY3"), true);
  EXPECT_EQ(d.use_file("pqr.PY3"), true);
  EXPECT_EQ(d.use_file("stu.Py3"), true);
  EXPECT_EQ(d.use_file("jkl.pyw"), true);
  EXPECT_EQ(d.use_file("mno.pYw"), true);
  EXPECT_EQ(d.use_file("pqr.PYw"), true);
  EXPECT_EQ(d.use_file("stu.Pyw"), true);
  EXPECT_EQ(d.use_file("vwx.pyW"), true);
  EXPECT_EQ(d.use_file("yza.pYW"), true);
  EXPECT_EQ(d.use_file("bcd.PYW"), true);
  EXPECT_EQ(d.use_file("efg.PyW"), true);
  EXPECT_EQ(d.use_file(".py"), false);
  EXPECT_EQ(d.use_file(".Py"), false);
  EXPECT_EQ(d.use_file(".pY"), false);
  EXPECT_EQ(d.use_file(".PY"), false);
  EXPECT_EQ(d.use_file(".Py3"), false);
  EXPECT_EQ(d.use_file(".pY3"), false);
  EXPECT_EQ(d.use_file(".PY3"), false);
  EXPECT_EQ(d.use_file(".pYw"), false);
  EXPECT_EQ(d.use_file(".PYw"), false);
  EXPECT_EQ(d.use_file(".Pyw"), false);
  EXPECT_EQ(d.use_file(".pyW"), false);
  EXPECT_EQ(d.use_file(".pYW"), false);
  EXPECT_EQ(d.use_file(".PYW"), false);
  EXPECT_EQ(d.use_file(".PyW"), false);
  EXPECT_EQ(d.use_file("1234567890.py"), false);
  EXPECT_EQ(d.use_file("_1.py"), true);
  EXPECT_EQ(d.use_file("_.py"), true);
  EXPECT_EQ(d.use_file("abc.pyc"), false);
  EXPECT_EQ(d.use_file("y z.py"), false);
  EXPECT_EQ(d.use_file("y z.pyw"), false);
  EXPECT_EQ(d.use_file("y z.py3"), false);
  EXPECT_EQ(d.use_file("x."), false);
  EXPECT_EQ(d.use_file("x.c"), false);
  EXPECT_EQ(d.use_file("x.h"), false);
}

// NOLINTNEXTLINE
TEST_F(File_Detector_Test, test_py_file_detection_with_exclude) {
  // Use the Python solver to import the file regex
  Solver_Py solver = Solver_Py();
  string file_regex = solver.get_file_regex();
  vector<string> exclude_regex;
  vector<string> base_paths;

  // exclude all *_tmp.py[3w] files (in any case)
  exclude_regex.emplace_back(".*_tmp\\.[pP][yY][3wW]?$");

  File_Detector d = File_Detector(file_regex, exclude_regex, base_paths);

  EXPECT_EQ(d.use_file("abc.py"), true);
  EXPECT_EQ(d.use_file("abc.Py"), true);
  EXPECT_EQ(d.use_file("def.pY"), true);
  EXPECT_EQ(d.use_file("ghi.PY"), true);
  EXPECT_EQ(d.use_file("jkl.py3"), true);
  EXPECT_EQ(d.use_file("mno.pY3"), true);
  EXPECT_EQ(d.use_file("pqr.PY3"), true);
  EXPECT_EQ(d.use_file("stu.Py3"), true);
  EXPECT_EQ(d.use_file("jkl.pyw"), true);
  EXPECT_EQ(d.use_file("mno.pYw"), true);
  EXPECT_EQ(d.use_file("pqr.PYw"), true);
  EXPECT_EQ(d.use_file("stu.Pyw"), true);
  EXPECT_EQ(d.use_file("vwx.pyW"), true);
  EXPECT_EQ(d.use_file("yza.pYW"), true);
  EXPECT_EQ(d.use_file("bcd.PYW"), true);
  EXPECT_EQ(d.use_file("efg.PyW"), true);
  EXPECT_EQ(d.use_file(".py"), false);
  EXPECT_EQ(d.use_file(".Py"), false);
  EXPECT_EQ(d.use_file(".pY"), false);
  EXPECT_EQ(d.use_file(".PY"), false);
  EXPECT_EQ(d.use_file(".Py3"), false);
  EXPECT_EQ(d.use_file(".pY3"), false);
  EXPECT_EQ(d.use_file(".PY3"), false);
  EXPECT_EQ(d.use_file(".pYw"), false);
  EXPECT_EQ(d.use_file(".PYw"), false);
  EXPECT_EQ(d.use_file(".Pyw"), false);
  EXPECT_EQ(d.use_file(".pyW"), false);
  EXPECT_EQ(d.use_file(".pYW"), false);
  EXPECT_EQ(d.use_file(".PYW"), false);
  EXPECT_EQ(d.use_file(".PyW"), false);
  EXPECT_EQ(d.use_file("1234567890.py"), false);
  EXPECT_EQ(d.use_file("_1.py"), true);
  EXPECT_EQ(d.use_file("_.py"), true);
  EXPECT_EQ(d.use_file("abc.pyc"), false);
  EXPECT_EQ(d.use_file("y z.py"), false);
  EXPECT_EQ(d.use_file("y z.pyw"), false);
  EXPECT_EQ(d.use_file("y z.py3"), false);
  EXPECT_EQ(d.use_file("x."), false);
  EXPECT_EQ(d.use_file("x.c"), false);
  EXPECT_EQ(d.use_file("x.h"), false);

  // the same as the positive cases above but with a "_tmp"
  EXPECT_EQ(d.use_file("abc_tmp.py"), false);
  EXPECT_EQ(d.use_file("abc_tmp.Py"), false);
  EXPECT_EQ(d.use_file("def_tmp.pY"), false);
  EXPECT_EQ(d.use_file("ghi_tmp.PY"), false);
  EXPECT_EQ(d.use_file("jkl_tmp.py3"), false);
  EXPECT_EQ(d.use_file("mno_tmp.pY3"), false);
  EXPECT_EQ(d.use_file("pqr_tmp.PY3"), false);
  EXPECT_EQ(d.use_file("stu_tmp.Py3"), false);
  EXPECT_EQ(d.use_file("jkl_tmp.pyw"), false);
  EXPECT_EQ(d.use_file("mno_tmp.pYw"), false);
  EXPECT_EQ(d.use_file("pqr_tmp.PYw"), false);
  EXPECT_EQ(d.use_file("_1_tmp.py"), false);
  EXPECT_EQ(d.use_file("_tmp.py"), false);
}

// vim: filetype=cpp et ts=2 sw=2 sts=2
