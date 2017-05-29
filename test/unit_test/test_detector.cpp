#include "detector.h"

#include "gtest/gtest.h"


using namespace INCLUDE_GARDENER;
using namespace std;

namespace{

class DetectorTest : public ::testing::Test
{

};

TEST_F( DetectorTest, test_file_detection_with_empty_args )
{
   string include_regex = "";
   string file_regex    = "";
   vector< string >        exclude_regex;
   vector< unsigned int >  include_group_select;

   Detector::Ptr d = Detector::get_detector( include_regex,
               file_regex,
               exclude_regex,
               include_group_select );


   EXPECT_EQ( d->use_file( ""), true );
   EXPECT_EQ( d->use_file( "a.b"), true );
   EXPECT_EQ( d->use_file( "b.c"), true );
   EXPECT_EQ( d->use_file( "def"), true );
}

TEST_F( DetectorTest, test_c_file_detection_without_exclude )
{
   string include_regex = "";
   string file_regex = "(.*)\\.(c|cc|h|hh|cpp|hpp)";
   vector< string >        exclude_regex;
   vector< unsigned int >  include_group_select;
   Detector::Ptr d = Detector::get_detector( include_regex,
               file_regex,
               exclude_regex,
               include_group_select );


   EXPECT_EQ( d->use_file( "abc.h"         ), true );
   EXPECT_EQ( d->use_file( "def.hh"        ), true );
   EXPECT_EQ( d->use_file( ".c"            ), true );
   EXPECT_EQ( d->use_file( "1234567890.cc" ), true );
   EXPECT_EQ( d->use_file( "x.hpp"         ), true );
   EXPECT_EQ( d->use_file( ".c"            ), true );
   EXPECT_EQ( d->use_file( "y z.cpp"       ), true );
   EXPECT_EQ( d->use_file( "y z.py"        ), false );
   EXPECT_EQ( d->use_file( "x."            ), false );
}

TEST_F( DetectorTest, test_c_file_detection_with_exclude )
{
   string include_regex = "";
   string file_regex = "(.*)\\.(c|cc|h|hh|cpp|hpp)";
   vector< string >        exclude_regex;

   // exclude all *_tmp.c and *-k.c files
   exclude_regex.push_back("(.*)_tmp\\.(c|cc|h|hh|cpp|hpp)");
   exclude_regex.push_back("(.*)-k\\.(c|cc|h|hh|cpp|hpp)");


   vector< unsigned int >  include_group_select;
   Detector::Ptr d = Detector::get_detector( include_regex,
               file_regex,
               exclude_regex,
               include_group_select );


   EXPECT_EQ( d->use_file( "abc.h"         ), true );
   EXPECT_EQ( d->use_file( "def.hh"        ), true );
   EXPECT_EQ( d->use_file( ".c"            ), true );
   EXPECT_EQ( d->use_file( "1234567890.cc" ), true );
   EXPECT_EQ( d->use_file( "x.hpp"         ), true );
   EXPECT_EQ( d->use_file( "t.c"           ), true );
   EXPECT_EQ( d->use_file( "y z.cpp"       ), true );
   EXPECT_EQ( d->use_file( "y z.py"        ), false );
   EXPECT_EQ( d->use_file( "x."            ), false );


   // the same as the positive cases above but with
   // the a "_tmp" and "-k"
   EXPECT_EQ( d->use_file( "abc_tmp.h"           ), false );
   EXPECT_EQ( d->use_file( "def_tmp.hh"          ), false );
   EXPECT_EQ( d->use_file( "_tmp.c"              ), false );
   EXPECT_EQ( d->use_file( "1234567890_tmp.cc"   ), false );
   EXPECT_EQ( d->use_file( "x_tmp.hpp"           ), false );
   EXPECT_EQ( d->use_file( "t_tmp.c"             ), false );
   EXPECT_EQ( d->use_file( "y z_tmp.cpp"         ), false );

   EXPECT_EQ( d->use_file( "abc-k.h"             ), false );
   EXPECT_EQ( d->use_file( "def-k.hh"            ), false );
   EXPECT_EQ( d->use_file( "-k.c"                ), false );
   EXPECT_EQ( d->use_file( "1234567890-k.cc"     ), false );
   EXPECT_EQ( d->use_file( "x-k.hpp"             ), false );
   EXPECT_EQ( d->use_file( "t-k.c"               ), false );
   EXPECT_EQ( d->use_file( "y z-k.cpp"           ), false );

}


TEST_F( DetectorTest, test_include_statements_with_empty_args )
{
   string include_regex = "";
   string file_regex    = "";
   vector< string >        exclude_regex;
   vector< unsigned int >  include_group_select;

   Detector::Ptr d = Detector::get_detector( include_regex,
               file_regex,
               exclude_regex,
               include_group_select );


   EXPECT_EQ( d->is_include_statement( "" ), "" );
   EXPECT_EQ( d->is_include_statement( "abc" ), "" );
}


TEST_F( DetectorTest, test_c_include_statements )
{
   string include_regex = "#\\s*include\\s+[\"|<](\\S+)[\"|>]";
   string file_regex    = "";
   vector< string >        exclude_regex;
   vector< unsigned int >  include_group_select;

   Detector::Ptr d = Detector::get_detector( include_regex,
               file_regex,
               exclude_regex,
               include_group_select );


   EXPECT_EQ( d->is_include_statement( "" ), "" );
   EXPECT_EQ( d->is_include_statement( "abc" ), "" );
   EXPECT_EQ( d->is_include_statement( "#include <iostream>" ), "iostream" );
   EXPECT_EQ( d->is_include_statement( "#include \"iostream\"" ), "iostream" );
}

TEST_F( DetectorTest, test_py_include_statements )
{
   string include_regex = "\\s*(from\\s*)?(\\S+)?\\s*import\\s+(\\S+)\\s*?";
   string file_regex    = "";
   vector< string >        exclude_regex;
   vector< unsigned int >  include_group_select;
   include_group_select.push_back( 2U );
   include_group_select.push_back( 3U );

   Detector::Ptr d = Detector::get_detector( include_regex,
               file_regex,
               exclude_regex,
               include_group_select );


   EXPECT_EQ( d->is_include_statement( "" ), "" );
   EXPECT_EQ( d->is_include_statement( "abc" ), "" );
   EXPECT_EQ( d->is_include_statement( "from abc import def" ), "abc" );
   EXPECT_EQ( d->is_include_statement( "import xyz" ), "xyz" );
   EXPECT_EQ( d->is_include_statement(
            " \t  from \t  abc    \t  import  \t def  \t " ), "abc" );
   EXPECT_EQ( d->is_include_statement( "  \t  import   \t   xyz  \t " ),
         "xyz" );

}


}
