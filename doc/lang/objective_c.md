Objective-C
===========
[TOC]

Objective-C include/import mechanism
===========

The mechanism is almost the same than with C/C++, except the 
`#import` statement, which was added (by Objective-C?, GCC?) to ensure that the 
file is imported only once.

Examples
-----------
```objective-c
#import <foo> 

```

Considerations
-----------
It would make sense to realize this together with the C/C++.

