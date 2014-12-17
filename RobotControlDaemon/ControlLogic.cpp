
#include "ControlLogic.h"

#include "../CommandList.h"
#include "base16message.h"

#include <iostream>
#include <cstdio>

using namespace JsWebUtils;

ControlLogic::ControlLogic(const char* serialPort)
    : serialChannel(serialPort)
    , mServer(this)
    , mLog(50)
    , mInterpreter()
    , mReceivedCommand(0)
{


}

ControlLogic::~ControlLogic()
{

}

void ControlLogic::run()
{
    std::cout << "Starting" << std::endl;
    serialChannel.subscribe(&ControlLogic::notificationCallback, (void*)this);
    serialChannel.startThread();

    std::cout << "Serial thread started" << std::endl;

    int roundCounter = 0;
    unsigned char tmpBuff[50];
    Base16Message message;
    unsigned int msgLength;

    //message.setBody(tmpBuff, 5);
    //message.encode();

    std::cout << "Message encoded" << std::endl;

    mServer.start();

    std::cout << "Server started" << std::endl;

    while(roundCounter < 120)
    {
        usleep(1000000L);
        // std::cout << "Slept" << std::endl;

        // Note!: Took out sending here to better test the main sending function.
        //serialChannel.sendMessage(message);

        // std::cout << "Sent hartbeat" << std::endl;

        // Note!: For development only, a timed turn-off.
        //roundCounter++;

        if(mReceivedCommand == 1)
        {
            mReceivedCommand = 0;

            mInterpreter.getSettingPacket((uint8_t*)tmpBuff, msgLength);
            message.setBody((const char*)tmpBuff, msgLength);
            message.encode();
            serialChannel.sendMessage(message);
            std::cout << "Message forwarded" << std::endl;
        }
        else if(mReceivedCommand == 2)
        {
            break;
        }
    }

    serialChannel.stopThread();
    mServer.stop();
}

void ControlLogic::msgRxNotification(Base16Message* msg)
{
 //   std::cout << "Message:" << std::endl;
 //   std::cout << "  Header: 0:" << (int)msg->getHeaderByte(0) << " , 1:" << (int)msg->getHeaderByte(1) << std::endl;
    int length = msg->decodedLength();
 //   std::cout << "  Body(" << length << "):";

 //   for(int i = 0; i < length; i++)
 //   {
 //       std::cout << " " << (int)(msg->decodedBytesPtr()[i]);
 //   }
 //   std::cout << std::endl << std::endl;

    const void* pBytes = msg->decodedBytesPtr();

    mInterpreter.setParamsSerial((const uint8_t*)pBytes, length);
}

void ControlLogic::serveCall(const std::string& query, const DExGeneralParam* &retResponseStatics, unsigned int& retNumItems)
{
    retResponseStatics = mInterpreter.getPrintables();
    retNumItems = mInterpreter.getNumPrintables();

    int fieldValue;
    std::string fieldName;
    std::string valueAsString;

    size_t position = 0;
    size_t endOfSection;

    bool validCmdParamFound = false;

    while(position < query.length())
    {
        endOfSection = query.find('=', position);

        // End of query string?
        if(endOfSection == std::string::npos)
        {
            // End of query string.
            break;
        }

        // Extract the argument
        fieldName = query.substr(position, endOfSection - position);

        // Skip over the '='
        position = endOfSection + 1;

        // Find the end of the number.
        endOfSection = query.find('&', position);

        // End of query string?
        if(endOfSection == std::string::npos)
        {
            // End of query string. The number ends to the end of the query string.
            endOfSection = query.length();
        }

        valueAsString = query.substr(position, endOfSection - position);

        position = endOfSection + 1;

        std::cout << "Arg: " << fieldName << "  Value: " << valueAsString << " - ";

        try
        {
            fieldValue = std::stoi(valueAsString);
        }
        catch(...)
        {
            // The value was either out of range or mangled. Either way, skip and continue.
            fieldValue = 0;
            continue;
        }

        std::cout << fieldValue << std::endl;

        if(fieldName.compare("motorR") == 0)
        {
            mInterpreter.setRightMotor(fieldValue);
            validCmdParamFound = true;
        }
        else if(fieldName.compare("motorL") == 0)
        {
            mInterpreter.setLeftMotor(fieldValue);
            validCmdParamFound = true;
        }
        else if(fieldName.compare("camPan") == 0)
        {
            mInterpreter.setCameraPan(fieldValue);
            validCmdParamFound = true;
        }
        else if(fieldName.compare("camTilt") == 0)
        {
            mInterpreter.setCameraTilt(fieldValue);
            validCmdParamFound = true;
        }
    }

    if(validCmdParamFound == true)
    {
        mReceivedCommand = 1;
    }
}

void ControlLogic::reportError(const char* errorStr)
{
    std::cout << "ControlLogic : Error:" << errorStr << std::endl;
}
