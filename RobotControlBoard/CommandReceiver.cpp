
#include "CommandReceiver.h"

CommandReceiver::CommandReceiver()
{
  init();
}
 
void CommandReceiver::init()
{
  mCounter = 0;
  mState = EMPTY;
}

uint8_t CommandReceiver::buffLength()
{
  if(mState == FINISHED)
  {
    return mCounter;
  }
  
  return 0;
}

uint8_t const * CommandReceiver::getBuffer()
{
  return mBuffer;
}

char CommandReceiver::addReceivedChar(char rec)
{
  if(mState != RECEIVING)
  {
    mCounter = 0;
    mState = RECEIVING;
  }
  
  // These will be referenced later
  uint8_t index = mCounter / 2;
  uint8_t odd = mCounter % 2;
  int8_t tmp = 0;
  
  if(rec == END_OF_MESSAGE)
  {
    // An odd count of characters or an empty message are always erroneus
    if(odd || (index == 1))
    {
      mCounter = 0;
      mState = HAD_ERROR;
      return MESSAGE_ERROR;
    }
    
    // There is a valid message. Put it on display with correct length
    mCounter /= 2;
    mState = FINISHED;
    return MESSAGE_READY;
  }
  else if(rec == CLEAR_BUFFER)
  {
    mState = EMPTY;
    mCounter = 0;
    return NO_ACTION;
  }
  else if((rec >= 'A') && (rec <= 'F'))
  {
    tmp = rec - 'A' + 10;
  }
  else if((rec >= '0') && (rec <= '9'))
  {
    tmp = rec - '0';
  }
  else
  {
    mCounter = 0;
    mState = HAD_ERROR;
    return MESSAGE_ERROR;
  }
  
  // To get here, the character received must have been 0-9, A-F 

  // There is an odd number of bytes received. This makes it even.
  if(odd)
  {
    mBuffer[mCounter / 2] |= (uint8_t)tmp;
  }
  else
  {
    mBuffer[mCounter / 2] = (uint8_t)tmp << 4;
  }
  
  return NO_ACTION;
}

