Python
===========
[TOC]

Python import mechanism
===========

See
 - [https://docs.python.org/3/reference/import.html](https://docs.python.org/3/reference/import.html)
 - [https://docs.python.org/3/reference/simple_stmts.html#import](https://docs.python.org/3/reference/simple_stmts.html#import)
 - [https://www.python.org/dev/peps/pep-0302/](https://www.python.org/dev/peps/pep-0302/)


Examples
-----------
```python
import foo                             # foo imported and bound locally
import foo.bar.baz                     # foo.bar.baz imported, foo bound locally
import foo.bar.baz as fbb              # foo.bar.baz imported and bound as fbb
import foo1 as f1, foo2 as f2          # import foo1 as f1 and foo2 as f2
from foo.bar import baz                # foo.bar.baz imported and bound as baz
from foo import attr                   # foo imported and foo.attr bound as attr
from foo import *                      # import everything from a module
from foo import a,b,c,d                # import a, b, c and d from foo
from foo import aa as a, bb as         # import aa as a and bb as b, both from foo
importlib.import_module(name)
importlib.import_module(name,package=x)
importlib.import_module(name,x)
importlib.__import__()
import a.\
b.\
c
```

The pre-designated 


Considerations
-----------
In Python, it is possible to dynamically import a module which is created during runtime.
Also, it is possible to change the import semantics by importiong the 
`builtins` module and assigning to `buildins.__import__`.
It is also possible to use [import hooks](https://www.python.org/dev/peps/pep-0302/), e.g. for
loading a file via network or extracting a file first.

