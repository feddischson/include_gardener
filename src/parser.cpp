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
#include "parser.h"

namespace INCLUDE_GARDENER
{


std::ostream& operator<<( std::ostream& os, const Parser& parser )
{
   os  << "Parser ("  << reinterpret_cast< const void* >( &parser ) << "): "
       << "TODO=" << "Add more content of this instance here @ "
       << __FILE__ << ":" << __LINE__;
   return os;
}


}; // namespace SVN_EXTERNALS_DISPOSER

// vim: filetype=cpp et ts=3 sw=3 sts=3

