###
#
# Command-line tests written in Python (C-language).
# Do
#  > python3 test_basic.py test-reports ../../build/include_gardener ../test_files/c
# to run the tests
#
# The first argument is used to defines the include_gardener executable,
# the second the test report output directory, and the
# third one defines where test-tree / files are located.
#
###
import sys
from test_support.log_select import enable_log
from test_support.gardener_call import GardenerCall
from test_support.main import main


try:
    import unittest2 as unittest
except ImportError:
    import unittest


class BasicGardenerTestCases(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        cls.gardener = GardenerCall(cls.path_gardener, cls.path_testfiles)

    @classmethod
    def tearDownClass(cls):
        """Nothing to tear down here"""

    def setUp(self):
        """Nothing to tear down here"""

    def tearDown(self):
        """Nothing to tear down here"""

    def test_showsUnrecognisedOption(self):
        """Tests "include_gardener --xyz"

        The test expects an unrecognised option information.
        """
        result_out, result_err = self.gardener.call(["--xyz"])
        self.assertEqual(result_err, "unrecognised option '--xyz'\n")

    def test_printHelp1(self):
        """Tests "include_gardener -h"

        The test expects at least 'Options:' in stdout.
        """
        result_out, result_err = self.gardener.call(["-h"])
        self.assertIn("Options:", result_out)

    def test_printHelp2(self):
        """Tests "include_gardener --help"

        The test expects at least 'Options:' in stdout.
        """
        result_out, result_err = self.gardener.call(["--help"])
        self.assertIn("Options:", result_out)

    def test_Version1(self):
        """Tests "include_gardener -v"

        The test expects at least 'Version' in stdout.
        """
        result_out, result_err = self.gardener.call(["-v"])
        self.assertIn("Version", result_out)

    def test_Version2(self):
        """Tests "include_gardener --version"

        The test expects at least 'Version' in stdout.
        """
        result_out, result_err = self.gardener.call(["--version"])
        self.assertIn("Version", result_out)

    def test_UnsuportedLanguage(self):
        """ Tests if an error message is given if an unsupported language
            is selected
        """
        result_out, result_err = self.gardener.call(["-l", 'x-lang'])
        self.assertIn("Error", result_err)
        self.assertIn("x-lang", result_err)

if __name__ == "__main__":
    main(sys.argv, BasicGardenerTestCases)

