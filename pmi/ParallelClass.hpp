#ifndef _PMI_PARALLELCLASS_HPP
#define _PMI_PARALLELCLASS_HPP
#include "pmi/types.hpp"
#include "pmi/transmit.hpp"
#include "pmi/exceptions.hpp"
#include "pmi/controller_func.hpp"

#ifdef WORKER
#include "pmi/worker_func.hpp"
#endif

#include <iostream>

using namespace std;

// macro to register a class
#define PMI_REGISTER_CLASS(name, aClass)					\
  template <>								\
  string pmi::ParallelClass<aClass>::CNAME =				\
    pmi::ParallelClass<aClass>::registerClass(name);

// macro to define proxy methods
#define PMI_PARALLEL_PROXY_METHOD(method, returnType)				\
  returnType method() {							\
    return pmi::ParallelClass<SubjectClass>::invoke<returnType, &SubjectClass::method>(); \
  }

#define PMI_PARALLEL_PROXY_METHOD_VOID(method)				\
  void method() {							\
    pmi::ParallelClass<SubjectClass>::invokeVoid<&SubjectClass::method>(); \
  }
  
namespace pmi {
  template < class T, class returnType, returnType (T::*method)() >
  class ParallelMethod;

  IdType generateClassId();
  IdType generateObjectId();
  void freeObjectId(const IdType id);

  // This class registers a parallel class with the Controller
  template < class T >
  class ParallelClass {
  public:
    typedef T SubjectClass;

#ifdef CONTROLLER
    // store the name of the class
    static string CNAME;

    // store the Id of the class
    static IdType CID;
#endif

    // store the Id of the object instance
    IdType OID;
    // the instance running on the controller
    SubjectClass *objectPtr;

  public:
    // register the class
    static const string &registerClass(const string &name) {
#ifdef WORKER
      // register constructorCaller with name
      constructorCallersByName()[name] = constructorCallerTemplate<T>;
      // register destructorCaller with name
      destructorCallersByName()[name] = destructorCallerTemplate<T>;
#endif
      return name;
    }

#ifdef CONTROLLER
    static const string &getName() { return CNAME; }
#endif

    // The constructor of the class: create an instance of the parallel object
    ParallelClass() {
      if (isWorker())
	PMI_USER_ERROR(printWorkerId()					\
		       << "tries to create a parallel object of type \"" \
		       << CNAME << "\".");
      if (!isWorkersActive())
	PMI_USER_ERROR("Controller tries to create a parallel object of type \"" \
		       << "\", but the workers have been terminated.");

      // Associate the class Id if not already done
      if (CID == NOT_ASSOCIATED) {
	// associate class with ID
	CID = generateClassId();
	
	LOG4ESPP_INFO(logger, "Controller associates class \"" << CNAME << \
		      "\" with class id " << CID << ".");
	transmit::associateClass(CNAME, CID);
#ifndef PMI_OPTIMIZE
	transmit::gatherStatus();
#endif
      }

      // Generate the object ID
      OID = generateObjectId();

      LOG4ESPP_INFO(logger, "Controller creates an instance of class \"" \
		    << CNAME << "\" (class id "				\
		    << CID << "), object id is " << OID << ".");
      // Broadcast creation command
      transmit::create(CID, OID);

      // Create the local instance
      objectPtr = new SubjectClass();

#ifndef PMI_OPTIMIZE
      transmit::gatherStatus();
#endif
    }

    SubjectClass &getLocalInstance() {
      return *objectPtr;
    }

    template < class returnType, returnType (T::*method)() >
    returnType invoke() {
      const string &MNAME 
	= ParallelMethod<T, returnType, method>::getName();
      
      if (isWorker())
	PMI_USER_ERROR(printWorkerId()					\
		       << "tries to invoke method \""			\
		       << MNAME					\
		       << "\" of parallel object id " << OID		\
		       << " of class \"" << CNAME			\
		       << "\".");

      if (!isWorkersActive())
	PMI_USER_ERROR("Controller tries to invoke method \""		\
		       << MNAME					\
		       << "\" of parallel object id " << OID		\
		       << " of class \"" << CNAME			\
		       << "\", but the workers have been terminated.");

      // associate method (if not already done)
      IdType MID =
	ParallelMethod<T, returnType, method>::associate();

      LOG4ESPP_INFO(logger, "Controller invokes method \""		\
		    << MNAME					\
		    << "\" (method id " << MID				\
		    << ") of object id " << OID				\
		    << " of class \"" << CNAME				\
		    << "\" (class id " << CID << ").");
      // broadcast invoke command
      transmit::invoke(CID, MID, OID);
      
      // invoke the method in the local objectPtr
      returnType returnValue = (objectPtr->*method)();

#ifndef PMI_OPTIMIZE
      transmit::gatherStatus();
#endif

      return returnValue;
    }

    template < void (T::*method)() >
    void invokeVoid() {
      const string &MNAME 
	= ParallelMethod<T, void, method>::getName();
      
      if (isWorker())
	PMI_USER_ERROR(printWorkerId()					\
		       << "tries to invoke (void) method \""		\
		       << MNAME						\
		       << "\" of parallel object id " << OID		\
		       << " of class \"" << CNAME			\
		       << "\".");

      if (!isWorkersActive())
	PMI_USER_ERROR("Controller tries to invoke (void) method \""	\
		       << MNAME						\
		       << "\" of parallel object id " << OID		\
		       << " of class \"" << CNAME			\
		       << "\", but the workers have been terminated.");

      IdType MID =
	ParallelMethod<T, void, method>::associate();

      LOG4ESPP_INFO(logger, "Controller invokes method \""		\
		    << MNAME					\
		    << "\" (method id " << MID				\
		    << ") of object id " << OID				\
		    << " of class \"" << CNAME				\
		    << "\" (class id " << CID << ").");
      // broadcast invoke command
      transmit::invoke(CID, MID, OID);
      
      // invoke the method in the local objectPtr
      (objectPtr->*method)();

#ifndef PMI_OPTIMIZE
      transmit::gatherStatus();
#endif
    }

    ~ParallelClass() {
      if (isWorker())
	PMI_USER_ERROR(printWorkerId()					\
		       << "tries to destroy parallel object id " << OID	\
		       << " of class \"" << CNAME			\
		       << "\".");

      if (isWorkersActive()) {
	LOG4ESPP_INFO(logger, "Controller destroys object id " << OID	\
		      << " of class \"" << CNAME			\
		      << "\" (class id " << CID << ").");
	transmit::destroy(CID, OID);
      } else {
	LOG4ESPP_DEBUG(logger, "Controller did not broadcast destroy message, as the workers are stopped.");
      }

      // destroy the local instance
      delete objectPtr;
      // free the objectId
      freeObjectId(OID);

#ifndef PMI_OPTIMIZE
      if (isWorkersActive()) {
	transmit::gatherStatus();
      } else {
	LOG4ESPP_DEBUG(logger, "Controller did not gather status after destroy, as the workers are stopped.");
      }
#endif
    }
  };

#ifdef CONTROLLER
  // Initialize class ID
  template < class T >
  IdType ParallelClass<T>::CID = NOT_ASSOCIATED;
  // CNAME is not initialized: it has to be initialized when the class
  // is registered. Use PMI_REGISTER_CLASS for that purpose.
#endif
}

#endif
