#include "workerMaps.hpp"

using namespace std;

namespace pmi {
  ConstructorCallerMapType constructorCallers =
    ConstructorCallerMapType();
  DestructorCallerMapType destructorCallers =
    DestructorCallerMapType();
  MethodCallerMapType methodCallers =
    MethodCallerMapType();
  ObjectMapType objects =
    ObjectMapType();
}


