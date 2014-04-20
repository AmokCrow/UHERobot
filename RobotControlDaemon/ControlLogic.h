
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

#include "MessageParser.h"
#include "fcgiServer/fcgiserviceif.h"
#include "fcgiServer/fcgiserver.h"

using namespace JsWebUtils;

class ControlLogic : public FcgiServiceIf
{
public:
  ControlLogic(const char* serialPort);
  virtual ~ControlLogic();

  void run();

  void msgRxNotification(Base16Message* msg);
  static void notificationCallback(void* obj, Base16Message* msg) { ((ControlLogic*)obj)->msgRxNotification(msg); }

  virtual const FcgiServiceIf::PrintableParam* serveCall(const std::string& query);
  
private:

  bool mShouldRun;

  int serialFileDescriptor;
  MessageParser serialChannel;

  FcgiServer mServer;
  FcgiServiceIf::PrintableParam* webTextList;
  FcgiServiceIf::PrintableParam* pStatusField;

  static const int PARAM_BUFFERS_LENGTH = 20;
  char battVoltageBuff[PARAM_BUFFERS_LENGTH];
  char devCurrentBuff[PARAM_BUFFERS_LENGTH];

};

#endif // CONTROL_LOGIC__H
 
