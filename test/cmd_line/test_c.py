###
#
# Command-line tests written in Python (C-language).
# Do
#  > python3 test_c.py test-reports ../../build/include_gardener ../test_files/c
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


class GardenerCTestCases(unittest.TestCase):

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

    @staticmethod
    def build_reference_graph():
        """ Builds a reference graph which shall be the same
            than the graph of ../test_files.

            A PyGraphml graph is returned.
        """
        g = pgml.Graph()

        f_1_c = g.add_node("1")
        f_2_c = g.add_node("2")
        f_3_c = g.add_node("3")
        existing_h = g.add_node("4")
        iostream = g.add_node("5")
        lib_f_1_h = g.add_node("6")
        lib_f_2_h = g.add_node("7")
        lib_f_3_h = g.add_node("8")
        lib2_f_4_h = g.add_node("9")
        lib2_f_1_h = g.add_node("10")

        f_1_c['key1'] = 'src/f_1.c'
        f_2_c['key1'] = 'src/f_2.c'
        f_3_c['key1'] = 'src/f_3.c'
        existing_h['key1'] = '../non/existing.h'
        iostream['key1'] = 'iostream'
        lib_f_1_h['key1'] = 'inc/lib/f_1.h'
        lib_f_2_h['key1'] = 'inc/lib/f_2.h'
        lib_f_3_h['key1'] = 'inc/lib/f_3.h'
        lib2_f_4_h['key1'] = 'inc/lib2/f_4.h'
        lib2_f_1_h['key1'] = 'inc/lib2/f_1.h'

        g.add_edge(f_3_c, existing_h)
        g.add_edge(f_3_c, iostream)
        g.add_edge(f_2_c, lib2_f_1_h)
        g.add_edge(f_1_c, iostream)
        g.add_edge(f_1_c, lib_f_2_h)
        g.add_edge(f_1_c, lib_f_1_h)
        g.add_edge(f_3_c, lib_f_3_h)
        g.add_edge(lib_f_2_h, lib_f_1_h)
        g.add_edge(lib_f_1_h, lib_f_3_h)
        g.add_edge(lib_f_1_h, lib2_f_4_h)
        g.add_edge(lib_f_3_h, lib_f_1_h)
        return g

    def test_SimpleCallWithSinglePath(self):
        """ Tests "include_gardener test_files"

        The test expects that the result can be read by pygraphviz
        and that there is at least one node.
        """
        G = self.gardener.dot_gardener_call(['-j', '1'])

        # the first node shall not be None ...
        n = G.get_node(1)
        self.assertNotEqual(n, None)

    def test_SimpleCallWithSinglePath_GraphmlOutput(self):
        """ Tests "include_gardener test_files -f xml -I test_files/inc"

        The test expects that the result can be read by graphml
        and that there is at least one node.
        """
        g1 = self.gardener.graphml_gardener_call(['-I', self.path_testfiles + '/inc/'])

        # get a reference graph
        g2 = GardenerCTestCases.build_reference_graph()

        # both graphs shall be the same
        compare(self, g1, g2)

    def test_LevelOption(self):
        """ Tests "include_gardener test_files -f xml" once without -L,
            once with -L 0, once with -L 1 and once with -L 2.

        The test expects that -L 0 counts no nodes, -L 1 counts just the
        nodes of the result of processing the files in src, and L 2 the
        same than wihtout the -L option.
        """
        g1 = self.gardener.graphml_gardener_call([])
        g2 = self.gardener.graphml_gardener_call(['-L', '0'])
        g3 = self.gardener.graphml_gardener_call(['-L', '1'])
        g4 = self.gardener.graphml_gardener_call(['-L', '2'])

        self.assertEqual(len(g2.nodes()), 0)
        self.assertEqual(len(g3.nodes()), 9)
        self.assertEqual(len(g4.nodes()), 15)
        compare(self, g1, g4)

    def test_ThreadOption(self):
        """ Tests "include_gardener test_files -f xml" with -j ranging
            from 1 to 5, each 10 times. Each call should produce the same
            result.
        """
        g1 = None
        for j in range(1, 5):
            for cnt in range(0, 10):
                if not g1:
                    # initialize reference
                    args = ['-I', self.path_testfiles +'/inc/', '-j', str(j)]
                    g1 = self.gardener.graphml_gardener_call(args)
                else:
                    args = ['-I', self.path_testfiles +'/inc/', '-j', str(j)]
                    g2 = self.gardener.graphml_gardener_call(args)
                    compare(self, g1, g2)

    def test_ThreadNullOption(self):
        """ Tests "include_gardener test_files -f xml -j 0" which should fail.
        """
        args = [self.path_testfiles, '-f', 'xml', '-j', '0']
        graphml_str, err_str = self.gardener.call(args)
        self.assertNotEqual(len(err_str), 0)
        self.assertIn("Error", err_str)

    def test_ExcludeOption(self):
        """ Tests "include_gardener test_files -f xml"
        once without the -e option, once with -e _1\.h and
        once with -e _1\.h -e _1\.c.
        The test expects that the first includes all files,
        the second call one file less and the third two files less.
        """
        args = ['-I', self.path_testfiles + '/inc/']
        g1 = self.gardener.graphml_gardener_call(args)
        args = ['-I', self.path_testfiles + '/inc/','-e', '_2\.c']
        g2 = self.gardener.graphml_gardener_call(args)
        args = ['-I', self.path_testfiles + '/inc/',
                '-e', '_2\.c', '-e', '_3\.c']
        g3 = self.gardener.graphml_gardener_call(args)

        self.assertEqual(len(g1.nodes()), 10)
        self.assertEqual(len(g2.nodes()), 9)

        # note: there are two three lodes less than in g1:
        # - f_2.c
        # - f_3.c
        # - non/existing.h (which is referenced in f_3.c)
        self.assertEqual(len(g3.nodes()), 7)

    def test_OutputFile(self):
        """ Tests if the gardener can write into an output file.

        The test expects that there is at least one node.
        """
        temp = tempfile.NamedTemporaryFile()
        self.gardener.graphml_gardener_call(['-o', temp.name])
        parser = pgml.GraphMLParser()

        # get the result from the system call:
        g = parser.parse(temp.name)
        temp = None
        self.assertNotEqual(len(g.nodes()), 0)


if __name__ == "__main__":
    main(sys.argv, GardenerCTestCases)

