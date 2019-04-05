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
#include "solver_py.h"
#include "solver.h"
#include "statement_detector.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>
#include <sstream>

using INCLUDE_GARDENER::Solver_Py;
using INCLUDE_GARDENER::Statement_Py;
using INCLUDE_GARDENER::Solver;
using INCLUDE_GARDENER::Statement_Detector;
using INCLUDE_GARDENER::Vertex;
using INCLUDE_GARDENER::Edge;
using INCLUDE_GARDENER::Edge_Descriptor;

using std::stringstream;
using std::string;
using std::vector;
using std::optional;
using std::pair;
using std::istream;
using std::make_shared;
using std::endl;

using boost::filesystem::path;

// Tests public functions
class StatementPyTest : public ::testing::Test {
};

// Tests protected functions
class StatementPyWrapper : public Statement_Py {

  public:
    StatementPyWrapper(): Statement_Py("a", "b", 0, 0){}

    void remove_quotes(string &statement) {
      remove_all_quotation_marks(statement);
    }

    void remove_as_stmnt(string &statement){
      remove_as_statements(statement);
    }

    void remove_dots_in_front(string &statement){
      remove_prepended_dots(statement);
    }

    void remove_spaces(string &statement){
      remove_whitespace(statement);
    }

    vector<string> split_at_comma(const string &statement){
      return split_by_comma(statement);
    }

    bool is_relative(const string &statement){
      return is_relative_import(statement);
    }

    string dot_to_slash(const string &statement){
      return dots_to_system_slash(statement);
    }

    void add_dot_in_front(string &statement){
      add_relative_dot_in_front(statement);
    }

    void add_string_in_front(string &statement,
                                      const string &add_in_front){
      add_package_name_in_front(statement, add_in_front);
    }

    string get_before_import(const string &statement){
      return get_all_before_import(statement);
    }

    string get_after_import(const string &statement){
      return get_all_after_import(statement);
    }

    bool contains_comma_char(const string &statement){
      return contains_comma(statement);
    }

    bool contains_star_char(const string &statement){
      return contains_star(statement);
    }

    /// @brief Counts how many directories above
    /// the import may be (number of dots - 1).
    unsigned int dots_dirs_above(const string &statement){
      return how_many_directories_above(statement);
    }

    /// @brief Splits an import statement that contains
    /// multiple imports in to child Statement_Py:s and
    /// places these in this instance's vector.
    void split_into_internal_vector(const string &src_path,
                                    const string &statement,
                                    unsigned int idx,
                                    unsigned int line_no){
      split_into_multiple_statements(src_path, statement, idx, line_no);
    }
};

TEST_F(StatementPyTest, CorrectInitialization){

  string statement = "import x";
  string src_path = "/src/path";
  unsigned int regex_idx = 0;
  unsigned int line_no = 0;

  Statement_Py statement_py(src_path, statement, regex_idx, line_no);

  EXPECT_THAT(statement_py.get_original_statement(), statement);
  EXPECT_THAT(statement_py.get_source_path(), src_path);
  EXPECT_THAT(statement_py.get_regex_idx(), regex_idx);
  EXPECT_THAT(statement_py.get_line_number(), line_no);
}

TEST_F(StatementPyTest, SimpleImport) {

  // "import x" was matched, which caught "x"
  string statement = "x";
  string src_path = "/src/path";
  unsigned int regex_idx = 0;
  unsigned int line_no = 0;

  Statement_Py statement_py(src_path, statement, regex_idx, line_no);

  EXPECT_EQ(statement_py.get_directories_above(), 0);
  EXPECT_EQ(statement_py.get_is_relative(), false);
  EXPECT_EQ(statement_py.get_had_star(), false);
  EXPECT_EQ(statement_py.contained_multiple_imports(), false);
  EXPECT_EQ(statement_py.extract_dummy_node_name("x"), "x");
  EXPECT_EQ(statement_py.get_modified_statement(), "x");
  EXPECT_EQ(statement_py.get_possible_path(), "x");
}

TEST_F(StatementPyTest, PackageImport) {

  // "import x" was matched, which caught "x"
  string statement = "x.y";
  string src_path = "/src/path";
  unsigned int regex_idx = 0;
  unsigned int line_no = 0;

  Statement_Py statement_py(src_path, statement, regex_idx, line_no);

  EXPECT_EQ(statement_py.get_directories_above(), 0);
  EXPECT_EQ(statement_py.get_is_relative(), false);
  EXPECT_EQ(statement_py.get_had_star(), false);
  EXPECT_EQ(statement_py.contained_multiple_imports(), false);
  EXPECT_EQ(statement_py.extract_dummy_node_name(statement), "x.y");
  EXPECT_EQ(statement_py.get_modified_statement(), "x.y");
  EXPECT_EQ(statement_py.get_possible_path(), "x/y");
}

