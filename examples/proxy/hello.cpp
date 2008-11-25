// Example for using PMI and a proxy class.
//
// Demonstrates, how a parallel proxy class can be created for a
// class, that can be called as though it was the original object.

#include <mpi.h>
#include <iostream>
#include <pmi/pmi.hpp>
#include <string>
#include <sstream>

using namespace std;

// define a very simple class
class HelloWorld {
public:
  // A simple function that prints out a message on every worker
  void printMessage() {
    cout << "Worker " << pmi::getWorkerId() 
	 << ": Hello World!" 
	 << endl;
  }

  // A function that gathers information from all workers
  const string getMessage() {
    unsigned short myMsg = pmi::getWorkerId();
    if (pmi::isWorker()) {
      // send the Id to the MPI task 0
      MPI::COMM_WORLD.Gather(&myMsg, 1, MPI::UNSIGNED_SHORT, 
			     0, 0, MPI::UNSIGNED_SHORT, 0);
      return string();
    } else {
      // gather the Ids
      int size = MPI::COMM_WORLD.Get_size();
      unsigned short allMsg[size];
      MPI::COMM_WORLD.Gather(&myMsg, 1, MPI::UNSIGNED_SHORT,
			     allMsg, 1, MPI::UNSIGNED_SHORT, 0);
      // compose the message
      ostringstream ost;
      ost << "Got \"Hello World\" from workers: " << allMsg[0];
      for (int i = 1; i < size; i++)
	ost << ", " << allMsg[i];
      return ost.str();
    }
  }
};

// register the class and method with PMI
PMI_REGISTER_CLASS("HelloWorld", HelloWorld);
PMI_REGISTER_METHOD_VOID("printMessage", HelloWorld, printMessage);
PMI_REGISTER_METHOD_SPMD("getMessage", HelloWorld, getMessage, const string);

// define the parallel proxy class
class PHelloWorld 
  : public pmi::ParallelObject<HelloWorld> {
public:
  // define its methods
  PMI_PARALLEL_PROXY_METHOD_VOID(printMessage);
  PMI_PARALLEL_PROXY_METHOD(getMessage, const string);
};

int main(int argc, char* argv[]) {
  // Required by MPI
  MPI::Init(argc, argv);
  // Required by the logging system
  LOG4ESPP_CONFIGURE();

  if (!pmi::mainLoop()) {
    // Create an instance of the proxy class
    PHelloWorld hello;
    
    // Call the simple method
    hello.printMessage();

    // Call the method that returns something
    string s = hello.getMessage();
    cout << "Got message: " << s << endl;

    // Stop the workers
    pmi::endWorkers();
  }
    
  return 0;
}

