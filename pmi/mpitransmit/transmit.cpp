#include "../types.hpp"
#include "../transmit.hpp"
#include "../exceptions.hpp"

#include <vector>
#include <sstream>

#include <boost/mpi.hpp>

using namespace std;
using namespace pmi;
namespace mpi = boost::mpi;

LOG4ESPP_LOGGER(mpilogger, "pmi.mpitransmit");

mpi::communicator world;

bool 
pmi::isController() {
  return getWorkerId() == CONTROLLER_ID;
}

bool 
pmi::isWorker() {
  return !isController();
}

WorkerIdType
pmi::getWorkerId() {
  return world.rank();
}

void
pmi::broadcast(CommandType &command) {
  if (isWorker())
    command = receive();
  else {
    LOG4ESPP_INFO(mpilogger, "Controller broadcasts " << command);
    mpi::broadcast(world, command, 0);
    LOG4ESPP_DEBUG(mpilogger, "Controller finished broadcast.");
  }
}

CommandType
pmi::receive() {
  if (isController())
    throw pmi::ControllerReceives();
  
  CommandType command;
  boost::mpi::broadcast(world, command, CONTROLLER_ID);
  LOG4ESPP_INFO(mpilogger, "Worker " << getWorkerId() << " received " << command);
  return command;
}

#ifndef PMI_OPTIMIZE
void
pmi::gatherControlMessages(const CommandType &expected) {
  if (isWorker()) 
    sendControlMessage(expected);
  else {
    vector<CommandType> allMessages;
    
    LOG4ESPP_INFO(mpilogger, "Controller gathers control messages " << expected);
    // gather messages from all workers
    mpi::gather(world, expected, allMessages, 0);
    LOG4ESPP_DEBUG(mpilogger, "Controller finished gather.");
    
    // compare the messages with the expected message
    for (int i = 0; i < allMessages.size(); i++)
      if (expected.commandId != (allMessages[i]).commandId) {
	std::ostringstream ostReceived, ostExpected;
	ostReceived << allMessages[i];
	ostExpected << expected;
	throw MismatchedControlMessage(i, ostReceived.str(), 
				       ostExpected.str());
      }
  }
}

void
pmi::gatherControlMessages(const string &expected) {
  if (isWorker())
    sendControlMessage(expected);
  else {
    vector<string> allMessages;
    
    
    LOG4ESPP_INFO(mpilogger, "Controller gathers control messages " << expected);
    mpi::gather(world, expected, allMessages, 0);
    LOG4ESPP_DEBUG(mpilogger, "Controller finished gather.");
    
    // compare the messages with the expected message
    for (int i = 0; i < allMessages.size(); i++)
      if (expected != allMessages[i])
	throw MismatchedControlMessage(i, allMessages[i], expected);
  }
}
  

// send finish message to the controller
// should be executed only by workers
void
pmi::sendControlMessage(const CommandType &message) {
  if (isController()) {
    ostringstream ost;
    ost << message;
    throw ControllerSendsControlMessage(ost.str());
  }

  LOG4ESPP_INFO(mpilogger, "Worker " << getWorkerId() << " sends control message " << message);
  mpi::gather(world, message, 0);
}

void
pmi::sendControlMessage(const string &message) {
  if (isController()) throw ControllerSendsControlMessage(message);
  
  LOG4ESPP_INFO(mpilogger, "Worker " << getWorkerId() << " sends control message " << message);
  mpi::gather(world, message, 0);
}

#endif
