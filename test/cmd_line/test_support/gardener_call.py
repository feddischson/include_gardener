import os
import pygraphviz as pgv
import pygraphml as pgml
import logging
import tempfile
from subprocess import Popen, PIPE


class GardenerCall:

    def __init__(self, path_gardener, path_testfiles):
        self.path_gardener = path_gardener
        self.path_testfiles = path_testfiles
        logging.info('created gardener: {} {}'.format(path_gardener, path_testfiles))

    def call(self, options=[], decode=True):
        """
        Calls the include gardener with the path 
        to the test-files as argument
        """
        p_args = [self.path_gardener] + options
        logging.debug(  'call gardener: ' + ' '.join( p_args ) )
        pipe = Popen(p_args, stdout=PIPE, stderr=PIPE)
        out, err = pipe.communicate()
        if decode:
            return (out.decode('utf-8'), err.decode('utf-8'))
        else:
            return (out, err)

    def dot_gardener_call(self, options, subpath=""):
        """
        Runs the include_gardener with the dot option,
        extracts the result and returns the graph.
        """
        # we don't use '-f dot' because dot is the default!
        result_str, err = self.call([os.path.join(self.path_testfiles,
                                                  subpath)] +
                                     options)
        logging.debug("dot-result: {}".format(result_str))
        G = pgv.AGraph(result_str)
        return G

    def graphml_gardener_call(self, options, subpath=""):
        """
        Runs the include_gardener with the xml option,
        extracts the result and returns the graph.
        """
        result_str, err = self.call([os.path.join(self.path_testfiles,
                                             subpath),
                                             '-f', 'xml'] + options, False)
        if len(result_str) == 0:
            return None
        logging.debug("graphml-result: {} ".format(result_str.decode('utf-8')))
        temp = tempfile.NamedTemporaryFile()
        temp.write(result_str)
        temp.flush()
        parser = pgml.GraphMLParser()

        # get the result from the system call:
        return parser.parse(temp.name)

