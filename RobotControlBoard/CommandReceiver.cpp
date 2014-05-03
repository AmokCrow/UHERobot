
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

const uint8_t * CommandReceiver::getBuffer()
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
    if(odd || (index < 1))
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
  mCounter++;

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

void CommandReceiver::writeResponse(uint8_t* dest, uint8_t destLen, const uint8_t* src, uint8_t srcLen)
{
  if(destLen < ((2 * srcLen) + 3))
  {
    srcLen = (destLen - 3) / 2;
  }
  else
  {
    destLen = (2 * srcLen) + 3;
  }
  
  dest[0] = CLEAR_BUFFER;
  
  int i = 0;
  while(i < srcLen)
  {
    char tmp = (src[i] & 0xF0) >> 4;
    
    
    if(tmp > 9)
    {
      tmp += 'A' - 10;
    }
    else
    {
      tmp += '0';
    }
    
    dest[(2 * i) + 1] = tmp;
    
    tmp = (src[i] & 0x0F);
    
    if(tmp > 9)
    {
      tmp += 'A' - 10;
    }
    else
    {
      tmp += '0';
    }
    
    dest[(2 * i) + 2] = tmp;
    
    i++;
  }
  
  dest[(2 * i) + 1] = END_OF_MESSAGE;
  dest[(2 * i) + 2] = 0;
}


