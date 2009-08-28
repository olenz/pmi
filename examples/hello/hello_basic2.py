if __name__ != 'pmi':
    ##################################################
    ## Serial code
    ##################################################
    import pmi

    pmi.setup()
    pmi.execfile_(__file__)

    # invoke the method on all workers and get the results
    res = pmi.invoke('hello_parallel', 'Olaf')

    print('\n'.join(res))

else:
    ##################################################
    ## Parallel code
    ##################################################
    from mpi4py import MPI

    def hello_parallel(name):
        return 'Hello %s, this is MPI task %d!' % (name, MPI.COMM_WORLD.rank)


