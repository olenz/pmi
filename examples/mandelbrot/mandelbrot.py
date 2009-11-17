import time, sys, os
import logging.config

if os.path.exists('log.conf'):
    logging.config.fileConfig("log.conf")
else:
    logging.basicConfig()

if len(sys.argv) > 1 and sys.argv[1] == 'serial':
    from mandelbrot_serial import *
    serial = True
else:
    import pmi
    pmi.setup()
    from mandelbrot_pmi import *
    serial = False

x1 = -2.0
x2 =  1.0
y1 = -1.0
y2 =  1.0

w = 300
h = 200
maxit = 127

print('Computing Mandelbrot set (%dpx x %dpx).' % (w, h))
if serial:
    print('Running serially.')
else:
    print('Using %d MPI tasks.' % pmi.size)

before = time.time()
M = mandelbrot((x1, y1), (x2, y2), (w, h), maxit)
after = time.time()

print('Total time: %f seconds.' % (after-before))

try:
    from matplotlib import pyplot as plt
except ImportError:
    pass
else:
    plt.imshow(M, aspect='equal')
    plt.show()

