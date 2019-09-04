##Test description

The files in this directory can be used to test the behaviour of different Python versions when importing packages. It should demonstrate for both Python 2 and 3:

* *\_\_init\_\_.py* is always ran when importing a package.
* Unless a module is explicitly imported in a package's *\_\_init\_\_*.py, no modules are imported when importing on the form ```import name_of_package```.
* The \_\_all\_\_ array is only paid attention to when importing on the form ```from name_of_package import *```.

### Test files (run these)
**import_test_main.py**
Tests imports on the form ```import name_of_package```.
Should only import **mod01**.

**import_all_test_main.py**
Tests imports on the form ```from name_of_package import *```.
Should only import **module01** and **mod01**.

### Test packages
**no_import_init_pack**
Pack does not contain any explicit imports. No modules should be imported.

**import_in_init_pack**
Pack contains a single explicit import. Only **mod01** should be imported from this pack.

**all_array_in_init_pack**
Pack contains a single \_\_all\_\_-array import. **module01** should only be imported if importing on the form ```from name_of_package import *```.