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

#include "gtest/gtest.h"

using namespace INCLUDE_GARDENER;
using namespace std;


class Statement_Detector_Test : public ::testing::Test {};

TEST_F(Statement_Detector_Test, empty_initialization) {
  vector<string> string_list;

  auto d = std::make_shared<Statement_Detector>(string_list);
  EXPECT_EQ(d->get_statements().size(), 0);
  d->wait_for_workers();
}

// vim: filetype=cpp et ts=2 sw=2 sts=2
