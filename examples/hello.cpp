// Basic example for using PMI.
//
// Demonstrates the most basic usage of PMI.
//
// Execute via:
//
//   > mpiexec -n 2 hello

#include <iostream>
#include <pmi/pmi.hpp>
#include <logging/log4espp.hpp>
#include <mpi.h>

using namespace std;

// define a very simple class
class HelloWorld : public pmi::ParallelClass<HelloWorld> {
private:
  unsigned short computeMessage() {
    return pmi::getWorkerId();
  }

public:

  // A void function that prints out something. SPMD style.
  // Noe that this prints out the message on all the workers itself,
  // it does NOT transfer the messages.
  void _printMessage() {
    cout << "printMessage(): Worker " << computeMessage()
	 << ": Hello World!" 
	 << endl;
  }

  // like this, we create the method printMessage that calls the
  // parallel version
  PMI_CREATE_SPMD_METHOD(printMessage, _printMessage);

  // Function that returns its message to the controller. The
  // controller prints the message. SPMD style. 
  void _printMessage2() {
    unsigned short myMsg = computeMessage();

    if (pmi::isWorker()) {
      // send the Id to the MPI task 0
      MPI::COMM_WORLD.Gather(&myMsg, 1, MPI::UNSIGNED_SHORT, 
			     0, 0, MPI::UNSIGNED_SHORT, 0);
    } else {
      // gather the Ids
      int size = MPI::COMM_WORLD.Get_size();
      unsigned short allMsg[size];
      MPI::COMM_WORLD.Gather(&myMsg, 1, MPI::UNSIGNED_SHORT,
			     allMsg, 1, MPI::UNSIGNED_SHORT, 0);
      // compose the message
      ostringstream ost;
      ost << "printeMessage2(): Got \"Hello World\" from workers: " << allMsg[0];
      for (int i = 1; i < size; i++)
	ost << ", " << allMsg[i];
      cout << ost.str() << endl;
    }
  }

  PMI_CREATE_SPMD_METHOD(printMessage2, _printMessage2);

  // Function that returns the message. This function actually
  // requires the definition of two functions, one of which is called
  // on the controller, and the other one on the worker.
  // Note, that the worker function does not have any input arguments
  // nor does it return anything - all communication is done via MPI.
  string getMessage() {
    // invoke the parallel method
    invoke<&HelloWorld::getMessageWorker>();

    // compute the message
    unsigned short myMsg = computeMessage();

    // gather the messages from the different workers
    int size = MPI::COMM_WORLD.Get_size();
    unsigned short allMsg[size];
    MPI::COMM_WORLD.Gather(&myMsg, 1, MPI::UNSIGNED_SHORT,
			   allMsg, 1, MPI::UNSIGNED_SHORT, 0);
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
			   0, 0, MPI::UNSIGNED_SHORT, 0);
    
  }
};

// register the class and method with PMI
PMI_REGISTER_CLASS("HelloWorld", HelloWorld);
PMI_REGISTER_METHOD("_printMessage", HelloWorld, _printMessage);
PMI_REGISTER_METHOD("_printMessage2", HelloWorld, _printMessage2);
PMI_REGISTER_METHOD("getMessageWorker", HelloWorld, getMessageWorker);

int main(int argc, char* argv[]) {
  // Required by MPI
  MPI::Init(argc, argv);
  // Required by the logging system
  LOG4ESPP_CONFIGURE();

  // mainLoop will return "false" only on the controller
  if (!pmi::mainLoop()) {
    // create a parallel instance of the object
    HelloWorld hello;
    
    // Call both void methods of the object
    hello.printMessage();
    hello.printMessage2();

    cout << hello.getMessage() << endl;

    // Stop the workers
    pmi::endWorkers();
  } 


  MPI::Finalize();
  return 0;
}

