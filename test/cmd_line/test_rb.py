###
#
# Command-line tests written in Python.
# Do
#  > python3 test_rb.py test-reports ../../build/include_gardener ../test_files/rb
# to run the tests
#
# The first argument is used to defines the include_gardener executable,
# the second the test report output directory, and the
# third one defines where test-tree / files are located.
#
###
import sys
import tempfile
import pygraphml as pgml
from test_support.log_select import enable_log
from test_support.gardener_call import GardenerCall
from test_support.main import main
from test_support.helper import compare

try:
    import unittest2 as unittest
except ImportError:
    import unittest


class GardenerRbTestCases(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        cls.gardener = GardenerCall(cls.path_gardener, cls.path_testfiles)

    @classmethod
    def tearDownClass(cls):
        """Nothing to tear down here"""

    def setUp(self):
        """Nothing to setup here"""

    def tearDown(self):
        """Nothing to tear down here"""

    @staticmethod
    def build_reference_graph():
        """ Builds a reference graph which shall be the same
            than the graph of ../test_files.

            A PyGraphml graph is returned.
        """
        g = pgml.Graph()
        f1 = g.add_node("1")
        f2 = g.add_node("2")
        f3 = g.add_node("3")
        f4 = g.add_node("4")
        f5 = g.add_node("5")
        f6 = g.add_node("6")

        f1['key1'] = 'motorcycle_test.rb'
        f2['key1'] = 'lib/motorcycle.rb'
        f3['key1'] = 'motorcycle_2.rb'
        f4['key1'] = 'motorcycle_3.rb'
        f5['key1'] = 'motorcycle_4.rb'
        f6['key1'] = 'lib/sidecar.rb'

        g.add_edge(f1, f2)
        g.add_edge(f1, f3)
        g.add_edge(f1, f4)
        g.add_edge(f1, f5)
        g.add_edge(f4, f6)
        return g

    def test_CallAndCompareOutput(self):
        """ Tests "include_gardener -f xml -l <language> -I <include_files> <test_files>"

        The test expects that the result can be read by graphml
        and that there is at least one node.
        """
        g1 = self.gardener.graphml_gardener_call(['-l', 'ruby', '-I', self.path_testfiles + '/lib'])

        # get a reference graph
        g2 = GardenerRbTestCases.build_reference_graph()

        # both graphs shall be the same
        compare(self, g1, g2)


if __name__ == "__main__":
    main(sys.argv, GardenerRbTestCases)

