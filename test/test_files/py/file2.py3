# Should show up as os, since it is not local
from os import fork

# Should show up as sys in graph
import sys

# Should create an edge to pack1/subpack1/__init__.py
from pack1.subpack1 import *

# Multiline import, package import with no effect
import \
    pack2 \
        \
        \
        \
        as  \
            p2

# Multiline import that should show up in graph
import pack1. \
     file1
