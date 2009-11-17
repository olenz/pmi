if __name__ != 'pmi':
    ##################################################
    ## Serial code
    ##################################################
    import pmi

    pmi.setup()
    pmi.execfile_(__file__)

    # create a frontend class
    class Hello(object):
        def __init__(self, name):
            self.pmiobj = pmi.create('HelloLocal', name)
        def __call__(self):
            return pmi.invoke(self.pmiobj, '__call__')

    # use the class
    hello = Hello('Olaf')
    print('\n'.join(hello()))

else:
    ##################################################
    ## Parallel code
    ##################################################
    from mpi4py import MPI

    class HelloLocal(object):
        def __init__(self, name):
            self.name = name
        def __call__(self):
            return 'Hello %s, this is MPI task %d!' % (self.name, MPI.COMM_WORLD.rank)

