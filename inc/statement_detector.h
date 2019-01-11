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
#ifndef STATEMENT_DETECTOR_H
#define STATEMENT_DETECTOR_H

#include <condition_variable>
#include <iostream>
#include <mutex>
#include <optional>
#include <regex>
#include <string>
#include <thread>
#include <vector>

#include "solver.h"

namespace INCLUDE_GARDENER {

/// @brief Statement_Detector class
class Statement_Detector {
 public:
  /// @brief Smart pointer for Statement_Detector
  using Ptr = std::shared_ptr<Statement_Detector>;

  /// @brief Default ctor.
  explicit Statement_Detector(Solver::Ptr solver, int n_workers = 1);

  /// @brief  Default copy ctor.
  Statement_Detector(const Statement_Detector &other) = default;

  /// @brief  Default assignment operator.
  Statement_Detector &operator=(const Statement_Detector &rhs) = default;

  /// @brief  Default move constructor.
  Statement_Detector(Statement_Detector &&rhs) = default;

  /// @brief  Default move assignment operator.
  Statement_Detector &operator=(Statement_Detector &&rhs) = default;

  /// @brief Default dtor
  ~Statement_Detector() = default;

  /// @brief Adds a further job (path to file, which is processed).
  void add_job(const std::string &abs_path);

  /// @brief Returns list of statements (as regex)
  std::vector<std::regex> get_statements();

  /// @brief Waits for all workers (blocking).
  void wait_for_workers(void);

 protected:
  /// @brief Detects include / import statements.
  std::optional<std::pair<std::string, unsigned int>> detect(
      const std::string &line) const;

  /// @brief Walk through a stream and searches for include / import statements.
  void process_stream(std::istream &input, const std::string &p);

 private:
  /// @brief Threading method: takes an entry from job_queue to processes it.
  void do_work(int id);

  /// @brief Internal vector of statements.
  std::vector<std::regex> statements;

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