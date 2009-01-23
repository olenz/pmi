#ifndef _PMI_BASIC_FUNC_HPP
#define _PMI_BASIC_FUNC_HPP

#include "pmi/types.hpp"
#include "pmi/transmit.hpp"

namespace pmi {
  // test whether the executing process is the controller
  bool isController();
  // test whether the executing process is the worker
  bool isWorker();
  // pretty print the worker id
  std::string printWorkerId();

  // broadcast an object (SPMD version)
  // On the controller: broadcasts the object pointed to by ptr
  // On the worker: receive an object
  template < class T >
  void broadcastParallelObject(T*& ptr);

  // broadcast object obj (call this only on the controller)
  template < class T >
  void broadcastParallelObject(T& obj);

  // receive a parallel object (call this only on a worker)
  template <class T>
  T& receiveParallelObject();

  // receive a parallel object (call this only on the worker)
  template <class T>
  void receiveParallelObject(T* ptr);

}
#endif
