#ifndef _PMI_WORKERMAPS_HPP
#define _PMI_WORKERMAPS_HPP

#include "types.hpp"
#include <map>

using namespace std;

namespace pmi {
  typedef void* (*ConstructorCallerType)();
  typedef void (*MethodCallerType)(void*);
  typedef void (*DestructorCallerType)(void*);

  typedef map<ClassIdType, ConstructorCallerType> ConstructorCallerMapType;
  typedef map<MethodIdType, MethodCallerType> MethodCallerMapType;
  typedef map<ClassIdType, DestructorCallerType> DestructorCallerMapType;
  typedef map<ObjectIdType, void*> ObjectMapType;
  
  extern ConstructorCallerMapType constructorCallers;
  extern MethodCallerMapType methodCallers;
  extern DestructorCallerMapType destructorCallers;
  extern ObjectMapType objects;

  template <class T>
  void*
  constructorCallerTemplate() {
    return static_cast<void*>(new T());
  }

  template <class T>
  void
  destructorCallerTemplate(void* voidPtr) {
    T* ptr = static_cast<T*>(voidPtr);
    delete ptr;
  }

  template <class T, void (T::*methodPtr)()>
  void
  methodCallerTemplate(void *voidPtr) {
    T* ptr = static_cast<T*>(voidPtr);
    (ptr->*methodPtr)();
  }
}

#endif
