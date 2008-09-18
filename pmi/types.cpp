#include "types.hpp"

#include <iostream>

LOG4ESPP_LOGGER(logger, "pmi");

pmi::CommandType::CommandType() 
#ifndef PMI_OPTIMIZE
  : commandId(COMMAND_NOT_DEFINED), 
    classId(CLASS_NOT_DEFINED),
    methodId(METHOD_NOT_DEFINED),
    objectId(OBJECT_NOT_DEFINED)
#endif
{}

std::ostream& 
pmi::operator<<(std::ostream& out, 
		const CommandType &command) {
  switch (command.commandId) {
  case COMMAND_NOT_DEFINED: 
    out << "COMMAND_NOT_DEFINED ("; 
    break;
  case STOP_WORKERS: 
    out << "STOP_WORKERS ("; 
    break;
  case CREATE: 
    out << "CREATE ("; 
    break;
  case INVOKE: 
    out << "INVOKE ("; 
    break;
  case DESTROY: 
    out << "DESTROY ("; 
    break;
#ifndef PMI_OPTIMIZE
  case REGISTER_CLASS: 
    out << "REGISTER_CLASS ("; 
    break;
  case REGISTER_METHOD: 
    out << "REGISTER_METHOD ("; 
    break;
  case ERROR_CLASS_ALREADY_REGISTERED:
    out << "ERROR_CLASS_ALREADY_REGISTERED (";
    break;
  case ERROR_CLASS_NOT_REGISTERED:
    out << "ERROR_CLASS_NOT_REGISTERED (";
    break;
  case ERROR_METHOD_ALREADY_REGISTERED:
    out << "ERROR_METHOD_ALREADY_REGISTERED (";
    break;
  case ERROR_METHOD_NOT_REGISTERED:
    out << "ERROR_METHOD_NOT_REGISTERED (";
    break;
  case ERROR_OBJECT_ALREADY_REGISTERED:
    out << "ERROR_OBJECT_ALREADY_REGISTERED (";
    break;
  case ERROR_OBJECT_NOT_REGISTERED:
    out << "ERROR_OBJECT_NOT_REGISTERED (";
    break;
#endif
  default:
    out << "!UNKNOWN_COMMAND! (";
  }
  if (command.classId != CLASS_NOT_DEFINED)
    out << " classId=" << command.classId;
  if (command.methodId != METHOD_NOT_DEFINED)
    out << " methodId=" << command.methodId;
  if (command.objectId != OBJECT_NOT_DEFINED)
    out << " objectId=" << command.objectId;
  out << " )";
  return out;
}
