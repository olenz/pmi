#ifndef _PMI_PARALLELOBJECT_HPP
#define _PMI_PARALLELOBJECT_HPP
// A ParallelObject represents a parallel instance of a class.
// To be a able to use it, the parallel instance has to be first
// registered as a parallel class on the controller as well as on the
// workers.

#include "pmi/types.hpp"
#include "pmi/controller.hpp"
#include "pmi/ParallelClass.hpp"
#include "pmi/ParallelMethod.hpp"

#ifdef CONTROLLER
using namespace pmi;

// macro to define proxy methods
#define PMI_PROXY_METHOD(method)					\
  void method() {							\
    ParallelObject<SubjectClass>::invoke<&SubjectClass::method>();	\
  }

namespace pmi {
  IdType generateObjectId();
  void freeObjectId(const IdType id);

  template < class T >
  class ParallelObject {
  public:
    typedef ParallelClass<T> PClass;
    typedef T SubjectClass;

  private:
    // the Id of the (parallel) instance
    IdType ID;
    // the instance running on the controller
    SubjectClass *objectPtr;

  public:
    ParallelObject() {
      if (isWorker()) {
	LOG4ESPP_FATAL(logger, printWorkerId()				\
		       << " tries to create an object of type \""	\
		       << PClass::getName() << "\".");
	// TODO: throw WorkerCreatesObject(PClass::NAME);
	// throw exception("Worker creates object");
      }
      
      IdType classId = PClass::associate();

      // generate the object ID
      ID = generateObjectId();

      LOG4ESPP_INFO(logger, "Controller creates an instance of class \"" \
		    << PClass::getName() << "\" (class id " 		\
		    << PClass::getId() << "), object id is " << ID << ".");
      // broadcast invoke command
      transmit::create(classId, ID);

      // create the local instance
      objectPtr = new T();

      // TODO:
      // #ifndef PMI_OPTIMIZE
      //       transmit::checkResults();
      // #endif
    }
      
    template < void (T::*method)() >
    void invoke() {
      if (isWorker()) {
	LOG4ESPP_FATAL(logger,						\
		       printWorkerId()					\
		       << " tries to invoke method \""			\
		       << (ParallelMethod<T,method>::getName())		\
		       << "\" of object id " << ID			\
		       << " of class \"" << PClass::getName() << "\".");
	// TODO: throw WorkerInvokesMethod(PClass::NAME, ParallelMethod<T, method>::NAME, ID);
	// throw exception("Worker invokes method");
      }

      IdType methodId =
	ParallelMethod<T,method>::associate();

      IdType classId = PClass::getId();
      LOG4ESPP_INFO(logger, "Controller invokes method \""		\
		    << (ParallelMethod<T,method>::getName())		\
		    << "\" (method id " << methodId			\
		    << ") of object id " << ID				\
		    << " of class \"" << PClass::getName()		\
		    << "\" (class id " << classId << ").");
      // broadcast invoke command
      transmit::invoke(classId, methodId, ID);

      // invoke the method in the local objectPtr
      (objectPtr->*method)();

      // TODO:
      // #ifndef PMI_OPTIMIZED
      //       transmit::invokeConfirm(classId, methodId, ID);
      // #endif
    }

    ~ParallelObject() {
      if (isWorker()) {
	LOG4ESPP_FATAL(logger, "Worker " << getWorkerId()		\
		       << " tries to destroy object id " << ID		\
		       << " of class \"" << PClass::getName()		\
		       << "\".");
	// TODO: throw WorkerDestroysObject(PClass::NAME, ID);
	// throw exception("Worker destroys object");
      }

      if (isWorkersActive()) {
	IdType classId = PClass::getId();
	LOG4ESPP_INFO(logger, "Controller destroys object id " << ID	\
		      << " of class \"" << PClass::getName()		\
		      << "\" (class id " << classId << ").");
	transmit::destroy(classId, ID);
      } else {
	LOG4ESPP_DEBUG(logger, "Controller did not broadcast message, as the workers are stopped.");
      }

      // destroy the local instance
      delete objectPtr;
      // free the objectId
      freeObjectId(ID);

      // TODO:
      // #ifndef PMI_OPTIMIZED
      //       if (isWorkersActive())
      // 	transmit::destroyConfirm(classId, ID);
      // #endif
    }
  };
}

#endif /* CONTROLLER */
#endif /* _PMI_PARALLELOBJECT_HPP */
