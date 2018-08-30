C/C++
===========
[TOC]

C/C++ preprocessor mechanism
===========

In C/C++, the include mechanism is done by the preprocessor.

See
 - [https://gcc.gnu.org/onlinedocs/cpp/Include-Operation.html#Include-Operation](https://gcc.gnu.org/onlinedocs/cpp/Include-Operation.html#Include-Operation)
 - [https://gcc.gnu.org/onlinedocs/cpp/Search-Path.html](https://gcc.gnu.org/onlinedocs/cpp/Search-Path.html)

Examples
-----------

```C
#include <file_a.h>

#include "file_b.h"

#include "file_c.y"

#include "../file_d.h"

/* also this seems to be possible: */
#\
inc\
lude \
<std\
io.h\
>

```

The following ways are possible to lookup for a include file
 - In case of `""` usage, the files are first searched in the same directory (relative path), then in 
   pre-designated search directories 
 - In case or `<>` usage, the files are only searched in the pre-designated search directories



Considerations
---------------
In some cases, include statements are inbetween `#if`, `#else`, `#endif`, `#ifdef`, `#ifndef` statements.
This means, that depending on the surrounding preprocessor situation, a file is included or not.
This makes the analysis more difficult. A possible solution might be to use a GCC or CLANG compiler
to do all the work, e.g. `gcc -M` or `clang++ -H -fsyntax-only`.
This would introduce a compiler runtime dependency which is not nice.

