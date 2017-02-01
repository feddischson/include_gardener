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
#include "include_entry.h"

namespace INCLUDE_GARDENER
{

Include_Entry::Include_Entry(
      const std::string & name,
      const std::string & rel_path,
      const std::string & abs_path ) :
   name( name ),
   rel_paths( 1, rel_path ),
   abs_paths( 1, abs_path )
{
}


std::ostream& operator<<( std::ostream& os, const Include_Entry& entry )
{
   os  << "Include_Entry :" << std::endl
       << "     name = " << entry.name;
   return os;
}


}; // namespace SVN_EXTERNALS_DISPOSER

// vim: filetype=cpp et ts=3 sw=3 sts=3

