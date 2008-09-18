#ifndef _PMI_TYPES_HPP
#define _PMI_TYPES_HPP

#include "acconfig.hpp"

#include <boost/serialization/access.hpp>
#include <string>
#include "log4espp.hpp"

LOG4ESPP_DECL_LOGGER(logger);

namespace pmi {

  // define what makes up a command
  typedef enum {
    COMMAND_NOT_DEFINED,
    STOP_WORKERS,
    CREATE,
    INVOKE,
    DESTROY,
    REGISTER_CLASS,
    REGISTER_METHOD,
#ifndef PMI_OPTIMIZE
    ERROR_CLASS_ALREADY_REGISTERED,
    ERROR_CLASS_NOT_REGISTERED,
    ERROR_METHOD_ALREADY_REGISTERED,
    ERROR_METHOD_NOT_REGISTERED,
    ERROR_OBJECT_ALREADY_REGISTERED,
    ERROR_OBJECT_NOT_REGISTERED,
#endif
  } CommandIdType;
  
  typedef int ClassIdType;
  typedef int MethodIdType;
  typedef int ObjectIdType;

#ifndef PMI_OPTIMIZE
  static const ClassIdType CLASS_NOT_REGISTERED = -1;
  static const ClassIdType CLASS_NOT_DEFINED = -2;
  static const MethodIdType METHOD_NOT_REGISTERED = -1;
  static const MethodIdType METHOD_NOT_DEFINED = -2;
  static const ObjectIdType OBJECT_NOT_DEFINED = -2;
#endif

  class CommandType {
  public:
    CommandType();
    CommandIdType commandId;
    ClassIdType classId;
    MethodIdType methodId;
    ObjectIdType objectId;
  private:
    friend class boost::serialization::access;
    
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
      ar & commandId;
      ar & classId;
      ar & methodId;
      ar & objectId;
    }
  };

  std::ostream& 
  operator<<(std::ostream& out, const CommandType &command);

  typedef int WorkerIdType;
  static const int CONTROLLER_ID = 0;
}

#endif
