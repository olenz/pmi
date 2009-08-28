import pmi
pmi.setup(2)

# create the parallel function
pmi.exec_("""
from mpi4py import MPI

def hello_parallel(name):
    return 'Hello %s, this is MPI task %d!' % (name, MPI.COMM_WORLD.rank)
""")

# invoke the function on all workers and get the results as a list
res = pmi.invoke('hello_parallel', 'Olaf')

print('\n'.join(res))

