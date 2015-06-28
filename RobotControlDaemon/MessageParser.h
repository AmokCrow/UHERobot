
#ifndef MESSAGE_PARSER__H
#define MESSAGE_PARSER__H

// A class for communication with an external system via BASE16 coded messages.
// (c) Johannes Sulkakoski, 2014 . Released under GPLv2.
// Some parts copied rather verbatim from Stack Exchange (mostly about serial port handling),
//  but I'm not going to dissect it further due to sheer laziness, so just give them some of the credit.


#include <stdio.h>      // standard input / output functions
#include <stdlib.h>
#include <string.h>     // string function definitions
#include <unistd.h>     // UNIX standard function definitions
#include <fcntl.h>      // File control definitions
#include <errno.h>      // Error number definitions
#include <termios.h>    // POSIX terminal control definitions

#include <string>
#include <vector>
// #include <thread>
// #include <mutex>
#include <pthread.h>

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

   static void* statMsgProcessingThread(void* obj) { ((MessageParser*)obj)->msgProcessingThread(); return NULL; }
   void msgProcessingThread();
   void readIncomingData();
   void notifySubscribers();

   void rxError(const char* msg);
   bool resolveTxIssue();

   void verbose(const char* msg);

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
   //std::thread* mRxThreadPtr;
    pthread_t rxThreadM;

   // Serial port needs
   termios mOldTtyDefinitions;
   std::string mPortName;
   int mPortNumber;

   // Message propagation
   std::vector<MsgSubscriber> mSubscriberList;
   Base16Message mMessage;

   // Thread safety bits
   // std::mutex txMutexM;
   pthread_mutex_t txMutexM;
};


#endif // MESSAGE_PARSER__H