TEST_F(StatementPyTest, FromPackageImport) {

  // "from x.y import z" was matched, which caught below.
  string statement = "x.y import z";
  string src_path = "/src/path";
  unsigned int regex_idx = 1;
  unsigned int line_no = 0;

  Statement_Py statement_py(src_path, statement, regex_idx, line_no);

  EXPECT_EQ(statement_py.get_directories_above(), 0);
  EXPECT_EQ(statement_py.get_is_relative(), false);
  EXPECT_EQ(statement_py.get_had_star(), false);
  EXPECT_EQ(statement_py.contained_multiple_imports(), false);
  EXPECT_EQ(statement_py.extract_dummy_node_name(statement), "x.y");
  EXPECT_EQ(statement_py.get_modified_statement(), "x.y.z");
  EXPECT_EQ(statement_py.get_possible_path(), "x/y/z");
}

TEST_F(StatementPyTest, RelativeImports) {

  // "from x.y import z" was matched, which caught below.
  string statement = "..x";
  string src_path = "/src/path";
  unsigned int regex_idx = 0;
  unsigned int line_no = 0;

  Statement_Py statement_py_a(src_path, statement, regex_idx, line_no);

  EXPECT_EQ(statement_py_a.get_directories_above(), 1);
  EXPECT_EQ(statement_py_a.get_is_relative(), true);
  EXPECT_EQ(statement_py_a.get_had_star(), false);
  EXPECT_EQ(statement_py_a.contained_multiple_imports(), false);
  EXPECT_EQ(statement_py_a.extract_dummy_node_name(statement), "..x");
  EXPECT_EQ(statement_py_a.get_modified_statement(), "x");
  EXPECT_EQ(statement_py_a.get_possible_path(), "x");
  EXPECT_EQ(statement_py_a.get_child_statements().empty(), true);

  statement = "...x import y";
  regex_idx = 1;

  Statement_Py statement_py_b(src_path, statement, regex_idx, line_no);

  EXPECT_EQ(statement_py_b.get_directories_above(), 2);
  EXPECT_EQ(statement_py_b.get_is_relative(), true);
  EXPECT_EQ(statement_py_b.get_had_star(), false);
  EXPECT_EQ(statement_py_b.contained_multiple_imports(), false);
  EXPECT_EQ(statement_py_b.extract_dummy_node_name(statement), "...x");
  EXPECT_EQ(statement_py_b.get_modified_statement(), "x.y");
  EXPECT_EQ(statement_py_b.get_possible_path(), "x/y");
  EXPECT_EQ(statement_py_b.get_child_statements().empty(), true);

  statement = "....x import *";
  regex_idx = 1;

  Statement_Py statement_py_c(src_path, statement, regex_idx, line_no);

  EXPECT_EQ(statement_py_c.get_directories_above(), 3);
  EXPECT_EQ(statement_py_c.get_is_relative(), true);
  EXPECT_EQ(statement_py_c.get_had_star(), true);
  EXPECT_EQ(statement_py_c.contained_multiple_imports(), false);
  EXPECT_EQ(statement_py_c.extract_dummy_node_name(statement), "....x");
  EXPECT_EQ(statement_py_c.get_modified_statement(), "x");
  EXPECT_EQ(statement_py_c.get_possible_path(), "x");
  EXPECT_EQ(statement_py_c.get_regex_idx(), 0); // Note: Modified for *-imports
  EXPECT_EQ(statement_py_c.get_child_statements().empty(), true);

  statement = ".x import y, z, a";
  regex_idx = 1;

  Statement_Py statement_py_d(src_path, statement, regex_idx, line_no);

  EXPECT_EQ(statement_py_d.get_directories_above(), 0);
  EXPECT_EQ(statement_py_d.get_is_relative(), true);
  EXPECT_EQ(statement_py_d.get_had_star(), false);
  EXPECT_EQ(statement_py_d.contained_multiple_imports(), true);
  EXPECT_EQ(statement_py_d.extract_dummy_node_name(statement), ".x");
  EXPECT_EQ(statement_py_d.get_child_statements().empty(), false);

  vector<Statement_Py> children = statement_py_d.get_child_statements();

  EXPECT_EQ(children.size(), 3);  // .x.y, .x.z, .x.a

  for (auto child : children){
    EXPECT_EQ(child.contained_multiple_imports(), false);
    EXPECT_EQ(child.get_had_star(), false);
    EXPECT_EQ(child.get_child_statements().empty(), true);

    // These should be "inherited" from parent statement
    EXPECT_EQ(child.get_line_number(), line_no);
    EXPECT_EQ(child.get_source_path(), src_path);

    // Is modified when passing on to normal import
    EXPECT_EQ(child.get_regex_idx(), 0);
  }

  EXPECT_EQ(children[0].get_modified_statement(), "x.y");
  EXPECT_EQ(children[0].get_original_statement(), ".x.y");
  EXPECT_EQ(children[0].get_possible_path(), "x/y");

  EXPECT_EQ(children[1].get_modified_statement(), "x.z");
  EXPECT_EQ(children[1].get_original_statement(), ".x.z");
  EXPECT_EQ(children[1].get_possible_path(), "x/z");

  EXPECT_EQ(children[2].get_modified_statement(), "x.a");
  EXPECT_EQ(children[2].get_original_statement(), ".x.a");
  EXPECT_EQ(children[2].get_possible_path(), "x/a");

}

