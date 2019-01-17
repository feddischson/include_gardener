Include Gardener
=================
[TOC]

Include Gardener
=================

[![Build Status](https://travis-ci.org/feddischson/include_gardener.svg?branch=master)](https://travis-ci.org/feddischson/include_gardener)


Introduction
-------------
This is a small C++ based commandline-tool which analyzes C/C++ code
and creates a graph or include tree.
The result can then further analyzed by other tools.

The following output formats are supported at the moment:
 - dot (Graphviz): see http://www.graphviz.org/Documentation.php
 - XML / Graphml: see http://graphml.graphdrawing.org/

This tool
 - is used via command-line
 - supports recursive file search
 - is able to process the files via multiple threads
 - written in C++
 - tested on Linux (GCC, clang)



Build Description
-----------------

Build dependecies
 - cmake

Runtime Dependencies:
 - boost

Test Dependencies:
 - Python 3.x
 - PyUnit
 - PyGraphviz
 - PyGraphml

```
mkdir build
cd build
cmake ..
make
make doc
make install
```


Usage
-------
```
# analyzes recursively all files in path/to/files,
# the result is written to stdout
./include_gardener  -P path/to/files

# multiple P options can be provided, for example
./include_gardener  -P path/to/files1 -P path/to/files2

# analyzes recursively all files in the curren directory,
# the include path is set to ./inc and the result is
# written to the file graph.dot:
./include_gardener  -P ./ -I ./inc -o graph.dot

# the result can then be further converted to a scalable vector graphics file.
dot -Tsvg graph.dot > graph.svg
```

Limitations
============

C/C++/Obj-C
------------
This tool does not respect any `#if #else #endif` or other pre-processor statements than `#include`.


Versions
============

1.0.0
------
 - Drops configuration file
 - Re-organizes architecture to allow further languages in fugure
 - Improves C/C++/Obj-C include statement detection

0.2.0
------
 - Adds configuration file
 - Small stability and performance improvements

0.1.0
------
 - Initial Version


Contribution
============

If you find a bug, unexpected behaviour or if you have a interesting feature
in your mind which fits to this tool, please add an issue / feature request.

Pull requests are always welcome.

License
============


This program is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public
License as published by the Free Software Foundation;
either version 3 of the License, or (at your option)
any later version.

This program is distributed in the hope that it will
be useful, but WITHOUT ANY WARRANTY; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License
for more details.

You should have received a copy of the GNU General
Public License along with this program; if not, see
<http://www.gnu.org/licenses/>.

