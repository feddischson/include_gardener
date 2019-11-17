
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

