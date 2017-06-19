###
#
# Command-line tests written in Python.
# Do
#    >  python3 test.py ../../include_gardener.conf ../../build/include_gardener ../test_files/
# to run the tests
#
# The first argument is used to defines the include_gardener executable,
# the second one defines where test-tree / files are located.
#
###
import unittest
import sys
import tempfile
import pygraphviz as pgv
import pygraphml  as pgml

from subprocess import Popen,PIPE,call
from os.path import abspath

class GardenerTestCases(unittest.TestCase):

    # default paths
    G_PATH="../../build/include_gardener"
    T_PATH="../test_files/"
    C_PATH="../../include_gardener.conf"

    def setUp( self ):
        """Nothing to setup here"""


    def tearDown( self ):
        """Nothing to tear down here"""


    def build_reference_graph( self ):
        """ Builds a reference graph which shall be the same
            than the graph of ../test_files.

            A PyGraphml graph is returned.
        """
        g = pgml.Graph()

        f_1_c       = g.add_node( "1" )
        f_2_c       = g.add_node( "2" )
        f_3_c       = g.add_node( "3" )
        existing_h  = g.add_node("4")
        iostream    = g.add_node("5")
        lib_f_1_h   = g.add_node("6")
        lib_f_2_h   = g.add_node("7")
        lib_f_3_h   = g.add_node("8")
        lib2_f_4_h  = g.add_node("9")
        lib2_f_1_h  = g.add_node("10")


        f_1_c      ['key1'] = 'src/f_1.c'
        f_2_c      ['key1'] = 'src/f_2.c'
        f_3_c      ['key1'] = 'src/f_3.c'
        existing_h ['key1'] = '../non/existing.h'
        iostream   ['key1'] = 'iostream'
        lib_f_1_h  ['key1'] = 'lib/f_1.h'
        lib_f_2_h  ['key1'] = 'lib/f_2.h'
        lib_f_3_h  ['key1'] = 'lib/f_3.h'
        lib2_f_4_h ['key1'] = 'lib2/f_4.h'
        lib2_f_1_h ['key1'] = '../inc/lib2/f_1.h'

        g.add_edge( f_3_c, existing_h     )
        g.add_edge( f_3_c, iostream       )
        g.add_edge( f_2_c, lib2_f_1_h     )
        g.add_edge( f_1_c, iostream       )
        g.add_edge( f_1_c, lib_f_2_h      )
        g.add_edge( f_1_c, lib_f_1_h      )
        g.add_edge( f_3_c, lib_f_3_h      )
        g.add_edge( lib_f_2_h, lib_f_1_h  )
        g.add_edge( lib_f_1_h, lib_f_3_h  )
        g.add_edge( lib_f_1_h, lib2_f_4_h )
        g.add_edge( lib_f_3_h, lib_f_1_h  )
        return g


    def compare( self, G1, G2 ):
        """ Compares two PyGraphml graphs by using PyUnittest's
            assert methods.
        """

        nodes1 = G1.nodes()
        nodes2 = G2.nodes()
        self.assertEqual( len( nodes1 ), len( nodes2 ) )
        for n1 in nodes1:
            found = False

            src1 = n1['key1']
            src2 = ""
            dst1 = []
            dst2 = []

            # get all children
            for c1 in G1.children( n1 ):
                dst1.append( c1['key1'] )

            # search for the src in the second list
            for n2 in nodes2:
                src2 = n2['key1']

                if src1 == src2:
                    found = True
                    # get all children
                    for c2 in G2.children( n2 ):
                        dst2.append( c2['key1'] )
                    break

            self.assertTrue( found )
            self.assertEqual( src1, src2 )
            self.assertCountEqual( dst1, dst2 )


    def test_showsUnrecognisedOption( self ):
        """Tests "include_gardener --xyz"

        The test expects an unrecognised option information.
        """
        pipe = Popen( [ self.G_PATH, "--xyz" ], stderr=PIPE )
        result = pipe.communicate()[1].decode("utf-8")
        self.assertEqual( result, "unrecognised option '--xyz'\n" )


    def test_printHelp1( self ):
        """Tests "include_gardener -h"

        The test expects at least 'Options:' in stdout.
        """
        pipe = Popen( [ self.G_PATH, "-h" ], stdout=PIPE  )
        result = pipe.communicate()[0].decode("utf-8")
        self.assertIn( "Options:", result )


    def test_printHelp2( self ):
        """Tests "include_gardener --help"

        The test expects at least 'Options:' in stdout.
        """
        pipe = Popen( [ self.G_PATH, "--help" ], stdout=PIPE  )
        result = pipe.communicate()[0].decode("utf-8")
        self.assertIn( "Options:", result )


    def test_Version1( self ):
        """Tests "include_gardener -v"

        The test expects at least 'Version' in stdout.
        """
        pipe = Popen( [ self.G_PATH, "-v" ], stdout=PIPE  )
        result = pipe.communicate()[0].decode("utf-8")
        self.assertIn( "Version", result )


    def test_Version2( self ):
        """Tests "include_gardener --version"

        The test expects at least 'Version' in stdout.
        """
        pipe = Popen( [ self.G_PATH, "--version" ], stdout=PIPE  )
        result = pipe.communicate()[0].decode("utf-8")
        self.assertIn( "Version", result )


    def test_SimpleCallWithSinglePath( self ):
        """ Tests "include_gardener test_files"

        The test expects that the result can be read by pygraphviz
        and that there is at least one node.
        """
        pipe = Popen( [ self.G_PATH, self.T_PATH ], stdout=PIPE  )
        graph_str1 = pipe.communicate()[0].decode("utf-8")
        G = pgv.AGraph( graph_str1 )

        # the first node shall not be None ...
        n = G.get_node(1)
        self.assertNotEqual( n, None )

        # ... and the default format shall be dot
        pipe = Popen( [ self.G_PATH, self.T_PATH, '-f', 'dot' ], stdout=PIPE  )
        graph_str2 = pipe.communicate()[0].decode("utf-8")
        self.assertEqual( graph_str1, graph_str2 )

    def gardener_call( self, options ):

        p_args = [ self.G_PATH, self.T_PATH, "-c", self.C_PATH ] + options
        print( p_args )
        pipe = Popen( p_args, stdout=PIPE  )
        graphml_str = pipe.communicate()[0]
        temp = tempfile.NamedTemporaryFile()
        temp.write( graphml_str )
        temp.flush()
        parser = pgml.GraphMLParser()

        # get the result from the system call:
        return parser.parse( temp.name )


    def graphml_gardener_call( self, options ):
        """
        Runs the include_gardener with the xml option,
        extracts the result and returns the graph.
        """
        return self.gardener_call( [ '-f', 'xml' ] + options )

    def test_SimpleCallWithSinglePath_GraphmlOutput( self ):
        """ Tests "include_gardener test_files -f xml -I test_files/inc"

        The test expects that the result can be read by graphml
        and that there is at least one node.
        """
        g1 = self.graphml_gardener_call( [ '-I', self.T_PATH + '/inc/' ] )

        # get a reference graph
        g2 = self.build_reference_graph()

        # both graphs shall be the same
        self.compare( g1, g2 )


    def test_LevelOption( self ):
        """ Tests "include_gardener test_files -f xml" once without -L,
            once with -L 0, once with -L 1 and once with -L 2.

        The test expects that -L 0 counts no nodes, -L 1 counts just the nodes of
        the result of processing the files in src, and L 2 the same than wihtout
        the -L option.
        """
        g1 = self.graphml_gardener_call( [ ] )
        g2 = self.graphml_gardener_call( [ '-L', '0' ] )
        g3 = self.graphml_gardener_call( [ '-L', '1' ] )
        g4 = self.graphml_gardener_call( [ '-L', '2' ] )

        self.assertEqual( len( g2.nodes() ), 0 )
        self.assertEqual( len( g3.nodes() ), 9 )
        self.assertEqual( len( g4.nodes() ), 14 )
        self.compare( g1, g4 )


    def test_ThreadOption( self ):
        """ Tests "include_gardener test_files -f xml" with -j ranging
            from 1 to 5, each 10 times. Each call should produce the same
            result.
        """

        g1 = None
        for j in range( 1, 5 ):
            for cnt in range( 0, 10 ):
                if not g1:
                    # initialize reference
                    g1 = self.graphml_gardener_call( [ '-I', self.T_PATH + '/inc/', '-j', str(j) ] )
                else:
                    g2 = self.graphml_gardener_call( [ '-I', self.T_PATH + '/inc/', '-j', str(j) ] )
                    self.compare( g1, g2 )


    def test_ThreadNullOption( self ):
        """ Tests "include_gardener test_files -f xml -j 0" which should fail.
        """
        pipe = Popen( [ self.G_PATH, self.T_PATH,
            '-f', 'xml', '-j', '0'  ], stderr=PIPE  )
        graphml_str = pipe.communicate()[1].decode("utf-8")
        self.assertNotEqual( len( graphml_str ), 0 )
        self.assertIn( "Error", graphml_str )


    def test_ExcludeOption( self ):
        """ Tests "include_gardener test_files -f xml"
        once without the -e option, once with -e _1\.h and once with -e _1\.h -e _1\.c.

        The test expects that the first includes all files,
        the second call one file less and the third two files less.
        """
        g1 = self.graphml_gardener_call( [ ] )
        g2 = self.graphml_gardener_call( [ '-e', '_1\.h' ] )
        g3 = self.graphml_gardener_call( [ '-e', '_1\.h', '-e', '_1\.c'] )


        # Note: there are two file which ends with _1.h, but we expect
        #       a difference of one. Reason: the second file is referenced and
        #       therefore added to the graph.
        self.assertEqual( len( g1.nodes() ), len( g2.nodes() )+1 )


        # because we do not process f_1.c, the second _1.h file mentioned
        # above is not added to the graph. Therefore, a difference of three
        # is correct.
        self.assertEqual( len( g1.nodes() ), len( g3.nodes() )+3 )


    def test_OutputFile( self ):
        """ Tests if the gardener can write into an output file.

        The test expects that there is at least one node.
        """
        temp = tempfile.NamedTemporaryFile()
        call( [ self.G_PATH, self.T_PATH,
            '-f', 'xml', '-o', temp.name  ] )
        parser = pgml.GraphMLParser()

        # get the result from the system call:
        g = parser.parse( temp.name )

        self.assertNotEqual( len( g.nodes() ), 0 )

if __name__ == "__main__":
    if len( sys.argv ) > 2:
        GardenerTestCases.T_PATH = abspath( sys.argv.pop() )
        GardenerTestCases.G_PATH = abspath( sys.argv.pop() )
        GardenerTestCases.C_PATH = abspath( sys.argv.pop() )
    unittest.main() # run all tests

