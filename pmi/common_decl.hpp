#ifndef _PMI_COMMON_DECL_HPP
#define _PMI_COMMON_DECL_HPP

#include "exceptions.hpp"
#include "transmit.hpp"

namespace pmi {
  extern bool workersActive;

  // forward declarations
  template < class T >
  class ParallelClass;

  template < class T, void (T::*method)() >
  class ParallelMethod;
}

#endif
