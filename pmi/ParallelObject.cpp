#include "pmi/ParallelObject.hpp"

#ifdef CONTROLLER

#include <set>

using namespace std;

set<IdType> freeObjectIds;

pmi::IdType 
pmi::generateObjectId() {
  static IdType nextObjectId = 0;
  if (freeObjectIds.empty()) {
    nextObjectId++;
    return nextObjectId - 1;
  } else {
    IdType id = *freeObjectIds.begin();
    freeObjectIds.erase(id);
    return id;
  }
}

void 
pmi::freeObjectId(const IdType id) {
  // TODO: Test whether the id is already free
  freeObjectIds.insert(id);
}
#endif
