import pmi
pmi.setup()

from hellomod import *

# No parallel code!
res = hello('Olaf')

print('\n'.join(res))

