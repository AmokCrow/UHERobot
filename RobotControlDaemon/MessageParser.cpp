
#include "MessageParser.h"
#include <iostream>

MessageParser::MessageParser(const char* serialPort)
   : mThreadInstruction(END),
     mThreadStatus(END),
     //mRxThreadPtr(NULL),
     rxThreadM(),
     mPortName(serialPort),
     mSubscriberList(),
     txMutexM()
{
   init();
   pthread_mutex_init(&txMutexM, NULL);
}

void MessageParser::init()
{
   // Opening a file handle the C way. The whole serial interface is in C, I guess.
   // I cound not find documentation for the O_NOCTTY, but I think it has to do with disabling echo and other console features.
   int portnum = open(mPortName.c_str(), O_RDWR | O_NOCTTY);
   mPortNumber = portnum;
   
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
      std::cout << "Error " << errno << " from tcsetattr" << std::endl;
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

   deinit();
   init();

   startThread();
}

void MessageParser::startThread()
{
    /*
    if(mRxThreadPtr == NULL)
    {
        mThreadInstruction = RUN;
        mRxThreadPtr = new std::thread(&MessageParser::msgProcessingThread, this);
    }
    */
    if(pthread_create(&rxThreadM, NULL, statMsgProcessingThread, this) != 0)
    {
        verbose("Error: Could not start thread");
    }
}

void MessageParser::stopThread()
{
    /*
    if(mRxThreadPtr != NULL)
    {
        mThreadInstruction = END;
        mRxThreadPtr->join();
        delete mRxThreadPtr;
        mRxThreadPtr = NULL;
    }
    */

    mThreadInstruction = END;
    pthread_join(rxThreadM, NULL);
}

void MessageParser::msgProcessingThread()
{
    while(1)
    {
        if(mThreadInstruction == RUN)
        {
            mThreadStatus = RUN;
            readIncomingData();
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
    char buffer[20];

    int numRead = read(mPortNumber, buffer, 20);
    int buffPos = 0;
    int numLeft = numRead;

    while(numLeft > 0)
    {
        // Numleft is updated by the method to reflect the remaining undigested data
        if(mMessage.feedRawMsgBuff(&(buffer[buffPos]), numLeft) == Base16Message::FINISHED)
        {
            notifySubscribers();
        }
        buffPos = numRead - numLeft;
    }
}

bool MessageParser::subscribe(MsgCallbackType cbaFunc, void* usrData)
{
    verbose("Someone subscribed");

    MsgSubscriber tmp;
    tmp.addr = cbaFunc;
    tmp.userDataPtr = usrData;
    mSubscriberList.push_back(tmp);
    return true;
}

void MessageParser::notifySubscribers()
{
    verbose("Notifying subscribers");

    for(unsigned int i = 0; i < mSubscriberList.size(); i++)
    {
        mSubscriberList.at(i).addr(mSubscriberList.at(i).userDataPtr, &mMessage);
    }
}

bool MessageParser::sendMessage(Base16Message& msg)
{
    verbose("Sending message");

    // Mutex here, to make message sending thread-safe.
    // One serial port could not be used for multiple simultaneous messages in any case.
    //txMutexM.lock();
    pthread_mutex_lock(&txMutexM);

    const char* buffPtr = msg.encodedBytesPtr();
    int numBytes = msg.encodedLength();

    while(numBytes > 0)
    {
        int numWritten = write(mPortNumber, buffPtr, numBytes);
        buffPtr += numWritten;
        numBytes -= numWritten;

        if(numWritten <= 0)
        {
            if(resolveTxIssue() == false)
            {
                return false;
            }

        }
    }

    //txMutexM.unlock();
    pthread_mutex_unlock(&txMutexM);

    return true;
}

bool MessageParser::resolveTxIssue()
{
    rxError("Transmission failure");
    return true;
}

MessageParser::~MessageParser()
{
    stopThread();
    deinit();
}

void MessageParser::rxError(const char* msg)
{
    std::cout << "MessageParser: Error: " << msg << std::endl;
}

void MessageParser::verbose(const char* msg)
{
    std::cout << "MessageParser: " << msg << std::endl;
}

