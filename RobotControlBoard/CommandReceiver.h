
#ifndef COMMAND_RECEIVER__H
#define COMMAND_RECEIVER__H

#include <Arduino.h>
#include "CommandList.h"

class CommandReceiver
{
  
public:
  
  enum eMessageStates
  {
    NO_ACTION = 0,
    MESSAGE_READY = 1,
    MESSAGE_ERROR = -1
  };
  
  CommandReceiver();
  void init();
  char addReceivedChar(char rec);
  
  uint8_t buffLength();
  uint8_t const* getBuffer();
  
private:
  
  enum eInternalStates
  {
    RECEIVING,
    EMPTY,
    HAD_ERROR,
    FINISHED
  };
  
  
  uint8_t mCounter;
  eInternalStates mState;
  uint8_t mBuffer[20];
};

#endif // COMMAND_RECEIVER__H
