

#ifndef Statement_Py_H
#define Statement_Py_H

#include <boost/filesystem.hpp>


namespace INCLUDE_GARDENER {
/// @brief Keeps information related to one statement
/// and knows how to modify it.
///
///
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


   virtual bool contained_multiple_imports();


   virtual std::string get_source_path(){ return source_path; }
   virtual std::string get_modified_statement(){ return modified_statement; }
   virtual std::string get_possible_path(){ return possible_path; }
   virtual unsigned int get_regex_idx(){ return regex_idx; }
   virtual unsigned int get_line_number(){ return line_number; }
   virtual bool get_is_relative() { return is_relative; }
   virtual bool get_had_star() { return had_star; }
   virtual unsigned int get_directories_above() { return directories_above; }
   virtual std::string extract_dummy_node_name(const std::string &statement);

   virtual std::vector<Statement_Py> get_child_statements();

 protected:



   virtual std::string dots_to_system_slash(const std::string &statement);


   virtual bool is_relative_import(const std::string &statement);

   virtual void erase_all_quotation_marks(std::string &statement);
   virtual void add_relative_dot_in_front(std::string &statement);
   virtual void add_package_name_in_front(std::string &statement,
                                          const std::string &package_name);

   virtual std::string get_all_before_import(const std::string &statement);
   virtual std::string get_all_after_import(const std::string &statement);


   virtual bool contains_comma(const std::string &statement);
   virtual bool contains_star(const std::string &statement);

   virtual void split_into_multiple_statements(const std::string &src_path,
                                           const std::string &statement,
                                           unsigned int idx,
                                           unsigned int line_no);

   /// @brief Counts how many dots a string is prepended with.
   virtual unsigned int how_many_directories_above(const std::string &statement);

   virtual std::vector<std::string> split_by_comma(const std::string &statement);

   virtual void remove_as_statements(std::string &statement);

   virtual void remove_prepended_dots(std::string &statement);

   virtual void remove_whitespace(std::string &statement);

 private:
    /// @brief Regex to match the first dot(s) in a string
    const std::string dot_regex = "^[ \\t]*([.]+).*$";

    /// @brief Regex to match everything past the first dot(s) in a string
    const std::string past_dot_regex = "^[ \\t]*[.]+(.*)$";

    const std::string source_path;
    const std::string original_statement;
    const unsigned int line_number;
    unsigned int regex_idx;

    std::string modified_statement = "";
    std::string possible_path = "";
    bool had_star = false;
    bool is_relative = false;
    bool contains_multiple_imports = false;
    unsigned int directories_above = 0;

    std::vector<Statement_Py> child_statements;

    // Enum naming the different import types (corresponds to regex index)
    enum Py_Regex { IMPORT = 0, FROM_IMPORT, ALL_IMPORT};

};  // class Statement_Py
}  // namespace INCLUDE_GARDENER

#endif // Statement_Py_H
