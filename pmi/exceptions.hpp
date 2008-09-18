#ifndef _PMI_EXCEPTIONS_HPP
#define _PMI_EXCEPTIONS_HPP
#include "types.hpp"

#include <exception>
#include <string>

using namespace std;
namespace pmi {
  class PMIException : public exception {};

  //////////////////////////////
  // Registration exceptions
  //////////////////////////////

  class RegistrationException : public PMIException {
  public:
    WorkerIdType &workerId;
    RegistrationException(WorkerIdType _workerId);
    const char* getWorkerStr() const;
    virtual const char* what() const throw();
  };

  // the class classId was already registered on worker workerId
  class ClassAlreadyRegistered : public RegistrationException {
  public:
    const ClassIdType &classId;
    const string &name;
    ClassAlreadyRegistered(const WorkerIdType _workerId, 
			   const ClassIdType _classId, 
			   const string &name);
    virtual const char* what() const throw();
  };

  class MethodAlreadyRegistered : public RegistrationException {
  public:
    const MethodIdType &methodId;
    const string &name;
    MethodAlreadyRegistered(const WorkerIdType _workerId, 
			    const MethodIdType _methodId, 
			    const string &name);
    virtual const char* what() const throw();
  };

  class ObjectAlreadyRegistered : public RegistrationException {
  public:
    const ObjectIdType &objectId;
    ObjectAlreadyRegistered(const WorkerIdType _workerId, 
			    const MethodIdType _methodId);
    virtual const char* what() const throw();
  };

  class ClassNotRegistered : public RegistrationException {
  public:
    ClassNotRegistered(WorkerIdType _workerId);
    virtual const char* what() const throw();
  };

  class MethodNotRegistered : public RegistrationException {
  public:
    MethodNotRegistered(WorkerIdType _workerId);
    virtual const char* what() const throw();
  };

  //////////////////////////////
  // Execution exceptions
  //////////////////////////////
  class ControllerReceives : public PMIException {
    virtual const char* what() const throw();
  };

  class WorkerCreatesObject : public PMIException {
    virtual const char* what() const throw();
  };

#ifndef PMI_OPTIMIZE
  class ControllerSendsControlMessage : public PMIException {
  public:
    string message;
    ControllerSendsControlMessage(const string &_message);
    virtual const char* what() const throw();
    virtual ~ControllerSendsControlMessage() throw ();
  };

  //////////////////////////////
  // Mismatched control message exception
  //////////////////////////////
  // A worker received a mismatched control message
  class MismatchedControlMessage : public PMIException {
  public:
    int rank;
    string received;
    string expected;
    MismatchedControlMessage(int _rank, const string &_received, const string &_expected);
    virtual const char* what() const throw();
    virtual ~MismatchedControlMessage() throw ();
  };
#endif

}
#endif
