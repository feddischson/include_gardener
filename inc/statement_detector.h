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
#ifndef STATEMENT_DETECTOR_H
#define STATEMENT_DETECTOR_H

#include <condition_variable>
#include <iostream>
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include <vector>

#include <boost/regex.hpp>

#include "solver.h"

namespace INCLUDE_GARDENER {

/// @brief Instances of this class are used to search for include-statements.
/// @details
///   This class can be used for any language.
///   One or multiple regular expressions can be provided to search for
///   include/import statements.
///   This class implements also a multi-threading mechanism, where
///   a file which shall be processed is added to a queue (via add_job).
///   The number of worker-threads is defined by n_workers in the ctor.
///
///   @TODO It would be good to ensure that wait_for_workers() is called in any
///   case.
class Statement_Detector {
 public:
  /// @brief Smart pointer for Statement_Detector
  using Ptr = std::shared_ptr<Statement_Detector>;

  /// @brief Initializes all members.
  ///       The member all_work_done is initialized with false.
  /// @param solver
  ///     A language-specific solver which is used to process a detected
  ///     statement.
  // @param n_workers Limits the number of threads. Must be >= 1.
  explicit Statement_Detector(const Solver::Ptr &solver, int n_workers = 1);

  /// @brief Default copy ctor.
  Statement_Detector(const Statement_Detector &other) = delete;

  /// @brief Default assignment operator.
  Statement_Detector &operator=(const Statement_Detector &rhs) = delete;

  /// @brief Default move constructor.
  Statement_Detector(Statement_Detector &&rhs) = delete;

  /// @brief Default move assignment operator.
  Statement_Detector &operator=(Statement_Detector &&rhs) = delete;

  /// @brief Default dtor
  ~Statement_Detector() = default;

  /// @brief Adds a further job (path to file, which is processed).
  void add_job(const std::string &abs_path);

  /// @brief Returns list of statements (as regex)
  std::vector<boost::regex> get_statements() const;

  /// @brief Waits for all workers (blocking).
  void wait_for_workers();

 protected:
  /// @brief Detects include / import statements.
  std::optional<std::pair<std::string, unsigned int>> detect(
      const std::string &line) const;

  /// @brief Walk through a stream and searches for include / import statements.
  void process_stream(std::istream &input, const std::string &input_path);

 private:
  /// @brief Threading method: takes an entry from job_queue to processes it.
  void do_work(int id);

  /// @brief Internal vector of statements.
  const std::vector<boost::regex> statements;

  /// @brief Vector of threads, each calling do_work.
  std::vector<std::thread> workers;

  /// @brief Each entry includes the name and path of the include entry.
  /// @details
  ///        The queue is protected by job_queue_mutex and job_queue_condition.
  std::deque<std::string> job_queue;

  /// @brief Protects job_queue (all worker threads and main-thread).
  std::mutex job_queue_mutex;

  /// @brief Condition variable to sleep thread until jobs are available.
  std::condition_variable job_queue_condition;

  /// @brief Flag to end worker threads.
  bool all_work_done;

  /// @brief Pointer to solver instance.
  Solver::Ptr solver;

};  // class Statement_Detector

}  // namespace INCLUDE_GARDENER

#endif  // STATEMENT_DETECTOR_H

// vim: filetype=cpp et ts=2 sw=2 sts=2
