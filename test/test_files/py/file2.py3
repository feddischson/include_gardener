# Should show up as sys in graph
import sys

# Should create an edge to pack1/subpack1/__init__.py
from pack1.subpack1 import *

# Should show up only as os, since it is not local
from os import fork

# Multiline import, package import with no effect
import \
    pack2 \
        \
        \
        \
        as  \
            p2

# Multiline import that should show up in graph
import pack1.\
     file1


