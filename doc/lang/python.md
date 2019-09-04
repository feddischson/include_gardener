Python
===========
[TOC]

Python import mechanism
===========

See
 - [https://docs.python.org/3/reference/import.html](https://docs.python.org/3/reference/import.html)
 - [https://docs.python.org/3/reference/simple_stmts.html#import](https://docs.python.org/3/reference/simple_stmts.html#import)
 - [https://www.python.org/dev/peps/pep-0302/](https://www.python.org/dev/peps/pep-0302/)
 - [https://www.python.org/dev/peps/pep-0328/](https://www.python.org/dev/peps/pep-0328/)

Project Composition
-----------
A Python project typically consists of modules and packages.


**Module:** A file that contains Python definitions and statements. Usually has the file extension *.py*.
**Package:** A directory that contains the file \__init__.py. The file can be empty.

If a module is present inside the directory of a package, the module belongs to that package and all Python-packages on the path to the module.

Absolute and Relative imports
-----------
**Absolute imports**

```
import foo.bar                    # Import bar from the package foo
from foo import bar               # Same as above.
from foo.bar import baz           # Import baz from the package foo.bar or from module bar import baz in package foo
import foo.bar.baz                # Import baz from foo.bar
```

Absolute import resolves the full path from the project’s root folder to the imported module. Package names are separated by dots instead of slashes.

**Relative imports**
```
from . import bar                  # Import bar from the current directory
from .bar import baz               # From bar in the current directory import baz
from  .. import hello              # Import hello from directory above
from ..qux import quux             # From qux in the directory above import quux
from ... import corge              # From two directories above import corge
```
Relative imports are resolved relative to the importing files. A single prepended dot means the current directory, two dots means a directory above, three dots means two directories above and so on.

Package Importing Behaviour
-----------
Packages can be imported in at least two ways:
```
import package_name                # imports explicit imports in __init__.py
from package_name import *         # imports explicit imports in __init__.py, modules named in __all__
```
Importing a package runs the \__init__.py-file for the imported package in both Python 2 and 3. This means that any explicit imports in the \__init__.py file are subsequently imported. \__all__ is only accessed when importing on the second form.

**Note:** If \__init__.py does not explicitly import anything, then nothing will actually be imported when using the two above forms.

Importing Style Guide
-----------
Imports should be located at the top of a file. It is also customary that import statements are placed on separate lines in Python files.

```
Yes: import os
     import sys

No:  import sys, os
```
If multiple items are imported from a single location it is okay to keep them on a single line:
```
from subprocess import Popen, PIPE
```

Absolute imports are preferable, but relative imports are allowed in complex package layouts.

Note that implicit relative imports should not be used: 
```
import foo
```
This is because Python 2 would look in the caller's directory, but Python 3 would not do this. Python 3 would instead check sys.path in search of foo.

Importing * (all) functions differently on modules and packages:

```
from foo import *      # 1: Imports all from module foo
                       # 2: foo is package and does not have __all__ defined in __init__.py: nothing
                       # 3: foo is package and has __all__ defined in __init__.py: imports modules (by name) defined in __all__
```
The \__init__.py file must define [\__all__] with names of modules from the package that should be imported when import * is called. Otherwise nothing will happen with the package import. Note that importing all from a module or package is prone to namespace collisions.

Source: [[PEP8]](https://www.python.org/dev/peps/pep-0008/#imports) 

Valid Python Identifiers
-----------
There are rules to what Python identifiers are valid and this rule also applies to filenames and imports. A valid identifier can only begin with a uppercase/lowercase letter or an underscore, after that any combination of uppercase/lowercase letters, numbers and underscores are allowed. [[Python 2 Doc]](https://docs.python.org/2/reference/lexical_analysis.html#identifiers)

Note that Python3 supports Unicode letter characters in identifiers. [[Python 3 Doc]](https://docs.python.org/3/reference/lexical_analysis.html#identifiers)

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
import a.\                             # multiline import
b.\
c
```

Considerations
-----------
In Python, it is possible to dynamically import a module which is created during runtime. Also, it is possible to change the import semantics by importiong the `builtins` module and assigning to `builtins.__import__`.

It is also possible to use [import hooks](https://www.python.org/dev/peps/pep-0302/), e.g. for loading a file via network or extracting a file first.

