#include "pmi/worker_internal.hpp"

#ifdef WORKER

#include <vector>
#include "pmi/basic_func.hpp"
#include "pmi/worker_func.hpp"

using namespace pmi;
using namespace std;

namespace pmi {
  namespace worker {
    vector<ConstructorCallerType> constructorCallers;
    vector<MethodCallerType> methodCallers;
    vector<DestructorCallerType> destructorCallers;
    vector<void*> objects;

    void associateClass(const string &name, const IdType id) {
#ifndef PMI_OPTIMIZE
      // TODO:
      // check whether a Constructor and Destructor exists for the given name
      // otherwise: user error (class not registered)

#endif
      LOG4ESPP_INFO(logger, printWorkerId()			\
		    << "associates class \""			\
		    << name << "\" to class id " << id << ".");

      ConstructorCallerType cc = constructorCallersByName()[name];
      constructorCallers.push_back(cc);
      constructorCallersByName().erase(name);
      DestructorCallerType dc = destructorCallersByName()[name];
      destructorCallers.push_back(dc); 
      destructorCallersByName().erase(name);
#ifndef PMI_OPTIMIZE
      // TODO:
      // check whether the id is identical to the size of the both maps
      // otherwise: internal error (expected id, but got id)
#endif
    }

    void associateMethod(const string &name, const IdType id) {
#ifndef PMI_OPTIMIZE
      // TODO:
      // check whether a methodCaller exists for the given name
      // otherwise: user error (method not registered)
#endif
      LOG4ESPP_INFO(logger, printWorkerId()			\
		    << "associates method \""			\
		    << name << "\" to method id " << id << ".");

      MethodCallerType mc = methodCallersByName()[name];
      methodCallers.push_back(mc);
      methodCallersByName().erase(name);
#ifndef PMI_OPTIMIZE
      // check whether the vector has the right size
      if (id != methodCallers.size()-1) {
	// TODO:
	// check whether the id is identical to the size of the both maps
	// internal error (Id error)
      }
      if (methodCallers[id] != mc) {
	// otherwise: internal error (expected id but got id)
      }
#endif
    }
    
    void create(const IdType classId, 
		const IdType objectId) {
#ifndef PMI_OPTIMIZE
      // check whether the class has an associated constructorCaller
      if (classId >= constructorCallers.size())
	PMI_INT_ERROR(printWorkerId()					\
		      << "does not have a constructorCaller for class id " \
		      << classId << ".");
      // check whether the object is already defined
      if (objectId < objects.size() && objects[objectId] != NULL)
	PMI_INT_ERROR(printWorkerId()				\
		      << "has object id "			\
		      << objectId << " already defined.");
      else if (objectId >= objects.size()+1)
	PMI_INT_ERROR(printWorkerId()				 \
		      << ": object id " << objectId		 \
		      << " overshoots (size " << objects.size()	 \
		      << ").");
#endif
      LOG4ESPP_INFO(logger, printWorkerId()				\
		    << "creates an instance of class id "		\
		    << classId << ", object id is " << objectId << ".");

      // create object
      ConstructorCallerType cc = constructorCallers[classId];
      if (objectId == objects.size())
	objects.push_back((*cc)());
      else
	objects[objectId] = (*cc)();
    }
    
    void invoke(const IdType classId, 
		const IdType methodId, 
		const IdType objectId) {
#ifndef PMI_OPTIMIZE
      // check whether the method has an associated methodCaller
      if (methodId >= methodCallers.size()) {
	// TODO: internal error (method not associated)
      }
      // check if the object is defined
      if (objectId >= objects.size() || objects[objectId] == NULL) {
	// TODO: internal error (object not initialized)
      }
      
      // TODO: check if object is of the right class
#endif
      // call method
      LOG4ESPP_INFO(logger, printWorkerId()				\
		    << "invokes method id " << methodId			\
		    << " of object id " << objectId			\
		    << " of class id " << classId << ".");

      void* voidPtr = objects[objectId];
      MethodCallerType mc = methodCallers[methodId];
      (*mc)(voidPtr);
    }
    
    void 
    destroy(const IdType classId,
	    const IdType objectId) {
#ifndef PMI_OPTIMIZE
      // check whether the class has an associated constructorCaller
      if (classId >= destructorCallers.size()) {
	// TODO: internal error (destructor not associated)
      }
      // check if the object is defined
      if (objectId >= objects.size() || objects[objectId] == NULL) {
	// TODO: internal error (object not initialized)
      }

      // TODO: check if object is of the right class
#endif
      // delete object
      LOG4ESPP_INFO(logger, printWorkerId()				\
		    << "destroys object id " << objectId		\
		    << " of class id " << classId << ".");

      void* voidPtr = objects[objectId];
      DestructorCallerType dc = destructorCallers[classId];
      (*dc)(voidPtr);
      objects[objectId] = NULL;
    }
  }
}
#endif /* WORKER */
