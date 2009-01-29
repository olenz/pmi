#ifndef _PMI_PARALLELMETHOD_HPP
#define _PMI_PARALLELMETHOD_HPP
#include "pmi/types.hpp"
#include "pmi/transmit.hpp"
#include "pmi/exceptions.hpp"
#include "pmi/ParallelClass.hpp"

#ifdef WORKER
#include "pmi/worker_func.hpp"
#endif

using namespace std;

// macro to register a class
#define PMI_REGISTER_METHOD_SPMD(name, aClass, aMethod, returnType)		\
  template <>								\
  string pmi::ParallelMethod<aClass, returnType, &aClass::aMethod>::MNAME = \
    pmi::ParallelMethod<aClass, returnType, &aClass::aMethod>::registerMethod(name);

#define PMI_REGISTER_METHOD_VOID(name, aClass, aMethod) \
  PMI_REGISTER_METHOD_SPMD(name, aClass, aMethod, void)

// #ifdef WORKER
// #define PMI_REGISTER_METHOD(name, aClass, aMethodController, aMethodWorker, returnType) \
//   PMI_REGISTER_METHOD_SPMD(name, aClass, aMethod, void)
//   #else
// #endif

namespace pmi { 
  IdType generateMethodId();

  template < class T, class returnType, returnType (T::*method)() >
  class ParallelMethod {
  public:
#ifdef CONTROLLER
    // store the name of the method
    static string MNAME;

    // store the Id of the method
    static IdType MID;
#endif

    // register the method
    // this is typically called statically
    static string registerMethod(const string &_name) {
      string name = ParallelClass<T>::getName() + "::" +_name + "()";
#ifdef WORKER
      methodCallersByName()[name] = methodCallerTemplate<T, returnType, method>;
#endif
      return name;
    }

#ifdef CONTROLLER
    static const string &getName() { return MNAME; }
    //    static IdType &getId() { return MID; }
    
    static IdType &associate() {
      if (MID == NOT_ASSOCIATED) {
	MID = generateMethodId();
	
	LOG4ESPP_INFO(logger, "Controller associates method \"" << MNAME << \
		      "\" to method id " << MID << ".");
	transmit::associateMethod(MNAME, MID);
#ifndef PMI_OPTIMIZE
	transmit::gatherStatus();
#endif
      }
      return MID;
    }
#endif
  };

#ifdef CONTROLLER
  // Initialize MID
  template < class T, class returnType, returnType (T::*method)() >
  IdType ParallelMethod<T, returnType, method>::MID = NOT_ASSOCIATED;

  // MNAME is not initialized: it has to be initialized when the method
  // is registered. Use PMI_REGISTER_METHOD for that purpose.

}
#endif

#endif
