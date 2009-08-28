import pmi

if pmi.isController:
    pmi.exec_('import hellomod')

from mpi4py import MPI

def hello_parallel(name):
    return 'Hello %s, this is MPI task %d!' \
           % (name, MPI.COMM_WORLD.rank)

def hello(name):
    return pmi.invoke('hellomod.hello_parallel', name)
