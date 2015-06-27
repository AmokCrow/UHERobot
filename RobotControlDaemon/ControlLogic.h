
// By: Johannes Sulkakoski, 2014
// For: UHERobot
// Does: Reads a named pipe and outputs control commands

#ifndef CONTROL_LOGIC__H
#define CONTROL_LOGIC__H


#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>

#include <list>

#include "MessageParser.h"
#include "fcgiServer/fcgiserviceif.h"
#include "fcgiServer/fcgiserver.h"
#include "debuglog.h"
#include "robotboard2interpreter.h"

#include <cstdint>

using namespace JsWebUtils;

class ControlLogic : public FcgiServiceIf
{
public:
  ControlLogic(const char* serialPort);
  virtual ~ControlLogic();

  void run();

  // This function is the callback for messages from the robot control board, via UART
  void msgRxNotification(Base16Message* msg);

  // This function is for the callbacks of the FastCGI server.
  // The return value is a linked list of status values for printing.
  static void notificationCallback(void* obj, Base16Message* msg) { ((ControlLogic*)obj)->msgRxNotification(msg); }

  virtual void serveCall(const std::string& query, const std::string& uri, const char* &response, eResponseType &respType);
  
private:

  void reportError(const char* errorStr);

  bool mShouldRun;

  int serialFileDescriptor;

  MessageParser serialChannel;

  FcgiServer mServer;

  DebugLog mLog;

  RobotBoard2Interpreter mInterpreter;

  volatile unsigned int mReceivedCommand;
};

#endif // CONTROL_LOGIC__H
 
