#include "pmi/types.hpp"
#include "pmi/transmit.hpp"
#include "pmi/worker_internal.hpp"

#include <vector>
#include <sstream>

#include <mpi.h>

using namespace std;
using namespace pmi;

LOG4ESPP_LOGGER(mpilogger, "pmi.mpitransmit");

WorkerIdType
pmi::getWorkerId() {
  return MPI::COMM_WORLD.Get_rank();
}


//////////////////////////////////////////////////
// Command definition
//////////////////////////////////////////////////
const unsigned int CMD_ASSOC_CLASS = 0;
const unsigned int CMD_ASSOC_METHOD = 1;
const unsigned int CMD_CREATE = 2;
const unsigned int CMD_INVOKE = 3;
const unsigned int CMD_DESTROY = 4;
const unsigned int CMD_END = 5;


//////////////////////////////////////////////////
// Broadcasts
//////////////////////////////////////////////////
void
broadcastCommand(const unsigned int command, 
		 const IdType classId, 
		 const IdType methodId, 
		 const IdType objectId) {
  unsigned int msg[4];
  msg[0] = command;
  msg[1] = classId;
  msg[2] = methodId;
  msg[3] = objectId;

  LOG4ESPP_INFO(mpilogger, "Controller broadcasts command ("	\
		<< msg[0] << ", "				\
		<< msg[1] << ", "				\
		<< msg[2] << ", "				\
		<< msg[3] << ")."				\
		);
  MPI::COMM_WORLD.Bcast(msg, 4, MPI::UNSIGNED, 0);
  LOG4ESPP_DEBUG(mpilogger, "Controller finished broadcast.");
}

void
broadcastName(const string &name, const unsigned int length) {
  // broadcast the string
  LOG4ESPP_INFO(mpilogger, "Controller broadcasts name \"" << name << "\".");
  MPI::COMM_WORLD.Bcast(const_cast<char*>(name.c_str()), length, MPI::CHAR, 0);
  LOG4ESPP_DEBUG(mpilogger, "Controller finished broadcast.");
}

void 
pmi::transmit::
associateClass(const string &name, const IdType id) {
  unsigned int l = name.length()+1;
  broadcastCommand(CMD_ASSOC_CLASS, id, NOT_DEFINED, l);
  broadcastName(name, l);
}


void 
pmi::transmit::
associateMethod(const string &name, const IdType id) {
  int l = name.length()+1;
  broadcastCommand(CMD_ASSOC_METHOD, NOT_DEFINED, id, l);
  broadcastName(name, l);
}


void 
pmi::transmit::
create(const IdType classId, const IdType objectId) {
  broadcastCommand(CMD_CREATE, classId, NOT_DEFINED, objectId);
}

void 
pmi::transmit::
invoke(const IdType classId, const IdType methodId, 
       const IdType objectId) {
  broadcastCommand(CMD_INVOKE, classId, methodId, objectId);
}

void 
pmi::transmit::
destroy(const IdType classId, const IdType objectId) {
  broadcastCommand(CMD_DESTROY, classId, NOT_DEFINED, objectId);
}

void 
pmi::transmit::
endWorkers() {
  broadcastCommand(CMD_END, NOT_DEFINED, NOT_DEFINED, NOT_DEFINED);
}


//////////////////////////////////////////////////
// Receive
//////////////////////////////////////////////////

const unsigned int*
receiveCommand() {
  static unsigned int msg[4];
  if (isController()) {
    // TODO:
    // throw pmi::ControllerReceives();
    LOG4ESPP_DEBUG(mpilogger, "Controller tries to receive a command.");
  }

  LOG4ESPP_DEBUG(mpilogger, "Worker " << getWorkerId()	\
		 << " waiting to receive a command.");
  
  MPI::COMM_WORLD.Bcast(msg, 4, MPI::UNSIGNED, 0);
  LOG4ESPP_INFO(mpilogger, "Worker " << getWorkerId()			\
		 << " received command ("				\
		 << msg[0] << ", "					\
		 << msg[1] << ", "					\
		 << msg[2] << ", "					\
		 << msg[3] << ")"					\
		 );
  return msg;
}

