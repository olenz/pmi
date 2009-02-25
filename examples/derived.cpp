// Basic example for using PMI.
//
// Demonstrates using PMI when deriving the parallel class.
// Note that this approach does NOT allow to create parallel instances
// of class hierarchies.
//
// Execute via:
//
//   > mpiexec -n 2 hello

#include <iostream>
#include <pmi/pmi.hpp>
#include <logging/log4espp.hpp>
#include <mpi.h>

using namespace std;

// Define a very simple class.
// Note, that in this case, the class does not contain a member of the
// parallel class, but is derived from the parallel class.
class HelloWorld : pmi::ParallelClass<HelloWorld> {
private:
  unsigned short computeMessage() {
    return pmi::getWorkerId();
  }

public:
  // BASIC PARALLEL METHOD DEFINITION
  // Same as in basic.cpp.
  string getMessage() {
    // invoke the parallel method
    this->invoke<&HelloWorld::getMessageWorker>();

    // compute the message
    unsigned short myMsg = computeMessage();

    // gather the messages from the different workers
    int size = MPI::COMM_WORLD.Get_size();
    unsigned short allMsg[size];
    MPI::COMM_WORLD.Gather(&myMsg, 1, MPI::UNSIGNED_SHORT,
			   allMsg, 1, MPI::UNSIGNED_SHORT, 
			   pmi::getControllerMPIRank());
    // compose and print the message
    ostringstream ost;
    ost << "getMessage(): Got \"Hello World\" from workers: " << allMsg[0];
    for (int i = 1; i < size; i++)
      ost << ", " << allMsg[i];
    return ost.str();
  }

  // The worker function.
  void getMessageWorker() {
    // compute the message
    unsigned short myMsg = computeMessage();
    
    // send the message to the controller
    MPI::COMM_WORLD.Gather(&myMsg, 1, MPI::UNSIGNED_SHORT, 
			   0, 0, MPI::UNSIGNED_SHORT, 
			   pmi::getControllerMPIRank());
    
  }

  // SPMD style
  void printMessageLocal() {
    unsigned short myMsg = computeMessage();

    if (pmi::isWorker()) {
      // send the Id to the MPI task 0
      MPI::COMM_WORLD.Gather(&myMsg, 1, MPI::UNSIGNED_SHORT, 
			     0, 0, MPI::UNSIGNED_SHORT, 
			     pmi::getControllerMPIRank());
    } else {
      // gather the Ids
      int size = MPI::COMM_WORLD.Get_size();
      unsigned short allMsg[size];
      MPI::COMM_WORLD.Gather(&myMsg, 1, MPI::UNSIGNED_SHORT,
			     allMsg, 1, MPI::UNSIGNED_SHORT, 
			     pmi::getControllerMPIRank());
      // compose the message
      ostringstream ost;
      ost << "printeMessage(): Got \"Hello World\" from workers: " << allMsg[0];
      for (int i = 1; i < size; i++)
	ost << ", " << allMsg[i];
      cout << ost.str() << endl;
    }
  }

  PMI_CREATE_SPMD_METHOD(printMessage, HelloWorld, printMessageLocal, (*this));
};

// register the class and method with PMI
PMI_REGISTER_CLASS(HelloWorld);
PMI_REGISTER_METHOD(HelloWorld, printMessageLocal);
PMI_REGISTER_METHOD(HelloWorld, getMessageWorker);

int main(int argc, char* argv[]) {
  // Required by MPI
  MPI::Init(argc, argv);
  // Required by the logging system
  LOG4ESPP_CONFIGURE();

  // mainLoop will return "false" only on the controller
  if (pmi::isWorker()) pmi::mainLoop();
  else {
    HelloWorld hello;
    
    cout << hello.getMessage() << endl;

    hello.printMessage();

    // Stop the workers
    pmi::endWorkers();
  } 


  MPI::Finalize();
  return 0;
}

