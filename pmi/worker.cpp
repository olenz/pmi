#include "worker.hpp"

#include <boost/mpi.hpp>
#include <iostream>

using namespace std;
using namespace pmi;

namespace pmi {
  void
  createObject(CommandType &command) {
#ifndef PMI_OPTIMIZE
    // check whether constructorCaller is registered
    if (constructorCallers.find(command.classId) == 
	constructorCallers.end()) {
      command.commandId = ERROR_CLASS_NOT_REGISTERED;
      return;
    }

    // check whether object is already registered
    if (objects.find(command.objectId) != objects.end()) {
      command.commandId = ERROR_OBJECT_ALREADY_REGISTERED;
      return;
    }
#endif
    // create object
    ConstructorCallerType constructorCaller
      = constructorCallers[command.classId];
    objects[command.objectId] = 
      (*constructorCaller)();
  }

  void
  invokeMethod(CommandType &command) {
#ifndef PMI_OPTIMIZE
    // check if method is registered
    if (methodCallers.find(command.methodId) == methodCallers.end()) {
      command.commandId = ERROR_METHOD_NOT_REGISTERED;
      return;
    }

    // check if object is registered
    if (objects.find(command.objectId) == objects.end()) {
      command.commandId = ERROR_OBJECT_NOT_REGISTERED;
      return;
    }

    // TODO: check if object is of the right class
#endif
    // call method
    void* voidPtr = objects[command.objectId];
    MethodCallerType methodCaller 
      = methodCallers[command.methodId];
    (*methodCaller)(voidPtr);
  }

  void
  destroyObject(CommandType &command) {
#ifndef PMI_OPTIMIZE
    // check if destructorCaller is registered
    if (destructorCallers.find(command.classId) == 
	destructorCallers.end()) {
      command.commandId = ERROR_CLASS_NOT_REGISTERED;
      return;
    }

    // check if object is registered
    if (objects.find(command.objectId) == objects.end()) {
      command.commandId = ERROR_OBJECT_NOT_REGISTERED;
      return;
    }

    // TODO: check if object is of the right class
#endif
    // delete object
    void* voidPtr = objects[command.objectId];
    DestructorCallerType destructorCaller
      = destructorCallers[command.classId];
    (*destructorCaller)(voidPtr);
    // delete object from objects
    objects.erase(command.objectId);
  }

  bool 
  mainLoop() {
    if (isController()) {
      LOG4ESPP_INFO(logger, "Controller starts and exits main loop.");
      return false;
    }

    LOG4ESPP_INFO(logger, "Worker " << getWorkerId() << " starts main loop.");
    
    CommandType command;
    do {
      command = receive();
      // handle the commands
      // - register commands: error
      
      switch (command.commandId) {
      case CREATE: createObject(command); break;
      case INVOKE: invokeMethod(command); break;
      case DESTROY: destroyObject(command); break;
      default: break;
      }

#ifndef PMI_OPTIMIZE
      sendControlMessage(command);
#endif
    } while (command.commandId != STOP_WORKERS);
    LOG4ESPP_INFO(logger, "Worker " << getWorkerId() << " finished.");
    return true;
  }
}


