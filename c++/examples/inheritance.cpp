// Example for using PMI.
//
// Demonstrates, how class hierarchies can be used together with
// PMI. Note that this is only possible when defining the parallel
// class as a member of the subject class, not when the subject class
// is derived from the parallel class.
//
// Execute via:
//
//   > mpiexec -n 2 hello
#include <iostream>
#include <pmi/pmi.hpp>
#include <logging/log4espp.hpp>
#include <mpi.h>

using namespace std;

// Define a simple class A
class A {
  pmi::ParallelClass<A> pclass;

protected:
  unsigned short computeMessage() {
    return pmi::getWorkerId();
  }

public:
  void printMessageLocal() {
    cout << "A::printMessage(): Worker " << computeMessage()
	 << ": Hello World!" 
	 << endl;
  }

  PMI_CREATE_SPMD_METHOD(printMessage, A, printMessageLocal, pclass);
};

// Register the class with PMI
PMI_REGISTER_CLASS(A);
PMI_REGISTER_METHOD(A, printMessageLocal);


// Define am simple class B that is derived from A
class B : public A {
  pmi::ParallelClass<B> pclass;

public:
  void printMessageLocal() {
    cout << "B::printMessage(): Worker " << computeMessage()
	 << ": Hello World!" 
	 << endl;
  }

  PMI_CREATE_SPMD_METHOD(printMessage, B, printMessageLocal, pclass);

};

// Register the class with PMI
PMI_REGISTER_CLASS(B);
PMI_REGISTER_METHOD(B, printMessageLocal);

int main(int argc, char* argv[]) {
  // Required by MPI
  MPI::Init(argc, argv);
  // Required by the logging system
  LOG4ESPP_CONFIGURE();

  if (pmi::isWorker()) pmi::mainLoop();
  else {
    A a;
    a.printMessage();
    
    B b;
    b.printMessage();
    
    // Stop the workers
    pmi::endWorkers();
  } 

  MPI::Finalize();
  return 0;
}

