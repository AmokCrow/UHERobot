
#include "MessageParser.h"

MessageParser::MessageParser(const char* serialPort)
   : mPortName(serialPort),
     mRxMsgBuffer(),
     mTxMsgBuffer(),
     mThreadInstruction(END),
     mRxMsgBuffMutex(),
     mTxMsgBuffMutex(),
     mRxBufferBytesCounter(0),
     mTxBufferBytes(0),
     mTxBufferPos(0)
{
   init();
}

void MessageParser::init()
{
   // Opening a file handle the C way. The whole serial interface is in C, I guess.
   // I cound not find documentation for the O_NOCTTY, but I think it has to do with disabling echo and other console features.
   int portnum = open(mPortName.c_str(), O_RDWR | O_NOCTTY);
   
   termios tty;
   memset (&tty, 0, sizeof tty); // TODO: Check if this is really needed, with tcgetattr on the next line.
      
   if (tcgetattr(portnum, &tty) != 0)
   {
      std::cout << "Error " << errno << " from tcgetattr: " << strerror(errno) << std::endl;
      return;
   }
   
   // Save old tty parameters
   mOldTtyDefinitions = tty;
   
   // TODO: Make the interface speed a constructor parameter.
   cfsetospeed (&tty, (speed_t)B9600);
   cfsetispeed (&tty, (speed_t)B9600);
   
   // Port settings to 8n1
   tty.c_cflag     &=  ~PARENB;
   tty.c_cflag     &=  ~CSTOPB;
   tty.c_cflag     &=  ~CSIZE;
   tty.c_cflag     |=  CS8;

   tty.c_cflag     &=  ~CRTSCTS;       // no flow control
   tty.c_cc[VMIN]      =   1;          // read doesn't block
   tty.c_cc[VTIME]     =   5;          // 0.5 seconds read timeout
   tty.c_cflag     |=  CREAD | CLOCAL; // turn on READ & ignore ctrl lines

   // According to Linux Manual this sets partially the same flags as the above. See "man termios" or "man cfmakeraw".
   cfmakeraw(&tty);

   // This flushes the read and write buffers of the port. Original example only flushed the read buffer.
   tcflush(portnum, TCIOFLUSH);

   // Applies (or tries to) the attributes of the structure to the port.
   if(tcsetattr(portnum, TCSANOW, &tty ) != 0)
   {
      cout << "Error " << errno << " from tcsetattr" << endl;
      return;
   }
}

void MessageParser::deinit()
{
    int err = close(mPortNumber);
    if(err != 0)
    {
        std::cout << "Error " << err << " from close():" << strerror(err) << std::endl;
        return;
    }
}

void MessageParser::reset()
{
   // Clear message buffers both ways
    stopThread();

   mRxMsgBuffer.clear();
   mTxMsgBuffer.clear();

   deinit();
   init();

   startThread();
}

int MessageParser::messagesAvailable()
{
    mRxMsgBuffMutex.lock();
    int tmp = mRxMsgBuffer.size();
    mRxMsgBuffMutex.unlock();
    return tmp;
}

bool MessageParser::getMessage(const RobotMessage& msg)
{
    mRxMsgBuffMutex.lock();
    if(mRxMsgBuffer.empty())
    {
        mRxMsgBuffMutex.unlock();
        return false;
    }

    msg = mRxMsgBuffer.front();
    mRxMsgBuffer.pop();
    mRxMsgBuffMutex.unlock();
    return true;
}

void MessageParser::sendMessage(const RobotMessage& msg)
{
    mTxMsgBuffMutex.lock();
    mTxMsgBuffer.push(msg);
    mRxMsgBuffMutex.unlock();
}

int MessageParser::getErrorStatus(std::string* = NULL)
{
    // TODO: implement error reporting
    return 0;
}

void MessageParser::msgProcessingThread()
{
    while(1)
    {
        if(mThreadInstruction == RUN)
        {
            mThreadStatus = RUN;
            readIncomingData();
            sendOutgoingData();
        }
        else if(mThreadInstruction == SLEEP)
        {
            mThreadStatus = SLEEP;
            // Sleep for 100ms (minimum, may sleep a lot longer depending on load)
            usleep(100000L);
        }
        else
        {
            mThreadStatus = END;
            break;
        }

    }
}

void MessageParser::readIncomingData()
{
    char buffer;
    int numRead;
    while(1)
    {
        numRead = read(mPortNumber, &buffer, 1);
        if(numRead == 0)
        {
            break;
        }
        else if(numRead < 0)
        {
            // I don't count this as an RX error yet, since I'm not sure if this happens whenever there are simply no characters to read.
            std::cout << "Error: read() returned less than 0" << std::endl;
            break;
        }
        else if(buffer == eControlChar::CLEAR_BUFFER)
        {
            mRxBufferBytesCounter = 0;
        }
        else if(buffer == eControlChar::END_OF_MESSAGE)
        {
            decodeMsgToQueue();
        }
        else
        {
            if(!incomingCharacter(buffer))
            {
                rxError("Error: Unknown character");
            }
        }
    }
}

// Returns true if the character was a valid BASE16 half-byte, and stores in buffer
bool incomingCharacter(char inc)
{
    char tmp = 0;
    if((inc >= 'A') && (inc <= 'F'))
    {
        tmp = inc - 'A' + 9;
    }
    else if((inc >= '0') && (inc <= '9'))
    {
        tmp = inc - '0';
    }
    else
    {
        // Not a valid BASE16 half-byte
        return false;
    }

    // If first half of a byte
    if((mRxBufferBytesCounter % 2) == 0)
    {
        mRxBuffer[mRxBufferBytesCounter / 2] = tmp;
    }
    // If second half of byte
    else
    {
        mRxBuffer[mRxBufferBytesCounter / 2] |= (tmp << 4);
    }

    mRxBufferBytesCounter++;

    return true;
}


void MessageParser::decodeMsgToQueue()
{
    // An uneven number of half-bytes indicates error
    if((mRxBufferBytesCounter % 2) != 0)
    {
        rxError("Error: Odd number of half-bytes in a message");
        return;
    }

    // An empty message is also an error
    if(mRxBufferBytesCounter <= 0)
    {
        rxError("Error: Too few half-bytes in a message");
        return;
    }

    RobotMessage tmpMsg;
    tmpMsg.header = mRxBuffer[0];
    tmpMsg.payloadlen = (mRxBufferBytesCounter / 2) - 1;

    if(tmpMsg.payloadlen > 0)
    {
        memcpy(tmpMsg.payload, &(mRxBuffer[1]), tmpMsg.payloadlen);
    }
}

void MessageParser::sendOutgoingData()
{
    // Anything in the character buffer to send?
    if(mTxBufferBytes <= 0)
    {
        // Nothing in character buffer, so try to get something from the queue.
        if(encodeMsgToBuffer() == false)
        {
            // Nothing in either the  character buffer of the queue, so just return.
            return;
        }
    }

    int numWritten = write(mPortNumber, &mTxBuffer[mTxBufferPos], mTxBufferBytes);
    mTxBufferBytes -= numWritten;
    mTxBufferPos += numWritten;
}

bool MessageParser::encodeMsgToBuffer()
{

}

MessageParser::~MessageParser()
{
    stopThread();
    deinit();
}

void MessageParser::rxError(const char* msg)
{
    std::cout << msg << std::endl;
    mRxBufferBytesCounter = 0;
}