string
receiveName(const unsigned int length) {
  char s[length];
  LOG4ESPP_DEBUG(mpilogger, "Worker " << getWorkerId()		\
		 << " waiting to receive a name of length "	\
		 << length << ".");
  MPI::COMM_WORLD.Bcast(&s, length, MPI::CHAR, 0);
  string name = s;
  LOG4ESPP_INFO(mpilogger, "Worker " << getWorkerId()		\
		 << " received name \"" << name << "\"");
  return name;
}

bool
pmi::transmit::
handleNext() {
  const unsigned int *msg = receiveCommand();
  static string name;
  
  LOG4ESPP_DEBUG(mpilogger, "Worker " << getWorkerId()			\
		 << " handles command ("				\
		 << msg[0] << ", "					\
		 << msg[1] << ", "					\
		 << msg[2] << ", "					\
		 << msg[3] << ")"					\
		 );

  switch (msg[0]) {
  case CMD_ASSOC_CLASS:
    name = receiveName(msg[3]);
    pmi::worker::associateClass(name, msg[1]);
    break;
  case CMD_ASSOC_METHOD:
    name = receiveName(msg[3]);
    worker::associateMethod(name, msg[2]);
    break;
  case CMD_CREATE:
    worker::create(msg[1], msg[3]);
    break;
  case CMD_INVOKE:
    worker::invoke(msg[1], msg[2], msg[3]);
    break;
  case CMD_DESTROY:
    worker::destroy(msg[1], msg[3]);
    break;
  case CMD_END:
    return false;
  default:
    LOG4ESPP_FATAL(mpilogger, "Worker " << getWorkerId()	\
		   << " cannot handle unknown command code "	\
		   << msg[0] << ".");
    // TODO:
    // internal error (received non-existing command)
    ;
  }

  return true;
}

// #ifndef PMI_OPTIMIZE
// void
// pmi::gatherControlMessages(const CommandType &expected) {
//   if (isWorker()) 
//     sendControlMessage(expected);
//   else {
//     vector<CommandType> allMessages;
    
//     LOG4ESPP_INFO(mpilogger, "Controller gathers control messages " << expected);
//     // gather messages from all workers
//     mpi::gather(world, expected, allMessages, 0);
//     LOG4ESPP_DEBUG(mpilogger, "Controller finished gather.");
    
//     // compare the messages with the expected message
//     for (int i = 0; i < allMessages.size(); i++)
//       if (expected.commandId != (allMessages[i]).commandId) {
// 	std::ostringstream ostReceived, ostExpected;
// 	ostReceived << allMessages[i];
// 	ostExpected << expected;
// 	throw MismatchedControlMessage(i, ostReceived.str(), 
// 				       ostExpected.str());
//       }
//   }
// }

// void
// pmi::gatherControlMessages(const string &expected) {
//   if (isWorker())
//     sendControlMessage(expected);
//   else {
//     vector<string> allMessages;
    
    
//     LOG4ESPP_INFO(mpilogger, "Controller gathers control messages " << expected);
//     mpi::gather(world, expected, allMessages, 0);
//     LOG4ESPP_DEBUG(mpilogger, "Controller finished gather.");
    
//     // compare the messages with the expected message
//     for (int i = 0; i < allMessages.size(); i++)
//       if (expected != allMessages[i])
// 	throw MismatchedControlMessage(i, allMessages[i], expected);
//   }
// }
  

// // send finish message to the controller
// // should be executed only by workers
// void
// pmi::sendControlMessage(const CommandType &message) {
//   if (isController()) {
//     ostringstream ost;
//     ost << message;
//     throw ControllerSendsControlMessage(ost.str());
//   }

//   LOG4ESPP_INFO(mpilogger, "Worker " << getWorkerId() << " sends control message " << message);
//   mpi::gather(world, message, 0);
// }

// void
// pmi::sendControlMessage(const string &message) {
//   if (isController()) throw ControllerSendsControlMessage(message);
  
//   LOG4ESPP_INFO(mpilogger, "Worker " << getWorkerId() << " sends control message " << message);
//   mpi::gather(world, message, 0);
// }

// #endif
