// Implements the broadcast
#include "controller.hpp"
#include "common_decl.hpp"
#include "workersActive.hpp"

#include <vector>
#include <iostream>
using namespace std;
using namespace pmi;

void 
pmi::stopWorkers() {
  CommandType command;
  command.commandId = STOP_WORKERS;
#ifndef PMI_OPTIMIZE
  command.classId = CLASS_NOT_DEFINED;
  command.methodId = CLASS_NOT_DEFINED;
  command.objectId = CLASS_NOT_DEFINED;
#endif
  workersActive = false;
  broadcast(command);

#ifndef PMI_OPTIMIZE
  gatherControlMessages(command);
#endif
}
