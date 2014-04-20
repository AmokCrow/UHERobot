
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


class ControlLogic
{
public:
  ControlLogic(const char* pipeName, const char* serialPort);

  void run();

  void msgRxNotification(Base16Message* msg);
  static void notificationCallback(void* obj, Base16Message* msg) { ((ControlLogic*)obj)->msgRxNotification(msg); }
  
private:

  bool mShouldRun;
 
  int serialFileDescriptor;
  MessageParser serialChannel;
 
    

};

#endif // CONTROL_LOGIC__H
 
