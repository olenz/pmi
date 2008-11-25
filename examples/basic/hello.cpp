// Basic example for using PMI.
//
// Demonstrates the most basic usage of PMI.
//
// Execute via:
//
//   > mpiexec -n 2 hello

#include <iostream>
#include <pmi/pmi.hpp>
#include <pmi/log4espp.hpp>
#include <mpi.h>

using namespace std;

// define a very simple class
class HelloWorld {
public:
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

int main(int argc, char* argv[]) {
  // Required by MPI
  MPI::Init(argc, argv);
  // Required by the logging system
  LOG4ESPP_CONFIGURE();

  // mainLoop will return "false" only on the controller
  if (!pmi::mainLoop()) {
    // Create a parallel instance of the object
    pmi::ParallelObject<HelloWorld> hello;
    
    // Call a method of the object
    hello.invokeVoid<&HelloWorld::printMessage>();

    string s = 
      hello.invoke<const string, &HelloWorld::getMessage>();
    cout << s << endl;

    // Stop the workers
    pmi::endWorkers();
  } 


  MPI::Finalize();
  return 0;
}

