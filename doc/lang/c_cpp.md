C/C++
===========
[TOC]

C/C++ preprocessor mechanism
===========

In C/C++, the include mechanism is done by the preprocessor.

See
 - [https://gcc.gnu.org/onlinedocs/cpp/Include-Operation.html#Include-Operation](https://gcc.gnu.org/onlinedocs/cpp/Include-Operation.html#Include-Operation)
 - [https://gcc.gnu.org/onlinedocs/cpp/Search-Path.html](https://gcc.gnu.org/onlinedocs/cpp/Search-Path.html)
 
 
 
 **header files , files , libraries**
 
 **Header Files** : The files that tell the compiler how to call some functionality (without knowing how the functionality actually works) are called header files. They contain the function prototypes. They also contain Data types and constants used with the libraries. We use #include to use these header files in programs. These files end with .h extension.
 
 **Library** : Library is the place where the actual functionality is implemented i.e. they contain function body. Libraries have mainly two categories :

1 . **Static**
2. **Shared or Dynamic**

**Static** : Static libraries contains object code linked with an end user application and then they become the part of the executable. These libraries are specifically used at compile time which means the library should be present in correct location when user wants to compile his/her C or C++ program. In windows they end with .lib extension and with .a for MacOS.

**Shared or Dynamic** : These libraries are only required at run-time i.e, user can compile his/her code without using these libraries. In short these libraries are linked against at compile time to resolve undefined references and then its distributed to the application so that application can load it at run time. For example, when we open our game folders we can find many .dll(dynamic link libraries) files. As these libraries can be shared by multiple programs, they are also called as shared libraries.These files end with .dll or .lib extensions. In windows they end with .dll extension.

**files** : any general script or normal code containing container, or data storing list is called a file. There can be two kinds of files:

1. **binary files**
2. **text files**

**here we  are trying to invoke files( specifically header files(.h) ) using <> and " " operators.**


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

