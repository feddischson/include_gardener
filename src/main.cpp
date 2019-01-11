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
#include <fstream>

#include <boost/graph/graphml.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include <boost/program_options.hpp>
#include <boost/property_map/transform_value_property_map.hpp>

#include "file_detector.h"
#include "solver_c.h"
#include "statement_detector.h"

using namespace INCLUDE_GARDENER;
using namespace std;
namespace po = boost::program_options;

static const string GARDENER_VERSION = _GARDENER_VERSION;

Graph g;

Solver::Ptr init_options(int argc, char* argv[]);

// Note: use "dot -Tsvg graph.dot > graph.svg" to create svg.

struct {
   int n_threads;
   int recursive_limit;
   string language;
   string format;
   vector<string> process_paths;
   vector<string> exclude;
   string out_file;
} opts;

int main(int argc, char* argv[]) {
   // initialize and parse options
   auto solver = init_options(argc, argv);
   if (solver == nullptr) {
      return -1;
   }

   // Create a file detector ...
   auto input_files =
       make_shared<File_Detector>(solver->get_file_regex(), opts.exclude,
                                  opts.process_paths, opts.recursive_limit);

   // ... and get all files.
   input_files->get(solver);

   // Then, create a statement detector ...
   Statement_Detector s_detector = Statement_Detector(solver, opts.n_threads);

   // iterate over all files in detector and add them to job queue
   for (auto i = input_files->begin(); i != input_files->end(); ++i) {
      s_detector.add_job(*i);
   }

   // and wait until all jobs are done!
   s_detector.wait_for_workers();

   // prepare the name-map for graphviz output generation
   auto name_map = boost::make_transform_value_property_map(
       [](Vertex::Ptr v) { return v->get_name(); },
       get(boost::vertex_bundle, g));

   if ("dot" == opts.format) {
      if (opts.out_file.length() > 0) {
         BOOST_LOG_TRIVIAL(info) << "Writing graph to " << opts.out_file;
         ofstream file_stream(opts.out_file);
         write_graphviz(file_stream, g, make_vertex_writer(name_map),
                        make_edge_writer(boost::get(&Edge::line, g)));
      } else {
         write_graphviz(cout, g, make_vertex_writer(name_map),
                        make_edge_writer(boost::get(&Edge::line, g)));
      }
   } else if ("xml" == opts.format || "graphml" == opts.format) {
      boost::dynamic_properties dp;
      dp.property("line", boost::get(&Edge::line, g));
      dp.property("name", name_map);
      if (opts.out_file.length() > 0) {
         BOOST_LOG_TRIVIAL(info) << "Writing graph to " << opts.out_file;
         ofstream file_stream(opts.out_file);
         write_graphml(file_stream, g, dp);
      } else {
         write_graphml(cout, g, dp);
      }
   }
   return 0;
}

Solver::Ptr init_options(int argc, char* argv[]) {
   // default options
   opts.n_threads = 1;
   opts.recursive_limit = -1;
   opts.language = "c";
   opts.format = "dot";
   opts.out_file = "";

   //
   // use boost's command line parser
   //
   po::options_description desc("Options");
   desc.add_options()("help,h", "displays this help message and exit")(
       "version,v", "displays version information")(
       "verbose,V", "sets verbosity")("out-file,o", po::value<string>(),
                                      "output file")(
       "format,f", po::value<string>(),
       "output format (suported formats: dot, xml/graphml)")(
       "process-path,P", po::value<vector<string> >()->composing(),
       "path which is processed")(
       "exclude,e", po::value<vector<string> >()->composing(),
       "regular expressions to exclude specific files")(
       "recursive-limit,L", po::value<int>(),
       "limits recursive processing (default=-1 = unlimited)")(
       "threads,j", po::value<int>(),
       "defines number of worker threads (default=2)")(
       "language,l", po::value<string>(), "selects the language (default=c)");
   Solver::add_options(&desc);

   po::positional_options_description pos;
   pos.add("process-path", -1);

   // the process-path arguments can also be provided als post-arguments
   po::variables_map vm;
   try {
      po::store(po::command_line_parser(argc, argv)
                    .options(desc)
                    .positional(pos)
                    .run(),
                vm);
      po::notify(vm);
   } catch (boost::program_options::unknown_option& e) {
      cerr << e.what() << "\n";
      return nullptr;
   }

   // print help if required
   if (true == vm.count("help")) {
      cout << desc << "\n";
      exit(0);
   }

   if (true == vm.count("version")) {
      cout << "Include Gardener Version " << GARDENER_VERSION << "\n";
      exit(0);
   }

   // ensure, that at least one process path is provided
   if (false == vm.count("process-path")) {
      cerr << "No input provided!"
           << "\n"
           << "\n"
           << desc << "\n";
      return nullptr;
   }

   // Sets log level to warning if verbose is not set.
   // This must be done bevore useing any BOOST_LOG_TRIVIAL statement.
   //
   if (false == vm.count("verbose")) {
      boost::log::core::get()->set_filter(boost::log::trivial::severity >=
                                          boost::log::trivial::warning);
   }

   if (true == vm.count("exclude")) {
      opts.exclude = vm["exclude"].as<vector<string> >();
   }

   if (true == vm.count("language")) {
      opts.language = vm["language"].as<string>();
      transform(opts.language.begin(), opts.language.end(),
                opts.language.begin(), ::tolower);
   }

   // extract the format
   // currently, only the dot format is supported
   if (true == vm.count("format")) {
      opts.format = vm["format"].as<string>();
   }

   if (true == vm.count("threads")) {
      opts.n_threads = vm["threads"].as<int>();
      if (opts.n_threads == 0) {
         cerr << "Error: Number of threads is set to 0, which is not allowed."
              << "\n"
              << "Please use at least one worker thread."
              << "\n";
         return nullptr;
      }
   }

   if (true == vm.count("recursive-limit")) {
      opts.recursive_limit = vm["recursive-limit"].as<int>();
   }

   if (!("" == opts.format || "dot" == opts.format || "xml" == opts.format ||
         "graphml" == opts.format)) {
      cerr << "Unrecognized format: " << opts.format << "\n"
           << "\n"
           << desc << "\n";
      return nullptr;
   }

   opts.process_paths = vm["process-path"].as<vector<string> >();

   if (true == vm.count("out-file")) {
      opts.out_file = vm["out-file"].as<string>();
   }

   auto solver = Solver::get_solver(opts.language, &g);
   if (nullptr == solver) {
      cerr << "Error: Unsuported language \""<<opts.language << "\"" << "\n";
      exit(-1);
   }
   solver->extract_options(vm);

   BOOST_LOG_TRIVIAL(trace) << "n_threads:      " << opts.n_threads;
   BOOST_LOG_TRIVIAL(trace) << "recursive_limit: " << opts.recursive_limit;
   BOOST_LOG_TRIVIAL(trace) << "language:        " << opts.language;
   BOOST_LOG_TRIVIAL(trace) << "format:          " << opts.format;
   BOOST_LOG_TRIVIAL(trace) << "out_file:        " << opts.out_file;
   BOOST_LOG_TRIVIAL(trace) << "process_paths:   ";
   for (auto p : opts.process_paths) {
      BOOST_LOG_TRIVIAL(trace) << "    " << p;
   }
   BOOST_LOG_TRIVIAL(trace) << "exclude:         ";
   for (auto e : opts.exclude) {
      BOOST_LOG_TRIVIAL(trace) << "    " << e;
   }

   return solver;
}

// vim: filetype=cpp et ts=3 sw=3 sts=3
