#ifndef _PMI_PARALLELCLASS_HPP
#define _PMI_PARALLELCLASS_HPP

#include <boost/mpi.hpp>

#include "ParallelMethod.hpp"

using namespace std;
using namespace pmi;

namespace pmi {
  // generate a new (unique) class id
  const ClassIdType &generateClassId();

  // This class registers a parallel class with the Controller
  template < class T >
  class ParallelClass {
    typedef ParallelClass<T> Self;
    static ClassIdType ID;
  public:
    // register the class
    ParallelClass(const string &name) {
      if (ID != CLASS_NOT_REGISTERED)
	throw ClassAlreadyRegistered(CONTROLLER_ID, ID, name);

      ID = generateClassId();

      if (isWorker()) {
	LOG4ESPP_INFO(logger, "Worker " << getWorkerId() << " registers class \"" << ID << "\".");
	// - register constructorCaller with classId
	constructorCallers[ID] = constructorCallerTemplate<T>;
	// - register destructorCaller with classId
	destructorCallers[ID] = destructorCallerTemplate<T>;
      } else {
	LOG4ESPP_INFO(logger, "Controller registers class \"" << ID << "\".");
      }


#ifndef PMI_OPTIMIZE
      CommandType command;
      command.commandId = REGISTER_CLASS;
      command.classId = ID;

      gatherControlMessages(command);
      gatherControlMessages(name);
#endif
    }
    
    template < void (T::*method)() >
    const Self &registerMethod(const string &name) const {
      ParallelMethod<T, method> m(name);
      return *this;
    }

    static const ClassIdType &getId() {
      if (ID == CLASS_NOT_REGISTERED)
	throw ClassNotRegistered(CONTROLLER_ID);
      else return ID;
    }
  };
  
  template <class T>
  static const ParallelClass<T> registerClass(const string &name) {
    return ParallelClass<T>(name);
  }

  // Initialize ID
  template < class T >
  ClassIdType ParallelClass<T>::ID = CLASS_NOT_REGISTERED;

}

#endif
