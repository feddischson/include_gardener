###
#
# Command-line tests written in Python.
# Do
#  > python3 test.py test-reports ../../build/include_gardener ../test_files/c
# to run the tests
#
# The first argument is used to defines the include_gardener executable,
# the second the test report output directory, and the
# third one defines where test-tree / files are located.
#
###

try:
    import unittest2 as unittest
except ImportError:
    import unittest

import os
import sys
import tempfile
import pygraphviz as pgv
import pygraphml as pgml

from subprocess import Popen, PIPE
from os.path import abspath, join


class GardenerTestCases(unittest.TestCase):

    # default paths
    M_PATH = "."
    T_PATH = "../test_files/"
    G_PATH = "../../build/include_gardener"
    LANGUAGE = "c"

    def setUp(self):
        """Nothing to setup here"""

    def tearDown(self):
        """Nothing to tear down here"""

    def build_reference_graph(self):
        """ Builds a reference graph which shall be the same
            than the graph of ../test_files.

            A PyGraphml graph is returned.
        """
        parser = pgml.GraphMLParser()
        return parser.parse(os.path.join(self.M_PATH, self.LANGUAGE + '_graph.dot'))

    def compare(self, G1, G2):
        """ Compares two PyGraphml graphs by using PyUnittest's
            assert methods.
        """
        nodes1 = G1.nodes()
        nodes2 = G2.nodes()
        self.assertEqual(len(nodes1), len(nodes2))
        for n1 in nodes1:
            found = False

            src1 = n1['key1']
            src2 = ""
            dst1 = []
            dst2 = []

            # get all children
            for c1 in G1.children(n1):
                dst1.append(c1['key1'])

            # search for the src in the second list
            for n2 in nodes2:
                src2 = n2['key1']

                if src1 == src2:
                    found = True
                    # get all children
                    for c2 in G2.children(n2):
                        dst2.append(c2['key1'])
                    break

            self.assertTrue(found)
            self.assertEqual(src1, src2)
            self.assertCountEqual(dst1, dst2)

    def gardener_call(self, options, subpath=""):
        
        p_args = [self.G_PATH, os.path.join(self.T_PATH, subpath)] + options
        # print( ' '.join( p_args ) )
        pipe = Popen(p_args, stdout=PIPE)
        result_str = pipe.communicate()[0]
        return result_str

    def dot_gardener_call(self, options, subpath=""):
        """
        Runs the include_gardener with the dot option,
        extracts the result and returns the graph.
        """
        # we don't use '-f dot' because dot is the default!
        result_str = self.gardener_call([] + options, subpath).decode("utf-8")
        # print("result:")
        # print(result_str)
        G = pgv.AGraph(result_str)
        return G

    def graphml_gardener_call(self, options, subpath=""):
        """
        Runs the include_gardener with the xml option,
        extracts the result and returns the graph.
        """
        result_str = self.gardener_call(['-f', 'xml'] + options, subpath)
        if len(result_str) == 0:
            return None
        # print("result:")
        # print(result_str.decode('utf-8'))
        temp = tempfile.NamedTemporaryFile()
        temp.write(result_str)
        temp.flush()
        parser = pgml.GraphMLParser()

        # get the result from the system call:
        return parser.parse(temp.name)

    def test_CallAndCompareOutput(self):
        """ Tests "include_gardener -f xml -l <language> -I <include_files> <test_files>"

        The test expects that the result can be read by graphml
        and that there is at least one node.
        """
        g1 = self.graphml_gardener_call(['-l', self.LANGUAGE, '-I', self.I_PATH])

        # get a reference graph
        g2 = self.build_reference_graph()

        # both graphs shall be the same
        self.compare(g1, g2)


if __name__ == "__main__":
    print(sys.argv)
    if len(sys.argv) > 4:
        GardenerTestCases.T_PATH = abspath(sys.argv.pop())
        GardenerTestCases.I_PATH = abspath(sys.argv.pop())
        GardenerTestCases.G_PATH = abspath(sys.argv.pop())
        GardenerTestCases.M_PATH = abspath(sys.argv.pop())
        GardenerTestCases.LANGUAGE = sys.argv.pop()

    import xmlrunner
    out_dir = abspath(sys.argv.pop())
    unittest.main(testRunner=xmlrunner.XMLTestRunner(output=out_dir))