TEST_F(StatementPyTest, RemovalStringFunctions){

  StatementPyWrapper wrapper;

  string test_string = ".....\"hello\" . 'world' as earth";
  wrapper.remove_quotes(test_string);
  EXPECT_EQ(test_string, ".....hello . world as earth");
  wrapper.remove_dots_in_front(test_string);
  EXPECT_EQ(test_string, "hello . world as earth");
  wrapper.remove_as_stmnt(test_string);
  EXPECT_EQ(test_string, "hello . world");
  wrapper.remove_spaces(test_string);
  EXPECT_EQ(test_string, "hello.world");
}

TEST_F(StatementPyTest, SplitAtComma){
  StatementPyWrapper wrapper;
  vector<string> str_split = wrapper.split_at_comma("a,b,c,d");
  EXPECT_EQ(str_split.size(), 4);

  for (auto str : str_split){
    EXPECT_EQ(str.length(), 1);
  }

  EXPECT_EQ(str_split.at(0), "a");
  EXPECT_EQ(str_split.at(3), "d");

  str_split = wrapper.split_at_comma("  a, b ,c  , d ");

  for (auto str : str_split){
    EXPECT_EQ(str.length(), 3);
  }

  EXPECT_EQ(str_split.at(0), "  a");
  EXPECT_EQ(str_split.at(3), " d ");

}

TEST_F(StatementPyTest, BeforeAndAfterImportFunctions) {

  // Asserts that everything after or before " import " is returned
  string s = "from dimport import importx";
  StatementPyWrapper wrapper;

  string before = wrapper.get_before_import(s);
  string after = wrapper.get_after_import(s);

  EXPECT_THAT(before, "from dimport");
  EXPECT_THAT(after, "importx");
  EXPECT_THAT(boost::contains(before, " import "), false);
  EXPECT_THAT(boost::contains(after, " import "), false);
}

TEST_F(StatementPyTest, DotsToSlash) {
  StatementPyWrapper wrapper;

  string input = "string.with.dots";
  path expected{"string"};
  expected /= "with";
  expected /= "dots";
  string output = wrapper.dot_to_slash(input);

  EXPECT_THAT(boost::contains(output, "/")
              || boost::contains(output, "\\"), true);
  EXPECT_THAT(boost::contains(output, "."), false);
  EXPECT_THAT(output, expected);

  input = "string    .    with.   dots";
  path expected_2{"string    "};
  expected_2 /= "    with";
  expected_2 /= "   dots";
  output = wrapper.dot_to_slash(input);

  EXPECT_THAT(boost::contains(output, "/")
              || boost::contains(output, "\\"), true);
  EXPECT_THAT(boost::contains(output, "."), false);
  EXPECT_THAT(output, expected_2);
}

TEST_F(StatementPyTest, DirectoriesAbove) {
  const string none = " .no.dir.above";
  const string one = " ..one.dir.above";
  const string twenty = " .....................twenty.dirs.above";

  StatementPyWrapper wrapper;
  EXPECT_EQ(wrapper.dots_dirs_above(none), 0);
  EXPECT_EQ(wrapper.dots_dirs_above(one), 1);
  EXPECT_EQ(wrapper.dots_dirs_above(twenty), 20);
}

TEST_F(StatementPyTest, IsRelativeImport) {
  const string dot_beginning = "    .yes.it.does";
  const string dot_beginning_too = ".dot";
  const string no_dot_beginning = "no.dots";

  StatementPyWrapper wrapper;
  EXPECT_TRUE(wrapper.is_relative(dot_beginning));
  EXPECT_TRUE(wrapper.is_relative(dot_beginning_too));
  EXPECT_FALSE(wrapper.is_relative(no_dot_beginning));
}

TEST_F(StatementPyTest, RemoveStartingDots) {
  StatementPyWrapper wrapper;

  string dot_beginning = "....dots";
  string dot_beginning_too = ".dot";
  string no_dot_beginning = "no.dots";

  wrapper.remove_dots_in_front(dot_beginning);
  wrapper.remove_dots_in_front(dot_beginning_too);
  wrapper.remove_dots_in_front(no_dot_beginning);

  EXPECT_THAT(dot_beginning, "dots");
  EXPECT_THAT(dot_beginning_too, "dot");
  EXPECT_THAT(no_dot_beginning, "no.dots");
}

TEST_F(StatementPyTest, ContainsCharacter){
  StatementPyWrapper wrapper;

  EXPECT_THAT(wrapper.contains_star_char("has no star"), false);
  EXPECT_THAT(wrapper.contains_star_char("has star*"), true);

  EXPECT_THAT(wrapper.contains_comma_char("has no comma"), false);
  EXPECT_THAT(wrapper.contains_comma_char("has, comma"), true);
}

TEST_F(StatementPyTest, SplitIntoInternalVector){
  StatementPyWrapper wrapper;

  wrapper.split_into_internal_vector("a", "x import y, z, a", 0, 0);
  vector<Statement_Py> children = wrapper.get_child_statements();
  EXPECT_THAT(children.size(), 3);
}

// vim: filetype=cpp et ts=2 sw=2 sts=2
