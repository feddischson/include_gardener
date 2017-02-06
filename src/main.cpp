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

#include <boost/graph/graphviz.hpp>
#include <boost/property_map/transform_value_property_map.hpp>
#include <boost/program_options.hpp>
#include <fstream>

#include "parser.h"

using namespace INCLUDE_GARDENER;
using namespace std;
namespace po = boost::program_options;


// Note: use "dot -Tsvg graph.dot > graph.svg" to create svg.

int main( int argc, char* argv[] )
{
   // global instances
   Parser parser;
   Graph g;
   Include_Entry::Map i_map;


   //
   // use boost's command line parser
   //
   po::options_description desc("Options");
   desc.add_options()
    ("help,h", "displays this help message and exit")
    ("input-path,I", po::value< vector< string> >()->composing(), "input path")
    ("out-file,o", po::value< string >(), "output file" )
    ("format,f", po::value<string>(), "output format (suported formats: dot)");
   po::positional_options_description pos;
   pos.add("input-path", -1);

   // the input-path arguments can also be provided als post-arguments
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

   // ensure, that at least one input path is provided
   if ( false == vm.count("input-path") )
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


   if( !( "" == format || "dot" == format ) )
   {
      cerr << "Unrecognized format: " << format << endl << endl << desc << endl;
      return -1;
   }

   // proceed all input paths
   auto input_paths = vm["input-path"].as< vector<string> >();
   for( auto p : input_paths )
   {
      cout << "Processing " << p << endl;
      parser.walk_tree( p, "", ".*\\.[h|cpp]", i_map, g );
   }


   // prepare the name-map for graphviz output generation
   auto name_map = boost::make_transform_value_property_map(
         []( Include_Entry::Ptr e){ return e->get_name(); },
         get(boost::vertex_bundle, g) );

   if( "" == format || "dot" == format )
   {
      if( vm.count( "out-file" ) == true )
      {
         auto file = vm["out-file"].as< string >();
         cout << "writing graph to " << file << endl;
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

   return 0;
}


// vim: filetype=cpp et ts=3 sw=3 sts=3

