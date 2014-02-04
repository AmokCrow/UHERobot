
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



class ControlLogic
{
public:
  ControlLogic(const char const* pipeName, const char const* serialPort);
  
private:
 
int serialFileDescriptor;
 
    

};

#endif // CONTROL_LOGIC__H
 
