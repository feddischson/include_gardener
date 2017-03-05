// Include-Gardener
//
// Copyright (C) 2017  Christian Haettich [feddischson]
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
#include <fstream>

#include <boost/graph/graphviz.hpp>
#include <boost/property_map/transform_value_property_map.hpp>
#include <boost/program_options.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/graph/graphml.hpp>

#include "parser.h"

using namespace INCLUDE_GARDENER;
using namespace std;
namespace po = boost::program_options;

static const std::string GARDENER_VERSION = _GARDENER_VERSION;

// Note: use "dot -Tsvg graph.dot > graph.svg" to create svg.

int main( int argc, char* argv[] )
{

   // global instances
   Graph g;
   int no_threads      =  2;
   int recursive_limit = -1;
   //
   // use boost's command line parser
   //
   po::options_description desc("Options");
   desc.add_options()
    ("help,h", "displays this help message and exit")
    ("version,v", "displays version information" )
    ("verbose,V", "sets verbosity")
    ("include-path,I", po::value< vector< string> >()->composing(), "include path")
    ("out-file,o", po::value< string >(), "output file" )
    ("format,f", po::value<string>(), "output format (suported formats: dot, xml/graphml)")
    ("process-path,P", po::value< vector< string> >()->composing(), "path which is processed")
    ("exclude,e", po::value<string>(), "Regular expression to exclude specific files" )
    ("recursive-limit,L",po::value<int>(), "Limits recursive processing (default=-1 = unlimited)")
    ("threads,j", po::value<int>(), "defines number of worker threads (default=2)");
   po::positional_options_description pos;
   pos.add("process-path", -1);

   // the process-path arguments can also be provided als post-arguments
   po::variables_map vm;
   try{
      po::store( po::command_line_parser( argc, argv ).
             options( desc ).positional( pos ).run(), vm );
      po::notify( vm );
   }
   catch( boost::program_options::unknown_option & e )
   {
      cerr << e.what() << endl;
      return -1;
   }

   // print help if required
   if( true == vm.count( "help" ) )
   {
      cout << desc << endl;
      return 1;
   }

   if( true == vm.count( "version" ) )
   {
      cout << "Include Gardener Verion " << GARDENER_VERSION << endl;
      return -1;
   }

   // ensure, that at least one process path is provided
   if ( false == vm.count("process-path") )
   {
      cerr << "No input provided!" << endl << endl << desc << endl;
      return -1;
   }

   // extract the format
   // currently, only the dot format is supported
   std::string format = "";
   if( true == vm.count( "format" ) )
   {
      format = vm["format"].as< string >();
   }

   std::string exclude = "";
   if( true == vm.count( "exclude" ) )
   {
      exclude = vm["exclude"].as< string >();
   }

   if( true == vm.count( "threads" ) )
   {
      no_threads = vm["threads"].as< int >();
      if( no_threads == 0 )
      {
         cerr << "Number of threads is set to 0, which is not allwed." << endl
              << "Please use at least one worker thread."              << endl;
         return -1;

      }
   }

   if( true == vm.count( "recursive-limit" ) )
   {
      recursive_limit = vm["recursive-limit"].as<int>();
   }

   if( !( ""    == format || "dot"     == format ||
          "xml" == format || "graphml" == format ) )
   {
      cerr << "Unrecognized format: " << format << endl << endl << desc << endl;
      return -1;
   }

   // Sets log level to warning if verbose is not set.
   if( false == vm.count( "verbose" ) )
   {
      boost::log::core::get()->set_filter
      (
           boost::log::trivial::severity >= boost::log::trivial::warning
      );
   }

   vector<string> include_paths;
   if( true == vm.count( "include-path" ) )
   {
      include_paths = vm["include-path"].as< vector<string> >();
   }
   auto process_paths = vm["process-path"].as< vector<string> >();

   Include_Path::Ptr i_path( new Include_Path( include_paths ) );
   Parser parser( no_threads, recursive_limit, exclude, i_path, &g );

   // proceed all input paths
   for( auto p : process_paths )
   {
      BOOST_LOG_TRIVIAL(info) << "Processing sources from " << p;
      parser.walk_tree( p, "", ".*" );
   }
   parser.wait_for_workers();


   // prepare the name-map for graphviz output generation
   auto name_map = boost::make_transform_value_property_map(
         []( Include_Entry::Ptr e){ return e->get_name(); },
         get(boost::vertex_bundle, g) );

   if( "" == format || "dot" == format )
   {
      if( vm.count( "out-file" ) == true )
      {
         auto file = vm["out-file"].as< string >();
         BOOST_LOG_TRIVIAL(info) << "Writing graph to " << file;
         ofstream file_stream( file );
         write_graphviz( file_stream, g,
               make_vertex_writer( name_map ),
               make_edge_writer( boost::get(&Edge::line, g) ) );
      }
      else
      {
         write_graphviz( cout, g,
               make_vertex_writer( name_map ),
               make_edge_writer( boost::get(&Edge::line, g) ) );
      }
   }
   else if( "xml" == format || "graphml" == format )
   {
      boost::dynamic_properties dp;
      dp.property( "line", boost::get(&Edge::line, g) );
      dp.property( "name", name_map );
      if( vm.count( "out-file" ) == true )
      {
         auto file = vm["out-file"].as< string >();
         BOOST_LOG_TRIVIAL(info) << "Writing graph to " << file;
         ofstream file_stream( file );
         write_graphml( file_stream, g, dp );
      }
      else
      {
         write_graphml( cout, g, dp );
      }
   }

   return 0;
}


// vim: filetype=cpp et ts=3 sw=3 sts=3

