###
#
# Command-line tests written in Python (Python language).
# Do
#  > python3 test_py.py test-reports ../../build/include_gardener ../test_files/py/graph_test_files
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
from os.path import abspath


class GardenerTestCases(unittest.TestCase):

    # default paths
    G_PATH = "../../build/include_gardener"
    T_PATH = "../test_files/py/graph_test_files/"

    def setUp(self):
        """Nothing to setup here"""

    def tearDown(self):
        """Nothing to tear down here"""

    def build_reference_graph(self):
        """ Builds a reference graph which shall be the same
            than the graph of ../test_files.

            A PyGraphml graph is returned.
        """

        """ List of files by doing:
	 find -iname '*.py' -or -iname '*.pyw' -or -iname '*.py3'

        Use as basis for constructing the graph
	./py/pack3/__init__.py
	./py/pack3/file1.py
	./py/file3.pyw
	./py/pack2/__init__.py
	./py/pack2/file3.py
	./py/pack2/file2.py
	./py/pack2/file1.py
	./py/file2.py3
	./py/file1.py
	./py/pack1/__init__.py
	./py/pack1/file3.py
	./py/pack1/file2.py
	./py/pack1/file1.py
	./py/pack1/subpack1/__init__.py
	./py/pack1/subpack1/file3.py
	./py/pack1/subpack1/file2.py
	./py/pack1/subpack1/file1.py


        """

        g = pgml.Graph()

        f1 = g.add_node("1")
        f2 = g.add_node("2")
        f3 = g.add_node("3")

        p1_f1 = g.add_node("4")
        p1_f2 = g.add_node("5")
        p1_f3 = g.add_node("6")
        p1_init = g.add_node("7")

        p1_s1_f1 = g.add_node("8")
        p1_s1_f2 = g.add_node("9")
        p1_s1_f3 = g.add_node("10")
        p1_s1_init = g.add_node("11")

        p2_f1 = g.add_node("12")
        p2_f2 = g.add_node("13")
        p2_f3 = g.add_node("14")
        p2_init = g.add_node("15")

        p3_f1 = g.add_node("16")
        p3_init = g.add_node("17")

        bogusfile = g.add_node("18")
        filecmp = g.add_node("19")
        os = g.add_node("20")
        pickle = g.add_node("21")
        sys = g.add_node("22")

        f1['key1'] = 'file1.py'
        f2['key1'] = 'file2.py3'
        f3['key1'] = 'file3.pyw'

        p1_f1['key1'] = 'pack1/file1.py'
        p1_f2['key1'] = 'pack1/file2.py'
        p1_f3['key1'] = 'pack1/file3.py'
        p1_init['key1'] = 'pack1/__init__.py'

        p1_s1_f1['key1'] = 'pack1/subpack1/file1.py'
        p1_s1_f2['key1'] = 'pack1/subpack1/file2.py'
        p1_s1_f3['key1'] = 'pack1/subpack1/file3.py'
        p1_s1_init['key1'] = 'pack1/subpack1/__init__.py'

        p2_f1['key1'] = 'pack2/file1.py'
        p2_f2['key1'] = 'pack2/file2.py'
        p2_f3['key1'] = 'pack2/file3.py'
        p2_init['key1'] = 'pack2/__init__.py'

        p3_f1['key1'] = 'pack3/file1.py'
        p3_init['key1'] = 'pack3/__init__.py'

        bogusfile['key1'] = 'bogusfilename'
        filecmp['key1'] = 'filecmp'
        os['key1'] = 'os'
        pickle['key1'] = 'pickle'
        sys['key1'] = 'sys'

	# Root
        g.add_edge(f1, p1_s1_f1)
        g.add_edge(f1, p1_init)
        g.add_edge(f1, p2_init)

        g.add_edge(f2, sys)
        g.add_edge(f2, p1_s1_init) # *-import
        g.add_edge(f2, p1_f1)
        g.add_edge(f2, os)
        g.add_edge(f2, p2_init)

        g.add_edge(f3, p2_init) # *-import
        g.add_edge(f3, bogusfile)

        # Pack 1
        g.add_edge(p1_f1, p1_s1_f1)
        g.add_edge(p1_f1, p2_f1)

        g.add_edge(p1_f2, f1)
        g.add_edge(p1_f2, f3)
        g.add_edge(p1_f2, p1_s1_f1)

        g.add_edge(p1_f3, sys)

        g.add_edge(p1_init, pickle)

        # Pack 1, Subpack 1
        g.add_edge(p1_s1_f1, f3)

        g.add_edge(p1_s1_f2, sys)
        g.add_edge(p1_s1_f2, f1)

        g.add_edge(p1_s1_f3, filecmp)

        # Pack 2 (__init__ has __all__ import)
        g.add_edge(p2_f2, filecmp)

        g.add_edge(p2_f3, p1_f2)

        g.add_edge(p2_init, p2_f1)
        g.add_edge(p2_init, p2_f2)
        g.add_edge(p2_init, p2_f3)

        # Pack 3 (__init__ has __all__ import)
        g.add_edge(p3_init, p3_f1)

        return g

    def print_graph_contents(self, G1, G2):

        nodes1 = G1.nodes()
        nodes2 = G2.nodes()

        print("\nFIRST GRAPH (INCLUDE GARDENER):")
        print("------------------------")
	
        n1keys = []
        for n1 in nodes1:
            n1keys.append(n1["key1"])

        print(n1keys)
        print("\nTotal:" + str(len(nodes1)))

        n2keys = []
        print("\nSECOND GRAPH (REFERENCE):")
        print("------------------------")
        for n2 in nodes2:
            n2keys.append(n2["key1"])

        print(n2keys)
        print("\nTotal:" + str(len(nodes2)) + "\n")


        if ((set(n1keys) - set(n2keys)) != set()):
            print("DIFFERENCE:")
            print("------------------------")
            print(set(n1keys) - set(n2keys))

    def compare(self, G1, G2):
        """ Compares two PyGraphml graphs by using PyUnittest's
            assert methods.
        """
        self.print_graph_contents(G1, G2)

        nodes1 = G1.nodes()
        nodes2 = G2.nodes()

        self.assertEqual(len(nodes1), len(nodes2))

        dst1 = []
        dst2 = []

        # get all children
        for n1 in nodes1:
            dst1.append(n1['key1'])

        for n2 in nodes2:
            dst2.append(n2['key1'])

        self.assertCountEqual(set(dst1), set(dst2))

    def gardener_call(self, options, subpath=""):

        p_args = [self.G_PATH, os.path.join(self.T_PATH, subpath)] + options + ['-l', 'py']
        print( ' '.join( p_args ) )
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

        temp = tempfile.NamedTemporaryFile()
        temp.write(result_str)
        temp.flush()

        parser = pgml.GraphMLParser()

        # get the result from the system call:
        return parser.parse(temp.name)

    def test_SimpleCallWithSinglePath_GraphmlOutput(self):
        """ Tests "include_gardener test_files -f xml -l py"

        The test expects that the result can be read by graphml
        and that there is at least one node.
        """
        g1 = self.graphml_gardener_call(['-j', '1'])

        # get a reference graph
        g2 = self.build_reference_graph()

        # both graphs shall be the same
        self.compare(g1, g2)

    def test_SimpleCallWithSinglePath(self):
        """ Tests "include_gardener test_files"

        The test expects that the result can be read by pygraphviz
        and that there is at least one node.
        """
        G = self.dot_gardener_call(['-j', '1'])

        # the first node shall not be None ...
        n = G.get_node(1)
        self.assertNotEqual(n, None)

if __name__ == "__main__":
    print(sys.argv)
    if len(sys.argv) > 2:
        GardenerTestCases.T_PATH = abspath(sys.argv.pop())
        GardenerTestCases.G_PATH = abspath(sys.argv.pop())
    import xmlrunner
    out_dir = abspath(sys.argv.pop())
    unittest.main(testRunner=xmlrunner.XMLTestRunner(output=out_dir))
