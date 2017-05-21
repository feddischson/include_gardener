#include "config.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <boost/property_tree/json_parser.hpp>


namespace INCLUDE_GARDENER
{

class MockConfig : public Config

{

   public:

      using Test_Data = std::map< std::string, 
            std::vector< std::tuple<std::string,std::string  > > >;

      void set_cfg( Test_Data & cfg_data )
      {
         this->cfg_data = cfg_data;
      }

      virtual void read_cfg( void )
      {
         namespace pt = boost::property_tree;
         pt::ptree languages;
         if( cfg_data.size() > 0 )
         {
            for( auto const & lang : cfg_data )
            {
               pt::ptree lang_config;

               for( auto const & conf : lang.second )
               {
                  pt::ptree tmp;
                  tmp.put_value( std::get<1>( conf ) );
                  lang_config.push_back( std::make_pair( std::get<0>( conf ), tmp ) );
               }

               languages.add_child(lang.first, lang_config);
               this->languages.push_back( lang.first );
            }
            this->root.add_child( "languages", languages );
         }

         this->check_cfg();
      }

      MockConfig( const std::string & cfg_path )
         : Config( cfg_path)
      {
      }


      static Config::Ptr get_cfg(
            const std::string & cfg_path,
            Test_Data & cfg_data
            )
      {
         MockConfig * c = new MockConfig( cfg_path );
         c->set_cfg( cfg_data );
         c->read_cfg( );
         Ptr p( c );
         return p;
      }

   private:
      Test_Data cfg_data;

};


}


using namespace INCLUDE_GARDENER;

namespace{

class ConfigTest : public ::testing::Test
{

};


TEST_F( ConfigTest, Check_config_path )
{

   MockConfig::Test_Data cfg_data;
   cfg_data["cpp"] = { std::make_tuple( "file_detection",     "aRegex" ),
                       std::make_tuple( "include_detection", "anotherRegex" ) };
   MockConfig::Ptr cfg = MockConfig::get_cfg( "some_path", cfg_data );
   EXPECT_EQ( cfg->get_cfg_path(), "some_path" );
}



TEST_F( ConfigTest, Check_failing_empty_config )
{
   MockConfig::Test_Data cfg_data;
   EXPECT_THROW(
         MockConfig::Ptr cfg = MockConfig::get_cfg( "some_path", cfg_data ),
         std::logic_error 
         );
}


TEST_F( ConfigTest, Check_failing_incomplete_config )
{
   MockConfig::Test_Data cfg_data;
   cfg_data["cpp"] = { std::make_tuple( "file_detection",     "aRegex" ) };
   EXPECT_THROW(
         MockConfig::Ptr cfg = MockConfig::get_cfg( "some_path", cfg_data ),
         std::logic_error 
         );
}

TEST_F( ConfigTest, Check_failing_invalid_config )
{
   MockConfig::Test_Data cfg_data;
   cfg_data["cpp"] = { std::make_tuple( "abcd_detection",     "aRegex" ),
                       std::make_tuple( "include_detection", "anotherRegex" ) };
   EXPECT_THROW(
         MockConfig::Ptr cfg = MockConfig::get_cfg( "some_path", cfg_data ),
         std::logic_error 
         );
}


TEST_F( ConfigTest, Check_failing_empty_config_entry_1 )
{
   MockConfig::Test_Data cfg_data;
   cfg_data["cpp"] = { std::make_tuple( "file_detection",     "" ),
                       std::make_tuple( "include_detection", "anotherRegex" ) };
   EXPECT_THROW(
         MockConfig::Ptr cfg = MockConfig::get_cfg( "some_path", cfg_data ),
         std::logic_error 
         );
}

TEST_F( ConfigTest, Check_failing_empty_config_entry_2 )
{
   MockConfig::Test_Data cfg_data;
   cfg_data["cpp"] = { std::make_tuple( "file_detection",     "aRegex" ),
                       std::make_tuple( "include_detection", "" ) };
   EXPECT_THROW(
         MockConfig::Ptr cfg = MockConfig::get_cfg( "some_path", cfg_data ),
         std::logic_error 
         );
}

TEST_F( ConfigTest, Check_access_to_file_and_include_detection_config )
{

   MockConfig::Test_Data cfg_data;
   cfg_data["cpp"] = { std::make_tuple( "file_detection",     "aRegex" ),
                       std::make_tuple( "include_detection", "anotherRegex" ) };
   MockConfig::Ptr cfg = MockConfig::get_cfg( "some_path", cfg_data );
   EXPECT_EQ( cfg->get_file_detection( "cpp" ), "aRegex" );
   EXPECT_EQ( cfg->get_include_detection( "cpp" ), "anotherRegex" );
}



}


