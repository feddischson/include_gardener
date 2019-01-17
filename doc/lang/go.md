Go
===========
[TOC]

Go import mechanism
===========

See
 - [https://golang.org/ref/spec](https://golang.org/ref/spec)


Examples
-----------
```go
import "foo.com/bar"
import "foo.com/bar/bar"
import _ "foo.com/bar/bar"       // anonym include
import f1 "foo.com/bar"          // aliasing
import (                         // multi-line group
   "foo"
   "bar"
)
import . "foo/bar"               // everything from foo/bar can be accessed without qualifier
```

Considerations
-----------
Nothing special so far.
