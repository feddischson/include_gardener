###
#
# Command-line tests written in Python.
# Do
#  > python3 test_py.py test-reports ../../build/include_gardener ../test_files/py
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
    T_PATH = "../test_files/"

    def setUp(self):
        """Nothing to setup here"""

    def tearDown(self):
        """Nothing to tear down here"""

    def build_reference_graph(self):
        """ Builds a reference graph which shall be the same
            than the graph of ../test_files.

            A PyGraphml graph is returned.
        """
        g = pgml.Graph()

        # TODO: This might be all wrong... Need to be updated to actually reflect what's in the test pack of python files..
        p1_f1 = g.add_node("1")
        p1_f2 = g.add_node("2")
        p1_f3 = g.add_node("3")
        p1_s1_f1 = g.add_node("4")
        p1_s1_f2 = g.add_node("5")
        p1_s1_f3 = g.add_node("6")
        p1_init = g.add_node("14")
        p1_s1_init = g.add_node("15")
        non_existing_py = g.add_node("7")
        sys = g.add_node("8")
        filecmp = g.add_node("9")
        pickle = g.add_node("10")
        f1 = g.add_node("11")
        f2 = g.add_node("12")
        f3 = g.add_node("13")
        p2_f1 = g.add_node("16")
        p2_f2 = g.add_node("17")
        p2_f3 = g.add_node("18")
        p2_init = g.add_node("19")
        os = g.add_node("20")
        bogusfile = g.add_node("21")

        p1_f1['key1'] = 'pack1.file1'
        p1_f2['key1'] = 'pack1.file2'
        p1_f3['key1'] = 'pack1.file3'
        p1_init['key1'] = 'pack1.__init__'
        p1_s1_f1['key1'] = 'pack1.subpack1.file1'
        p1_s1_f2['key1'] = 'pack1.subpack1.file2'
        p1_s1_f3['key1'] = 'pack1.subpack1.file3'
        p1_s1_init['key1'] = 'pack1.subpack1.__init__'
        non_existing_py['key1'] = 'bogusfilename'
        sys['key1'] = 'sys'
        os['key1'] = 'os'
        filecmp['key1'] = 'filecmp'
        pickle['key1'] = 'pickle'
        f1['key1'] = 'file1'
        f2['key1'] = 'file2'
        f3['key1'] = 'file3'
        p2_f1['key1'] = 'pack2.file1'
        p2_f2['key1'] = 'pack2.file2'
        p2_f3['key1'] = 'pack2.file3'
        p2_init['key1'] = 'pack2.__init__'
        bogusfile['key1'] = 'bogusfilename'

        # Pack 1
        g.add_edge(p1_f1, p1_s1_f1)
        g.add_edge(p1_f3, sys)
        g.add_edge(p1_init, pickle)
        g.add_edge(p1_f1, p2_f1)

        # Subpack 1
        g.add_edge(p1_s1_f1, p1_s1_f3)
        g.add_edge(p1_s1_f2, sys)
        g.add_edge(p1_s1_f2, p1_s1_f1)

        # Pack 2
        g.add_edge(p2_f3, p1_f2)
        g.add_edge(p2_f2, pickle)
        g.add_edge(p2_init, p1_f1)
        g.add_edge(p2_f2, filecmp)

        # File 1
        g.add_edge(f1, p1_init)
        g.add_edge(f1, p2_init)
        g.add_edge(f1, p1_s1_f1)
       
        # File 2
        g.add_edge(f2, os)
        g.add_edge(f2, sys)
        g.add_edge(f2, p1_s1_init)
        g.add_edge(f2, p2_init)

        # File 3
        # TODO: Some could be removed if import * statements won't be supported
        g.add_edge(f3, p2_init)
        g.add_edge(f3, p2_f1)
        g.add_edge(f3, p2_f2)
        g.add_edge(f3, p2_f3)
        g.add_edge(f3, bogusfile)


        return g

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

    def test_SimpleCallWithSinglePath(self):
        """ Tests "include_gardener test_files"

        The test expects that the result can be read by pygraphviz
        and that there is at least one node.
        """
        G = self.dot_gardener_call(['-j', '1'])

        # the first node shall not be None ...
        n = G.get_node(1)
        self.assertNotEqual(n, None)

    def test_SimpleCallWithSinglePath_GraphmlOutput(self):
        """ Tests "include_gardener test_files -f xml -I test_files/inc"

        The test expects that the result can be read by graphml
        and that there is at least one node.
        """
        g1 = self.graphml_gardener_call(['-I', self.T_PATH + '/inc/'])

        # get a reference graph
        g2 = self.build_reference_graph()

        # both graphs shall be the same
        self.compare(g1, g2)

if __name__ == "__main__":
    print(sys.argv)
    if len(sys.argv) > 2:
        GardenerTestCases.T_PATH = abspath(sys.argv.pop())
        GardenerTestCases.G_PATH = abspath(sys.argv.pop())
    import xmlrunner
    out_dir = abspath(sys.argv.pop())
    unittest.main(testRunner=xmlrunner.XMLTestRunner(output=out_dir))
