
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
    webTextList = new FcgiServiceIf::PrintableParam;
    webTextList->name = "Battery: ";
    webTextList->value = battVoltageBuff;
    sprintf(battVoltageBuff, "empty");

    webTextList->next = new FcgiServiceIf::PrintableParam;
    FcgiServiceIf::PrintableParam* pTmp = webTextList->next;
    pTmp->name = "Current: ";
    pTmp->value = devCurrentBuff;
    sprintf(devCurrentBuff, "none");

    pTmp->next = new FcgiServiceIf::PrintableParam;
    pTmp = pTmp->next;
    pTmp->name = "Status: ";
    pTmp->value = "erroneous";

    pStatusField = pTmp;

    /*
     *battVoltageBuff[PARAM_BUFFERS_LENGTH];
  char devCurrentBuff[PARAM_BUFFERS_LENGTH];
  */
}

ControlLogic::~ControlLogic()
{
    while(webTextList != NULL)
    {
        FcgiServiceIf::PrintableParam* pTmp = webTextList->next;
        delete webTextList;
        webTextList = pTmp;
    }
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
                snprintf(battVoltageBuff, PARAM_BUFFERS_LENGTH, "%ui", (((unsigned int)pBytes[i + 2] << 8) | ((unsigned int)pBytes[i + 3])));
            }
            // Current measurement
            else if(pBytes[i + 1] == 1)
            {
                snprintf(devCurrentBuff, PARAM_BUFFERS_LENGTH, "%ui", (((unsigned int)pBytes[i + 2] << 8) | ((unsigned int)pBytes[i + 3])));
            }

            i += ANALOG_READING_LENGTH;
        }
    }
}

const FcgiServiceIf::PrintableParam* ControlLogic::serveCall(const std::string& query)
{
    if(query.find("fastforward") != std::string::npos)
    {
        pStatusField->value = "Moving - FastForward";
        //setTrackSpeeds(1.0f, 1.0f);
    }
    else if(query.find("forward") != std::string::npos)
    {
        pStatusField->value = "Moving - Forward";
        //setTrackSpeeds(0.5f, 0.5f);
    }
    else if(query.find("stop") != std::string::npos)
    {
        pStatusField->value = "Stopped";
        //setTrackSpeeds(0.0f, 0.0f);
    }
    else if(query.find("left") != std::string::npos)
    {
        pStatusField->value = "Moving - Turning Left";
        //setTrackSpeeds(-0.3f, 0.3f);
    }
    else if(query.find("right") != std::string::npos)
    {
        pStatusField->value = "Moving - Turning Right";
        //setTrackSpeeds(0.3f, -0.3f);
    }
    else
    {
        pStatusField->value = "Error";
        //setTrackSpeeds(0.0f, 0.0f);
    }


    return webTextList;
}

void ControlLogic::reportError(const char* errorStr)
{
    std::cout << "ControlLogic : Error:" << errorStr << std::endl;
}
