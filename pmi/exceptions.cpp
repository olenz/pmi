#include "exceptions.hpp"
#include "transmit.hpp"

#include <sstream>

using namespace std;
using namespace pmi;

RegistrationException::
RegistrationException(WorkerIdType _workerId)
  : workerId(_workerId) 
{}

const char*
RegistrationException::getWorkerStr() const {
  if (workerId == CONTROLLER_ID)
    return "Controller: ";
  else {
    ostringstream ost;
    ost << "Worker " << workerId << ": ";
    return ost.str().c_str();
  }
}

const char* 
RegistrationException::what() 
  const throw() {
  ostringstream ost;
  ost << getWorkerStr() << "registration exception";
  return ost.str().c_str();
}

ClassAlreadyRegistered::
ClassAlreadyRegistered(const WorkerIdType _workerId, 
		       const ClassIdType _classId, 
		       const string &_name)
  : RegistrationException(_workerId), 
    classId(_classId), name(_name) 
{}

const char* 
ClassAlreadyRegistered::what() 
  const throw() {
  ostringstream ost;
  ost << getWorkerStr();
  ost << "tried to register class \"" << name << "\", "
      << "that is already registered with classid=" 
      << classId << endl;
  return ost.str().c_str();
}


MethodAlreadyRegistered::
MethodAlreadyRegistered(const WorkerIdType _workerId, 
			const MethodIdType _methodId, 
			const string &_name)
  : RegistrationException(_workerId), 
    methodId(_methodId), name(_name) 
{}

const char* 
MethodAlreadyRegistered::what() 
  const throw() {
  ostringstream ost;
  ost << getWorkerStr()
      << "tried to register method \"" << name << "\""
      << ", that is already registered with methodId=" 
      << methodId << endl;
  return ost.str().c_str();
}

ClassNotRegistered::
ClassNotRegistered(WorkerIdType _workerId)
  : RegistrationException(_workerId)
{}

const char* 
ClassNotRegistered::what() 
  const throw() {
  ostringstream ost;
  ost << getWorkerStr();
  ost << "tried to use a class that is not registered";
  return ost.str().c_str();
}

MethodNotRegistered::
MethodNotRegistered(WorkerIdType _workerId)
  : RegistrationException(_workerId)
{}

const char* 
MethodNotRegistered::what() 
  const throw() {
  ostringstream ost;
  ost << getWorkerStr();
  ost << "tried to use a method that is not registered";
  return ost.str().c_str();
}

const char* 
ControllerReceives::what() 
  const throw() {
  return "controller tried to call receive()";
}

const char*
WorkerCreatesObject::what()
  const throw() {
  return "worker tried to create a parallel object";
}

#ifndef PMI_OPTIMIZE

ControllerSendsControlMessage::
ControllerSendsControlMessage(const string &_message) 
  : message(_message) {}

const char* 
ControllerSendsControlMessage::what() 
  const throw() {
  ostringstream ost;
  ost << "Controller tried to send control message: "
      << message;
  return ost.str().c_str();
}

ControllerSendsControlMessage::
~ControllerSendsControlMessage() throw ()
{}

MismatchedControlMessage::
MismatchedControlMessage(int _rank, const string &_received, 
			 const string &_expected)
  : rank(_rank), received(_received), expected(_expected) {};

const char* 
MismatchedControlMessage::what() 
  const throw() {
  ostringstream ost;
  ost << "Controller received mismatched control message from worker "
      << rank
      << ": expected=" << expected
      << " received=" << received;
  return ost.str().c_str();
}

MismatchedControlMessage::
~MismatchedControlMessage() throw ()
{}

#endif
