#include "ParallelObject.hpp"

const ObjectIdType &pmi::generateObjectId() {
  static pmi::ObjectIdType nextObjectId = -1;
  nextObjectId++;
  return nextObjectId;
}
