#include <iostream>
#include <pmi/pmi.hpp>
#include <boost/mpi.hpp>

using namespace std;

// define a very simple class
class HelloWorld {
public:
  void printMessage() {
    cout << "Worker " << pmi::getWorkerId() 
	 << ": Hello World!" 
	 << endl;
  }
};

int main(int argc, char* argv[]) {
  // This is required when MPI is used
  boost::mpi::environment env(argc, argv);

  LOG4ESPP_CONFIGURE();

  // Register the class and the method
  pmi::registerClass<HelloWorld>("HelloWorld")
    .registerMethod<&HelloWorld::printMessage>("printMessage");

  // somewhat longer:
  // pmi::registerClass<HelloWorld>("HelloWorld");
  // pmi::registerMethod<HelloWorld, &HelloWorld::printMessage>("printMessage");

  if (pmi::isController()) {
    // On the controller:
    // Create a parallel instance of the object
    pmi::ParallelObject<HelloWorld> hello;
    
    // Call the method
    hello.invoke<&HelloWorld::printMessage>();

    // Stop the workers
    stopWorkers();
  } else {
    // On the worker:
    // execute the main loop
    mainLoop();
  }
    
  return 0;
}

