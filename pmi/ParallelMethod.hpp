#ifndef _PMI_PARALLELMETHOD_HPP
#define _PMI_PARALLELMETHOD_HPP

#include "common_decl.hpp"
#include "transmit.hpp"
#include "workerMaps.hpp"

using namespace std;
using namespace pmi;
namespace pmi {
  // forward declaration of the ParallelClass template
  template < class T >
  class ParallelClass;

  const MethodIdType &generateMethodId();

  template < class T, void (T::*method)() >
  class ParallelMethod {
    typedef ParallelClass<T> Class;
    static MethodIdType ID;

  public:
    ParallelMethod(bool withName = false) {
      if (ID != METHOD_NOT_REGISTERED) 
	throw MethodAlreadyRegistered(CONTROLLER_ID, ID);

    }
    
    ParallelMethod(const string &name) {
      if (ID != METHOD_NOT_REGISTERED) 
	throw MethodAlreadyRegistered(CONTROLLER_ID, ID, name);

      ID = generateMethodId();

//       if (isController())
// 	cerr << "Controller: ";
//       else 
// 	cerr << "Worker " << getWorkerId() << ": ";
//       cerr << "Registering method \""
// 	   << ID
// 	   << "\" with class \""
// 	   << Class::getId()
// 	   << "\"."
// 	   << endl;

      if (isWorker()) {
	LOG4ESPP_INFO(logger, "Worker " << getWorkerId() << " registers method \"" << ID << "\".");
	// - register methodCaller with methodId
	methodCallers[ID] = methodCallerTemplate<T, method>;
      } else {
	LOG4ESPP_INFO(logger, "Controller registers method \"" << ID << "\".");
      }

#ifndef PMI_OPTIMIZE
      CommandType command;
      command.commandId = REGISTER_METHOD;
      command.classId = Class::getId();
      command.methodId = ID;

      gatherControlMessages(command);
      gatherControlMessages(name);
#endif
    }

    static const MethodIdType &getId() {
      if (ID == METHOD_NOT_REGISTERED)
	throw MethodNotRegistered(CONTROLLER_ID);
      else return ID;
    }
  };

  template <class T, void (T::*method)() >
  static void registerClass(const string &name) {
    ParallelMethod<T, method> m(name);
  }

  // Initialize ID
  template < class T, void (T::*method)() >
  MethodIdType 
  ParallelMethod<T, method>::ID = METHOD_NOT_REGISTERED;
}

#endif
