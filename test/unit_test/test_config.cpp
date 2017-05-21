#include "config.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <boost/property_tree/json_parser.hpp>


namespace INCLUDE_GARDENER
{

class MockConfig : public Config

{

   public:

      void set_cfg( const std::string & cfg_data )
      {
         this->cfg_data = cfg_data;
      }

      virtual void read_cfg( void )
      {
         std::stringstream ss;
         ss << cfg_data;
         boost::property_tree::read_json(ss, this->root );
         this->check_cfg();
      }

      MockConfig( const std::string & cfg_path )
         : Config( cfg_path)
      {
      }


      static Config::Ptr get_cfg(
            const std::string & cfg_path,
            const std::string & cfg_data
            )
      {
         MockConfig * c = new MockConfig( cfg_path );
         c->set_cfg( cfg_data );
         c->read_cfg( );
         Ptr p( c );
         return p;
      }

   private:
      std::string cfg_data;

};


}


using namespace INCLUDE_GARDENER;

namespace{

class ConfigTest : public ::testing::Test
{

};


TEST_F( ConfigTest, Check_config_path )
{
   std::string cfg_data = " \
   { \
      \"languages\" : { \"cpp\" : {\
         \"file_detection\"    : \"aRegex\",\
         \"include_detection\" : \"anotherRegex\", \
         \"group_select\"      : [0]\
      } } \
   }\
   ";
   MockConfig::Ptr cfg = MockConfig::get_cfg( "some_path", cfg_data );
   EXPECT_EQ( cfg->get_cfg_path(), "some_path" );
}


TEST_F( ConfigTest, Check_failing_empty_config )
{
   std::string cfg_data = "{}";
   EXPECT_THROW(
         MockConfig::Ptr cfg = MockConfig::get_cfg( "some_path", cfg_data ),
         std::logic_error
         );
}


TEST_F( ConfigTest, Check_failing_incomplete_config )
{
   std::string cfg_data = " \
   { \
      \"languages\" : { \"cpp\" : {\
         \"file_detection\"    : \"aRegex\",\
         \"group_select\"      : [0,1]\
      } } \
   }\
   ";
   EXPECT_THROW(
         MockConfig::Ptr cfg = MockConfig::get_cfg( "some_path", cfg_data ),
         std::logic_error
         );
}

TEST_F( ConfigTest, Check_failing_invalid_config )
{
   std::string cfg_data = " \
   { \
      \"languages\" : { \"cpp\" : {\
         \"abcd_detection\"    : \"aRegex\",\
         \"include_detection\" : \"anotherRegex\", \
         \"group_select\"      : [0]\
      } } \
   }\
   ";
   EXPECT_THROW(
         MockConfig::Ptr cfg = MockConfig::get_cfg( "some_path", cfg_data ),
         std::logic_error
         );
}


TEST_F( ConfigTest, Check_failing_empty_config_entry_1 )
{
   std::string cfg_data = " \
   { \
      \"languages\" : { \"cpp\" : {\
         \"file_detection\"    : \"aRegex\",\
         \"include_detection\" : \"\", \
         \"group_select\"      : [0]\
      } } \
   }\
   ";
   EXPECT_THROW(
         MockConfig::Ptr cfg = MockConfig::get_cfg( "some_path", cfg_data ),
         std::logic_error
         );
}

TEST_F( ConfigTest, Check_failing_empty_config_entry_2 )
{
   std::string cfg_data = " \
   { \
      \"languages\" : { \"cpp\" : {\
         \"file_detection\"    : \"\",\
         \"include_detection\" : \"anotherRegex\", \
         \"group_select\"      : [0]\
      } } \
   }\
   ";
   EXPECT_THROW(
         MockConfig::Ptr cfg = MockConfig::get_cfg( "some_path", cfg_data ),
         std::logic_error
         );
}

TEST_F( ConfigTest, Check_failing_empty_config_entry_3 )
{
   std::string cfg_data = " \
   { \
      \"languages\" : { \"cpp\" : {\
         \"file_detection\"    : \"aRegex\",\
         \"include_detection\" : \"anotherRegex\", \
         \"group_select\"      : []\
      } } \
   }\
   ";
   EXPECT_THROW(
         MockConfig::Ptr cfg = MockConfig::get_cfg( "some_path", cfg_data ),
         std::logic_error
         );
}

TEST_F( ConfigTest, Check_access_to_file_and_include_detection_config )
{

   std::string cfg_data = " \
   { \
      \"languages\" : { \"cpp\" : {\
         \"file_detection\"    : \"aRegex\",\
         \"include_detection\" : \"anotherRegex\", \
         \"group_select\"      : [0,2,4]\
      } } \
   }\
   ";
   MockConfig::Ptr cfg = MockConfig::get_cfg( "some_path", cfg_data );
   EXPECT_EQ( cfg->get_file_detection( "cpp" ), "aRegex" );
   EXPECT_EQ( cfg->get_include_detection( "cpp" ), "anotherRegex" );
   EXPECT_EQ( cfg->get_include_group_select( "cpp").size(), 3U );
}


}


