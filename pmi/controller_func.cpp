// Implements the broadcast
#include "pmi/controller_func.hpp"
#include "pmi/transmit.hpp"

#include <vector>
#include <iostream>
using namespace std;
using namespace pmi;

bool workersActive = true;

bool 
pmi::isWorkersActive() { return workersActive; }

void 
pmi::endWorkers() {
  if (isWorker()) {
    LOG4ESPP_FATAL(logger, "Worker " << getWorkerId()	\
		   << " tries to end workers!");
    // TODO: throw exception
    // throw exception();
  }
  LOG4ESPP_INFO(logger, "Controller ends all workers.");
  transmit::endWorkers();
  workersActive = false;

  // TODO:
  // #ifndef PMI_OPTIMIZE
  //   broadcaster::stopWorkersConfirm();
  // #endif
}
