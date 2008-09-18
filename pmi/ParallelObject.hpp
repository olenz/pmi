#ifndef _PMI_CONTROLLER_PARALLELINSTANCE_HPP
#define _PMI_CONTROLLER_PARALLELINSTANCE_HPP
// A ParallelObject represents a parallel instance of a class.
// To be a able to use it, the parallel instance has to be first
// registered as a parallel class on the controller as well as on the
// workers.

#include <iostream>
#include <string>

#include "workersActive.hpp"
#include "transmit.hpp"
#include "ParallelClass.hpp"

using namespace pmi;

namespace pmi {
  const ObjectIdType &generateObjectId();

  template < class T >
  class ParallelObject {
    typedef ParallelClass<T> Class;
      
    // the Id of the (parallel) instance
    ObjectIdType objectId;
    // the instance running on the controller
    T *objectPtr;
  public:
    ParallelObject() {
      if (isWorker())
	throw WorkerCreatesObject();

      if (Class::getId() == CLASS_NOT_REGISTERED)
	throw ClassNotRegistered(getWorkerId());
	  
      objectId = generateObjectId();

      CommandType command;
      command.commandId = CREATE;
      command.classId = Class::getId();
      command.objectId = objectId;
#ifndef PMI_OPTIMIZED
      command.methodId = METHOD_NOT_DEFINED;
#endif

      LOG4ESPP_INFO(logger, "Controller creates an instance of class " << command.classId << \
	       ", object id is " << command.objectId << ".");

      // broadcast create command
      broadcast(command);

      // create the local objectPtr 
      objectPtr = new T();

#ifndef PMI_OPTIMIZED
      gatherControlMessages(command);
#endif
    }
      
    template < void (T::*method)() >
    void invoke() {
      CommandType command;
      command.commandId = INVOKE;
      command.classId = Class::getId();
      command.methodId = 
	ParallelMethod<T,method>::getId();
      command.objectId = objectId;
	  
      LOG4ESPP_INFO(logger, "Controller invokes method " << command.methodId\
	       << " in object " << command.objectId\
	       << " of class " << command.classId << ".");
      // broadcast invoke command
      broadcast(command);
	
      // invoke the method in the local objectPtr
      (objectPtr->*method)();

#ifndef PMI_OPTIMIZED
      gatherControlMessages(command);
#endif
    }

    ~ParallelObject() {
      CommandType command;

      LOG4ESPP_INFO(logger, "Controller destroys object " << objectId \
	       << " of class " << Class::getId() << ".");

      if (workersActive) {
	command.commandId = DESTROY;
	command.classId = Class::getId();
	command.objectId = objectId;
#ifndef PMI_OPTIMIZED
	command.methodId = METHOD_NOT_DEFINED;
#endif

	// broadcast destroy command
	broadcast(command);
      } else
	LOG4ESPP_DEBUG(logger, "Controller did not broadcast message, as the workers are stopped.");

      // destroy the local instance
      delete objectPtr;

#ifndef PMI_OPTIMIZED
      if (workersActive)
	gatherControlMessages(command);
#endif
    }
  };
}

#endif
