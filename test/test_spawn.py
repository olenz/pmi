from mpi4py import MPI
import sys, numpy

world = MPI.COMM_WORLD
print('Starter: Spawning.')
intercomm = world.Spawn(command = sys.executable,
                        args = ['test_spawn_child.py'], maxprocs=1)

buf = numpy.int(42)
print('Starter: Broadcasting %s to children via Intercomm.'
      % buf)
intercomm.Bcast([buf, MPI.LONG])

# print('Starter: Merging.')
# newcomm = intercomm.Merge(False)

