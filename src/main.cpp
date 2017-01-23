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

using namespace INCLUDE_GARDENER;

int main( int argc, char* argv[] )
{
   Parser p;

   //  ./include_gardener ./ ".*\.[h|cpp]"
   if( argc == 3 )
   {
      p.walk_tree( argv[1], argv[2] );
   }
   return 0;
}


// vim: filetype=cpp et ts=3 sw=3 sts=3

