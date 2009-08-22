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
class HelloWorld {
private:
  // The class contains an instance of the parallel class as a member.
  pmi::ParallelClass<HelloWorld> pmiObject;

public:
  // BASIC PARALLEL METHOD DEFINITION

  // The method getMessage returns a message that is composed from
  // messages obtained from the different workers.
  // To implement such a method, one typically needs to define two
  // methods: the method that is called on the worker, and the method
  // that is called on the controller. In many cases, the controller
  // and the worker have to do a common piece of work. In that case, a
  // third method might be useful, that does the common work.
  // Note, that the method called on the worker does not have any
  // input arguments nor does it return anything - all communication
  // has to be done via MPI.

  // The controller method
  string getMessage() {
    // invoke the method on the workers
    pmiObject.invoke<&HelloWorld::getMessageWorker>();

    // compute the message (does the common computation)
    unsigned short myMsg = getMessageLocal();

    // gather the messages from the different workers
    int size = MPI::COMM_WORLD.Get_size();
    unsigned short allMsg[size];
    MPI::COMM_WORLD.Gather(&myMsg, 1, MPI::UNSIGNED_SHORT,
			   allMsg, 1, MPI::UNSIGNED_SHORT, 
			   pmi::getControllerMPIRank());
    // compose and return the message
    ostringstream ost;
    ost << "getMessage(): Got \"Hello World\" from workers: " << allMsg[0];
    for (int i = 1; i < size; i++)
      ost << ", " << allMsg[i];
    return ost.str();
  }

  // The worker method. This is called on all workers.
  void getMessageWorker() {
    // compute the message
    unsigned short myMsg = getMessageLocal();
    
    // send the message to the controller
    MPI::COMM_WORLD.Gather(&myMsg, 1, MPI::UNSIGNED_SHORT, 
			   0, 0, MPI::UNSIGNED_SHORT, 
			   pmi::getControllerMPIRank());
    
  }

  // Define a simple method that does some "computing".
  unsigned short getMessageLocal() {
    return pmi::getWorkerId();
  }

  // SPMD STYLE PARALLEL METHOD DEFINITION
  
  // When a method does not have any arguments and does not return
  // anything, it is possible to make the definition of such a method
  // much simpler: just define a single method that does all the work,
  // but distinguishes between controller and worker, and use the
  // macro PMI_CREATE_SPMD_METHOD to define an SPMD method from that.
  void printMessageLocal() {
    unsigned short myMsg = pmi::getWorkerId();

    if (!pmi::isController()) {
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

  // This defines a method printMessage that calls printMessageLocal on
  // all workers and on the controller.
  PMI_CREATE_SPMD_METHOD(printMessage, HelloWorld, printMessageLocal, pmiObject);

};

#ifdef HAVE_MPI
// Finally, the class and methods have to be registered with PMI
// outside of the class scope.
PMI_REGISTER_CLASS(HelloWorld);
PMI_REGISTER_METHOD(HelloWorld, getMessageWorker);
PMI_REGISTER_METHOD(HelloWorld, printMessageLocal);
#endif

int main(int argc, char* argv[]) {
  // Required by MPI
  MPI::Init(argc, argv);
  // Required by the logging system
  LOG4ESPP_CONFIGURE();

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

