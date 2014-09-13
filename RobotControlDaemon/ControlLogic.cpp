
#include "ControlLogic.h"

#include "../CommandList.h"
#include "base16message.h"

#include <iostream>
#include <cstdio>

using namespace JsWebUtils;

ControlLogic::ControlLogic(const char* serialPort)
    : serialChannel(serialPort)
    , mServer(this)
    , fTargetSpeedLeftM(0.0f)
    , fTargetSpeedRightM(0.0f)
    , mLog(50)
{


    /*
     *battVoltageBuff[PARAM_BUFFERS_LENGTH];
  char devCurrentBuff[PARAM_BUFFERS_LENGTH];
  */
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
    char tmpBuff[50];
    Base16Message message;

    tmpBuff[0] = eRobotCommandTag::SET_MOTORS_TAG;
    tmpBuff[1] = 0;
    tmpBuff[2] = 0;
    tmpBuff[3] = 0;
    tmpBuff[4] = 0;

    message.setBody(tmpBuff, 5);
    message.encode();

    std::cout << "Message encoded" << std::endl;

    mServer.start();

    std::cout << "Server started" << std::endl;

    while(roundCounter < 120)
    {
        usleep(500000L);
        // std::cout << "Slept" << std::endl;
        serialChannel.sendMessage(message);

        // std::cout << "Sent hartbeat" << std::endl;
        roundCounter++;
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

    const char* pBytes = msg->decodedBytesPtr();

    for(int i = 0; i < length; i++)
    {
        if(pBytes[i] == ANALOG_READING_TAG)
        {
            std::cout << "Got analog reading, channel "
                      << (unsigned int)pBytes[i + 1]
                      << " result "
                      << (((unsigned int)pBytes[i + 2] << 8) | ((unsigned int)pBytes[i + 3]))
                      << std::endl;

            // Voltage measurement
            if(pBytes[i + 1] == 0)
            {
                mVoltage = (((uint16_t)pBytes[i + 2] << 8) | ((uint16_t)pBytes[i + 3]));
                //snprintf(battVoltageBuff, PARAM_BUFFERS_LENGTH, "%ui", (((unsigned int)pBytes[i + 2] << 8) | ((unsigned int)pBytes[i + 3])));
            }
            // Current measurement
            else if(pBytes[i + 1] == 1)
            {
                mCurrent = (((uint16_t)pBytes[i + 2] << 8) | ((uint16_t)pBytes[i + 3]));
                //snprintf(devCurrentBuff, PARAM_BUFFERS_LENGTH, "%ui", (((unsigned int)pBytes[i + 2] << 8) | ((unsigned int)pBytes[i + 3])));
            }

            i += ANALOG_READING_LENGTH;
        }
    }
}

void ControlLogic::serveCall(const std::string& query, const PrintableParamStat *&responseStatics, std::list<PrintableParamDyn> &responseDynamics)
{
    int fieldValue;
    std::string fieldName;
    std::string valueAsString;

    int motorR = 0;
    int motorL = 0;

    size_t position = 0;
    size_t endOfSection;

    responseStatics = 0;

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

        if(fieldName == "motorR")
        {
            motorR = fieldValue;
        }
        else if(fieldName == "motorL")
        {
            motorL == fieldValue;
        }
    }

    /*
    messageTxBuff[0] = SET_MOTORS_TAG;
    messageTxBuff[1] = (motorL >> 8) & 0xFF;
    messageTxBuff[2] = motorL & 0xFF;
    messageTxBuff[3] = (motorR >> 8) & 0xFF;
    messageTxBuff[4] = motorR & 0xFF;

    message.setBody(messageTxBuff, 5);
    message.encode();
    */

    char tmpBuff[50];
    PrintableParamDyn parTmp;

    // Voltage
    parTmp.name = "voltage";
    snprintf(tmpBuff, 50, "%ui", mVoltage);
    parTmp.value = tmpBuff;
    responseDynamics.push_back(parTmp);

    // Current
    parTmp.name = "current";
    snprintf(tmpBuff, 50, "%ui", mCurrent);
    parTmp.value = tmpBuff;
    responseDynamics.push_back(parTmp);
}

void ControlLogic::reportError(const char* errorStr)
{
    std::cout << "ControlLogic : Error:" << errorStr << std::endl;
}
