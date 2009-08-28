import pmi
if pmi.isController:
    pmi.exec_('import mandelbrot_pmi')

from mpi4py import MPI
import numpy as np
import time

def mandelbrot_func(x, y, maxit):
    c = x + y*1j
    z = 0 + 0j
    it = 0
    while abs(z) < 2.0 and it < maxit:
        z = z**2 + c
        it += 1
    return it

def mandelbrot_parallel((x1, y1), (x2, y2), (w, h), maxit):
    before = time.time()
    
    ##################################################
    # initial communication
    comm = MPI.COMM_WORLD
    size = np.intc(comm.size)
    rank = np.intc(comm.rank)

    rmsg = np.empty(4, dtype='f')
    imsg = np.empty(3, dtype='l')
    
    if pmi.isController:
        rmsg[:] = [x1, x2, y1, y2]
        imsg[:] = [w, h, maxit]
    
    comm.Bcast([rmsg, MPI.FLOAT], root=0)
    comm.Bcast([imsg, MPI.LONG], root=0)
    
    x1, x2, y1, y2 = rmsg
    w, h, maxit    = imsg

    dx = (x2 - x1) / w
    dy = (y2 - y1) / h

    # number of lines to compute here
    N = h // size + (h % size > rank)
    # indices of lines to compute here
    I = np.arange(rank, h, size, dtype='l')

    after = time.time()
    print('%f seconds for initial communication on task %d.'
          % ((after-before, pmi.rank)))

    before = time.time()

    ##################################################
    # compute local lines
    C = np.empty([N, w], dtype='l')
    for k in np.arange(N):
        y = y1 + np.single(I[k] * dy)
        for j in np.arange(w):
            x = x1 + np.single(j * dx)
            C[k, j] = mandelbrot_func(x, y, maxit)

    after = time.time()
    print('%f seconds for computation on task %d.'
          % ((after-before, pmi.rank)))

    before = time.time()

    ##################################################
    # gather results at root
    counts = np.intc()
    indices = np.intc()
    cdata = np.intc()
    if pmi.isController:
        counts = np.empty(size, dtype='l')
        indices = np.empty(h, dtype='l')
        cdata = np.empty([h, w], dtype='l')

    comm.Gather(sendbuf=[N, MPI.LONG],
                recvbuf=[counts, MPI.LONG],
                root=pmi.CONTROLLER)
    comm.Gatherv(sendbuf=[I, MPI.LONG],
                 recvbuf=[indices, (counts, None), MPI.LONG],
                 root=pmi.CONTROLLER)
    comm.Gatherv(sendbuf=[C, MPI.LONG],
                 recvbuf=[cdata, (counts*w, None), MPI.LONG],
                 root=pmi.CONTROLLER)

    after = time.time()
    print('%f seconds for final communication on task %d.'
          % ((after-before), pmi.rank))

    if pmi.isWorker: return
    M = np.zeros([h, w], dtype='l')
    M[indices, :] = cdata

    return M

def mandelbrot(c1, c2, size, maxit):
    return pmi.call('mandelbrot_pmi.mandelbrot_parallel',
                    c1, c2, size, maxit)

