#include "ParallelMethod.hpp"

namespace pmi {
  const ClassIdType &generateMethodId() {
    static MethodIdType nextMethodId = -1;
    nextMethodId++;
    return nextMethodId;
  }
}
