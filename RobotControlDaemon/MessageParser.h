
#ifndef MESSAGE_PARSER__H
#define MESSAGE_PARSER__H

// A clas for communication with an external system via BASE16 coded messages.
// (c) Johannes Sulkakoski, 2014 . Released under GPLv2.
// Some parts copied from Stack Exchange, but I'm not going to point all of them out due to sheel laziness.


#include <stdio.h>      // standard input / output functions
#include <stdlib.h>
#include <string.h>     // string function definitions
#include <unistd.h>     // UNIX standard function definitions
#include <fcntl.h>      // File control definitions
#include <errno.h>      // Error number definitions
#include <termios.h>    // POSIX terminal control definitions
#include <string>


class MessageParser
{
public:
   MessageParser(const char* serialPort);
   void reset();


private:
   void init();
   void deinit();

   // Class variables
   termios mOldTtyDefinitions;
   std::string mPortName;
};


#endif // MESSAGE_PARSER__H

