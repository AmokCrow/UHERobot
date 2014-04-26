
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
#include <vector>
#include <thread>
#include <mutex>

#include "../CommandList.h"
#include "base16message.h"

typedef void (*MsgCallbackType)(void*, Base16Message*);

class MessageParser
{

public:

   struct MsgSubscriber
   {
       MsgCallbackType addr;
       void* userDataPtr;
   };

   MessageParser(const char* serialPort);
   ~MessageParser();
   void startThread();
   void stopThread();
   void reset();

   bool sendMessage(Base16Message& msg);
   bool subscribe(MsgCallbackType cbaFunc, void* usrData);

private:
   void init();
   void deinit();

   void msgProcessingThread();
   void readIncomingData();
   void notifySubscribers();

   void rxError(const char* msg);
   bool resolveTxIssue();

   enum eThreadInstruction
   {
       RUN,
       SLEEP,
       END
   };

   // --Class variables--
   // Thread control
   volatile eThreadInstruction mThreadInstruction;
   volatile eThreadInstruction mThreadStatus;
   std::thread* mRxThreadPtr;

   // Serial port needs
   termios mOldTtyDefinitions;
   std::string mPortName;
   int mPortNumber;

   // Message propagation
   std::vector<MsgSubscriber> mSubscriberList;
   Base16Message mMessage;

   // Thread safety bits
   std::mutex txMutexM;
};


#endif // MESSAGE_PARSER__H

