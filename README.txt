********************************
PMI - Parallel Method Invocation
********************************

PMI is a pure python module that allows libraries to provide functions
that are parallelized using MPI but that can nontheless be called from
serial Python scripts.

Requirements
------------
- Python version 2.4+
- An MPI module: 
  - mpi4py (http://mpi4py.scipy.org/) is recommended and can be
    installed from PyPI via easy_install or pip.
  - boostmpi (http://documen.tician.de/boostmpi/) should also work
    fine.

Installation
------------
To get and install PMI, you can either install it using easy_install:

   easy_install pmi

or pip:

   pip install pmi

or you can download it directly from the modules homepage:

    http://www.espresso-pp.de/projects/pmi/?subpage=summary

and install it using the distutils:

    python setup.py install

Documentation
-------------
PMI is documented using pydoc, i.e. if you have a working python
installation, you should be able to access the documentation using

    pydoc pmi


Contributors
------------
Olaf Lenz <olaf@lenz.name>
Thomas Brandes
Axel Arnold
Jonathan Halverson
Torsten Stühn
Dirk Reith
Anton Schüller
