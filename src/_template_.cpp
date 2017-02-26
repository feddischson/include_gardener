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
#include "_template_.h"

namespace INCLUDE_GARDENER
{


std::ostream& operator<<( std::ostream& os, const _Template_& tpl )
{
   os  << "_Template_ ("  << reinterpret_cast< const void* >( &tpl ) << "): "
       << "TODO=" << "Add more content of this instance here @ "
       << __FILE__ << ":" << __LINE__;
   return os;
}


} // namespace INCLUDE_GARDENER

// vim: filetype=cpp et ts=3 sw=3 sts=3

