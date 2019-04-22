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

#ifndef STATEMENT_PY_H
#define STATEMENT_PY_H

#include <boost/filesystem.hpp>

namespace INCLUDE_GARDENER {
/// @brief Keeps information related to one Python
/// import statement and knows how to modify it.
///
/// Once a statement is passed in to the constructor
/// it is analyzed. Getters can then be used to find
/// out information about the statement.
class Statement_Py {

 public:
   /// @brief Ctor for statement
   Statement_Py(const std::string &src_path, const std::string &statement,
                unsigned int idx, unsigned int line_no);

   /// @brief Copy ctor: not implemented!
   Statement_Py(const Statement_Py &other) = default;

   /// @brief Assignment operator: not implemented!
   Statement_Py &operator=(const Statement_Py &rhs) = delete;

   /// @brief Move constructor: not implemented!
   Statement_Py(Statement_Py &&rhs) = delete;

   /// @brief Move assignment operator: not implemented!
   Statement_Py &operator=(Statement_Py &&rhs) = delete;

   /// @brief Checks if the statement given contained
   /// multiple imports. This requires the user
   /// to check the vector child_statements.
   virtual bool contained_multiple_imports();

   /// @brief Extracts a dummy name for the import.
   /// Useful if the module or package are not found.
   virtual std::string extract_dummy_node_name(const std::string &statement);

   // Getters
   virtual std::vector<Statement_Py> get_child_statements() { return child_statements; }
   virtual std::string get_source_path(){ return source_path; }
   virtual std::string get_modified_statement(){ return modified_statement; }
   virtual std::string get_possible_path(){ return possible_path; }
   virtual std::string get_original_statement(){ return original_statement; }
   virtual unsigned int get_regex_idx(){ return regex_idx; }
   virtual unsigned int get_line_number(){ return line_number; }
   virtual bool get_is_relative() { return is_relative; }
   virtual bool get_had_star() { return had_star; }

   /// @brief Get how many directories above the import statement
   /// pointed at.
   virtual unsigned int get_directories_above() { return directories_above; }

 protected:

   /// @brief Replaces all dots in a string with system-specific slash.
   virtual std::string dots_to_system_slash(const std::string &statement);

   /// @brief Evaluates whether or not the import statement
   /// is a relative import.
   virtual bool is_relative_import(const std::string &statement);

   // Functions for adding something to a string.
   virtual void add_relative_dot_in_front(std::string &statement);
   virtual void add_package_name_in_front(std::string &statement,
                                          const std::string &package_name);

   // Functions for extracting substrings from a string.
   virtual std::string get_all_before_import(const std::string &statement);
   virtual std::string get_all_after_import(const std::string &statement);

   // Functions for checking for a specific character in a string.
   virtual bool contains_comma(const std::string &statement);
   virtual bool contains_star(const std::string &statement);

   /// @brief Splits an import statement that contains
   /// multiple imports in to child Statement_Py:s and
   /// places these in this instance's vector.
   virtual void split_into_multiple_statements(const std::string &src_path,
                                           const std::string &statement,
                                           unsigned int idx,
                                           unsigned int line_no);

   /// @brief Counts how many dots a string is prepended with.
   virtual unsigned int how_many_directories_above(const std::string &statement);

   /// @brief Splits a string by comma and places results in a vector.
   virtual std::vector<std::string> split_by_comma(const std::string &statement);

   // Functions for removing something from a string.
   virtual void remove_all_quotation_marks(std::string &statement);
   virtual void remove_as_statements(std::string &statement);
   virtual void remove_prepended_dots(std::string &statement);
   virtual void remove_whitespace(std::string &statement);

 private:
    /// @brief Regex to match the first dot(s) in a string
    const std::string dot_regex = "^[ \\t]*([.]+).*$";

    /// @brief Regex to match everything past the first dot(s) in a string
    const std::string past_dot_regex = "^[ \\t]*[.]+(.*)$";

    /// @brief The original data given to this class
    const std::string source_path;
    const std::string original_statement;
    const unsigned int line_number;
    unsigned int regex_idx;

    /// @brief Information about the argument statement
    bool had_star = false;
    bool is_relative = false;
    bool contains_multiple_imports = false;
    unsigned int directories_above = 0;

    /// @brief Itinerary places to store the
    /// string as it is modified and its possible
    /// path equivalent.
    std::string modified_statement = "";
    std::string possible_path = "";

    /// @brief If the statement contained multiple
    /// imports, they are created as separate instances
    /// and stored here.
    std::vector<Statement_Py> child_statements;

    // Enum naming the different import types (corresponds to regex index)
    enum Py_Regex { IMPORT = 0, FROM_IMPORT, ALL_IMPORT};

};  // class Statement_Py
}  // namespace INCLUDE_GARDENER

#endif // Statement_Py_H
