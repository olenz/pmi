##################################################
## Serial code
##################################################
import pmi

pmi.setup()
pmi.execfile_('hello_class.py')

# create a frontend class via the proxy
class Hello(object):
    __metaclass__ = pmi.Proxy
    pmiproxydefs = \
        dict(cls = 'HelloLocal',
             pmiinvoke = [ '__call__' ])

# use the class
hello = Hello('Olaf')
print('\n'.join(hello()))
