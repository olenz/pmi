#include "ParallelClass.hpp"

namespace pmi {
  const ClassIdType &generateClassId() {
    static ClassIdType nextClassId = -1;
    nextClassId++;
    return nextClassId;
  }
}
