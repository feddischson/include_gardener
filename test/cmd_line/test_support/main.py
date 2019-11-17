from os.path import abspath
import logging
try:
    import unittest2 as unittest
except ImportError:
    import unittest


def main(argv, test_class):
    if len(argv) > 4:
        loglevel = argv.pop()
    else:
        loglevel = "WARNING"

    if len(argv) > 3:
        test_class.path_testfiles = abspath(argv.pop())
        test_class.path_gardener = abspath(argv.pop())

    if len(argv) is not 2:
        print("Invalid arguments")
        exit(-1)

    numeric_level = getattr(logging, loglevel.upper(), None)
    if not isinstance(numeric_level, int):
        raise ValueError('Invalid log level: %s' % loglevel)

    logging.basicConfig(format='%(asctime)s %(message)s', level=numeric_level)

    import xmlrunner
    out_dir = abspath(argv.pop())
    unittest.main(testRunner=xmlrunner.XMLTestRunner(output=out_dir))

