// Include-Gardener
//
// Copyright (C) 2018  Christian Haettich [feddischson]
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
#include "statement_detector.h"

#include <boost/log/trivial.hpp>
#include <fstream>

#include "helper.h"

using namespace std;

namespace INCLUDE_GARDENER {

Statement_Detector::Statement_Detector(const vector<string>& statement_vector,
                                       Solver::Ptr solver, int n_workers)
    : statements(init_regex_vector(statement_vector)),
      workers(n_workers),
      job_queue(),
      job_queue_mutex(),
      job_queue_condition(),
      all_work_done(false),
      solver(solver) {
  for (int i = 0; i < n_workers; ++i) {
    workers[i] = thread(&Statement_Detector::do_work, this, i);
  }
}

void Statement_Detector::add_job(const string& abs_path) {
  unique_lock<mutex> lck(job_queue_mutex);
  job_queue.push_front(abs_path);
  job_queue_condition.notify_all();
}

vector<regex> Statement_Detector::get_statements() { return statements; }

optional<pair<string, unsigned int>> Statement_Detector::detect(
    const string& line) const {
  smatch match;
  for (unsigned int i = 0; i < statements.size(); i++) {
    auto s = statements[i];
    if (regex_search(line, match, s)) {
      if (match.size() == 0) {
        continue;
      } else {
        BOOST_LOG_TRIVIAL(trace) << "Statement matched:" << endl;
        BOOST_LOG_TRIVIAL(trace) << "Size: " << match.size();
        return pair<string, unsigned int>(match[match.size() - 1], i);
      }
    }
  }
  return {};
}

void Statement_Detector::process_stream(istream& input,
                                        const string& input_path) {
  string multi_line;
  string line;
  bool found_multi_line = false;
  int line_cnt = 0;
  optional<pair<string, int>> statement;
  while (getline(input, line)) {
    // handle empty lines
    if (line.size() == 0) {
      // if we previously got a multi-line statement: process it!
      if (found_multi_line == true) {
        statement = detect(multi_line);
        if (!statement) {
          continue;
        } else {
          solver->add_edge(input_path, statement->first, statement->second);
        }
        found_multi_line = false;
      } else {
        // ... if not: move on.
        line_cnt++;
        continue;
      }
    }

    if (line.back() == '\\') {
      line.pop_back();
      multi_line.append(line);
      BOOST_LOG_TRIVIAL(trace) << "found multiline statement: " << line << endl;
      found_multi_line = true;
    } else {
      if (found_multi_line) {
        multi_line.append(line);
        statement = detect(multi_line);
        if (!statement) {
          continue;
        } else {
          solver->add_edge(input_path, statement->first, statement->second);
        }
        found_multi_line = false;
      } else {
        statement = detect(line);
        if (!statement) {
          continue;
        } else {
          solver->add_edge(input_path, statement->first, statement->second);
        }
      }
    }
    line_cnt++;
  }

  // If 'found_multi_line' this is true:
  // This means that the last line contains a backslash-newline,
  // which is invalid.
  if (found_multi_line) {
    // A c-compiler warns here with
    // "backslash-newline at the end of file".
    // Assume, that the previous lines are a complete statement
    // and process this multi-line statement.
    //
    BOOST_LOG_TRIVIAL(warning) << "Missing line at the end of stream" << endl;
    statement = detect(multi_line);
    if (statement) {
      solver->add_edge(input_path, statement->first, statement->second);
    }
    found_multi_line = false;
  }
}

void Statement_Detector::do_work(int id) {
  BOOST_LOG_TRIVIAL(debug) << "Started worker [" << id << "]";

  while (true) {
    unique_lock<mutex> lck(job_queue_mutex);
    job_queue_condition.wait(
        lck, [this]() { return job_queue.size() > 0 || all_work_done; });
    if (all_work_done) {
      BOOST_LOG_TRIVIAL(debug) << "[" << id << "] All work is done";
      return;
    }

    auto entry = job_queue.back();
    job_queue.pop_back();
    lck.unlock();
    job_queue_condition.notify_all();

    BOOST_LOG_TRIVIAL(debug) << "[" << id << "]"
                             << " processing" << entry;
    ifstream ifs(entry, ifstream::in);
    process_stream(ifs, entry);
  }
}

/// @details
///   It first waits until the job-queue is empty. If this is the case,
///   the done flag is set and join() is called on all threads to wait
///   for all of them.
void Statement_Detector::wait_for_workers(void) {
  // wait until the queue is empty
  {
    unique_lock<mutex> lck(job_queue_mutex);
    job_queue_condition.wait(lck, [this]() { return job_queue.size() == 0; });
    lck.unlock();
    job_queue_condition.notify_all();
  }

  // set the done flag
  {
    unique_lock<mutex> lck(job_queue_mutex);
    all_work_done = true;
    lck.unlock();
    job_queue_condition.notify_all();
  }

  for (size_t i = 0; i < workers.size(); i++) {
    workers[i].join();
  }
  BOOST_LOG_TRIVIAL(debug) << "All threads are done";
}

}  // namespace INCLUDE_GARDENER

// vim: filetype=cpp et ts=2 sw=2 sts=2
