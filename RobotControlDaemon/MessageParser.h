
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
#include <queue>
#include <mutex>

#include "../CommandList.h"

struct RobotMessage
{
    char header;
    char payloadlen;
    char payload[UHEROBOT_BOARD_MSG_MAX_LEN_BYTES];
};

class MessageParser
{
public:
   MessageParser(const char* serialPort);
   ~MessageParser();
   void reset();

   int messagesAvailable();
   bool getMessage(const RobotMessage& msg);
   void sendMessage(const RobotMessage& msg);
   int getErrorStatus(std::string* = NULL);

private:
   void init();
   void deinit();

   void msgProcessingThread();
   void readIncomingData();
   // Decodes the BASE16-encoded message and separated the header for easier consumption
   void decodeMsgToQueue();
   // Returns true if the character was a valid BASE16 half-byte, and stores in buffer
   bool incomingCharacter(char inc);
   void sendOutgoingData();
   // Returns true if there was something in the buffer to decode. False otherwise.
   bool encodeMsgToBuffer();

   void rxError(const char* msg);


   static const int TX_BUFF_LEN = ((UHEROBOT_BOARD_MSG_MAX_LEN_BYTES * 2) + 2) * 5;
   static const int RX_BUFF_LEN = (UHEROBOT_BOARD_MSG_MAX_LEN_BYTES * 2) + 2;
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

   // Serial port needs
   termios mOldTtyDefinitions;
   std::string mPortName;
   int mPortNumber;

   // Messaging
   // RX side
   std::queue<RobotMessage> mRxMsgBuffer;
   std::mutex mRxMsgBuffMutex;
   char mRxBuffer[RX_BUFF_LEN];
   int mRxBufferBytesCounter;

   // TX side
   std::queue<RobotMessage> mTxMsgBuffer;
   std::mutex mTxMsgBuffMutex;
   char mTxBuffer[TX_BUFF_LEN];
   int mTxBufferBytes;
   int mTxBufferPos;
};


#endif // MESSAGE_PARSER__H

