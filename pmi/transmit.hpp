#ifndef _PMI_TRANSMIT_HPP
#define _PMI_TRANSMIT_HPP

#include "types.hpp"
#include <string>
#include <iostream>
#include <sstream>

namespace pmi {
  // test whether the executing process is the controller
  bool isController();
  // test whether the executing process is the worker
  bool isWorker();
  // get the workerId of the executing process
  // corresponds to the MPI rank
  // 0 is the controller
  WorkerIdType getWorkerId();

  // broadcast the next command to all workers
  // when executed by a worker, calls command = receive()
  void broadcast(CommandType &command);
    
  // receive the next command from the controller
  // should be executed only by workers
  CommandType receive();
    
#ifndef PMI_OPTIMIZE
  // gather finish messages from the workers
  // when executed by a worker, calls sendControlMessage()
  void gatherControlMessages(const CommandType &expected);
  void gatherControlMessages(const std::string &expected);
    
  // send finish message to the controller
  // should be executed only by workers
  void sendControlMessage(const CommandType &message);
  void sendControlMessage(const std::string &message);
#endif
}

#endif
