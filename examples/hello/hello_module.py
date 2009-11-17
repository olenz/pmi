import pmi
from hellomod import *

pmi.setup()

# No parallel code!
res = hello('Olaf')

print('\n'.join(res))

