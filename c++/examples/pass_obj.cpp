// Basic example for passing PMI object (references) via PMI.
//
// Demonstrates, how a method can be passed a reference to another PMI
// object.
//
// Execute via:
//
//   > mpiexec -n 2 pass_obj

#include <iostream>
#include <pmi/pmi.hpp>
#include <logging/log4espp.hpp>
#include <mpi.h>

using namespace std;

// Simple class that "computes a message" (actually, it only returns
// the MPI thread, but the principle is sound, eh?). The class does
// not have any parallel methods by itseld, but as it should be passed
// to the class B, it does have to be registered.
class A {
public:
  pmi::ParallelClass<A> pmiObject;

  unsigned short computeMessage() { 
    return pmi::getWorkerId();
  }
};

PMI_REGISTER_CLASS(A);

// This class has an A class member. As A and B are parallel classes
class B 
{
  pmi::ParallelClass<B> pmiObject;

  A *a;

public:
  B() { a = 0; }

  void setA(A& _a) {
    // invoke setAWorker in parallel
    pmiObject.invoke<&B::setAWorker>();

    // broadcast object "a"
    pmi::broadcastPMIObject(_a.pmiObject);

    // set the local copy
    a = &_a;
  }

  void setAWorker() {
    // receive object "a"
    a = pmi::receivePMIObjectPtr<A>();

    // MPI::bcast(&oid);
    // a = getObjectPtr<A>(oid);
  }

  std::string getMessage() {
    // invoke the parallel method
    pmiObject.invoke<&B::getMessageWorker>();

    // compute the message
    unsigned short myMsg = getMessageLocal();

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

  void getMessageWorker() {
    // compute the message
    unsigned short myMsg = getMessageLocal();

    // send the message to the controller
    MPI::COMM_WORLD.Gather(&myMsg, 1, MPI::UNSIGNED_SHORT, 
			   0, 0, MPI::UNSIGNED_SHORT,
			   pmi::getControllerMPIRank());
  }

  short unsigned getMessageLocal() {
    return a->computeMessage();
  }
};

PMI_REGISTER_CLASS(B);
PMI_REGISTER_METHOD(B, setAWorker);
PMI_REGISTER_METHOD(B, getMessageWorker);

int main(int argc, char* argv[]) {
  // Required by MPI
  MPI::Init(argc, argv);
  // Required by the logging system
  LOG4ESPP_CONFIGURE();

  if (pmi::isWorker()) pmi::mainLoop();
  else {
    A a;
    B b;
    
    b.setA(a);
    cout << b.getMessage() << endl;
    
    // Stop the workers
    pmi::endWorkers();
  } 


  MPI::Finalize();
  return 0;
}
