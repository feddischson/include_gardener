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

#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include <boost/program_options.hpp>

#include "file_detector.h"
#include "solver_c.h"
#include "statement_detector.h"

using std::cerr;
using std::cout;
using std::exception;
using std::make_shared;
using std::ofstream;
using std::string;
using std::vector;

using INCLUDE_GARDENER::File_Detector;
using INCLUDE_GARDENER::Solver;
using INCLUDE_GARDENER::Statement_Detector;

namespace po = boost::program_options;

// Note: use "dot -Tsvg graph.dot > graph.svg" to create svg.

struct Options {
   int n_threads;
   int recursive_limit;
   string language;
   string format;
   string out_file;
   vector<string> process_paths;
   vector<string> exclude;
   // default options
   Options()
       : n_threads{1}, recursive_limit{-1}, language("c"), format("dot") {}
};

Solver::Ptr init_options(int argc, char* argv[], Options* opts);

int main(int argc, char* argv[]) {
   try {
      // global options
      Options opts;

      // initialize and parse options
      auto solver = init_options(argc, argv, &opts);
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
      Statement_Detector s_detector =
          Statement_Detector(solver, opts.n_threads);

      // iterate over all files in detector and add them to job queue
      for (const auto& i : *input_files) {
         s_detector.add_job(i);
      }

      // and wait until all jobs are done!
      s_detector.wait_for_workers();

      // Finally, write the graph somewhere to a file or cout.
      if (opts.out_file.length() > 0) {
         BOOST_LOG_TRIVIAL(info) << "Writing graph to " << opts.out_file;
         auto of = ofstream(opts.out_file);
         solver->write_graph(opts.format, of);
      } else {
         BOOST_LOG_TRIVIAL(info) << "Writing graph to stdout";
         solver->write_graph(opts.format, cout);
      }

   } catch (const exception& e) {
      cerr << e.what() << "\n";
      exit(-1);
   } catch (...) {
      cerr << "Unecpected Error!"
           << "\n";
      exit(-1);
   }

   return 0;
}

Solver::Ptr init_options(int argc, char* argv[], Options* opts) {
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
   if (vm.count("help") > 0) {
      cout << desc << "\n";
      exit(0);
   }

   if (vm.count("version") > 0) {
      cout << "Include Gardener Version " << _GARDENER_VERSION << "\n";
      exit(0);
   }

   // ensure, that at least one process path is provided
   if (vm.count("process-path") == 0) {
      cerr << "No input provided!"
           << "\n"
           << "\n"
           << desc << "\n";
      return nullptr;
   }

   // Sets log level to warning if verbose is not set.
   // This must be done bevore useing any BOOST_LOG_TRIVIAL statement.
   //
   if (vm.count("verbose") == 0) {
      boost::log::core::get()->set_filter(boost::log::trivial::severity >=
                                          boost::log::trivial::warning);
   }

   if (vm.count("exclude") > 0) {
      opts->exclude = vm["exclude"].as<vector<string> >();
   }

   if (vm.count("language") > 0) {
      opts->language = vm["language"].as<string>();
      transform(opts->language.begin(), opts->language.end(),
                opts->language.begin(), ::tolower);
   }

   // extract the format
   // currently, only the dot format is supported
   if (vm.count("format") > 0) {
      opts->format = vm["format"].as<string>();
   }

   if (vm.count("threads") > 0) {
      opts->n_threads = vm["threads"].as<int>();
      if (opts->n_threads == 0) {
         cerr << "Error: Number of threads is set to 0, which is not allowed."
              << "\n"
              << "Please use at least one worker thread."
              << "\n";
         return nullptr;
      }
   }

   if (vm.count("recursive-limit") > 0) {
      opts->recursive_limit = vm["recursive-limit"].as<int>();
   }

   if (!(opts->format.empty() || "dot" == opts->format ||
         "xml" == opts->format || "graphml" == opts->format)) {
      cerr << "Unrecognized format: " << opts->format << "\n"
           << "\n"
           << desc << "\n";
      return nullptr;
   }

   opts->process_paths = vm["process-path"].as<vector<string> >();

   if (vm.count("out-file") > 0) {
      opts->out_file = vm["out-file"].as<string>();
   }

   auto solver = Solver::get_solver(opts->language);
   if (nullptr == solver) {
      cerr << "Error: Unsuported language \"" << opts->language << "\""
           << "\n";
      exit(-1);
   }
   solver->extract_options(vm);

   BOOST_LOG_TRIVIAL(trace) << "n_threads:      " << opts->n_threads;
   BOOST_LOG_TRIVIAL(trace) << "recursive_limit: " << opts->recursive_limit;
   BOOST_LOG_TRIVIAL(trace) << "language:        " << opts->language;
   BOOST_LOG_TRIVIAL(trace) << "format:          " << opts->format;
   BOOST_LOG_TRIVIAL(trace) << "out_file:        " << opts->out_file;
   BOOST_LOG_TRIVIAL(trace) << "process_paths:   ";
   for (const auto& p : opts->process_paths) {
      BOOST_LOG_TRIVIAL(trace) << "    " << p;
   }
   BOOST_LOG_TRIVIAL(trace) << "exclude:         ";
   for (const auto& e : opts->exclude) {
      BOOST_LOG_TRIVIAL(trace) << "    " << e;
   }

   return solver;
}

// vim: filetype=cpp et ts=3 sw=3 sts=3
