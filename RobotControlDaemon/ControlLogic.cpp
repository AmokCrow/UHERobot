
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
    char tmpBuff[5];
    Base16Message message;

    tmpBuff[0] = eRobotCommands::CONTROLLER_HEARTBEAT;
    tmpBuff[1] = 0;
    tmpBuff[2] = 0;
    message.setHeader(tmpBuff);
    message.setBody(&(tmpBuff[1]), 0);
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

    if(msg->getHeaderByte(0) == ANALOG_READING)
    {
        std::cout << "Got analog reading, length "
                  << msg->decodedLength()
                  << ", content "
                  << (int)(msg->decodedBytesPtr()[0])
                  << " " << (int)(msg->decodedBytesPtr()[1]) << std::endl;
    }
    else
    {
        std::cout << "Got a message, length "
                  << msg->decodedLength()
                  << ", content ";
        for(int i = 0; i < msg->decodedLength(); i++)
        {
            std::cout << (int)(msg->decodedBytesPtr()[i]) << " ";
        }
        std::cout << std::endl;
    }
}

const FcgiServiceIf::PrintableParam* ControlLogic::serveCall(const std::string& query)
{
    if(query.find("fastforward") != std::string::npos)
    {
        pStatusField->value = "Moving - FastForward";
        setTrackSpeeds(1.0f, 1.0f);
    }
    else if(query.find("forward") != std::string::npos)
    {
        pStatusField->value = "Moving - Forward";
        setTrackSpeeds(0.5f, 0.5f);
    }
    else if(query.find("stop") != std::string::npos)
    {
        pStatusField->value = "Stopped";
        setTrackSpeeds(0.0f, 0.0f);
    }
    else if(query.find("left") != std::string::npos)
    {
        pStatusField->value = "Moving - Turning Left";
        setTrackSpeeds(-0.3f, 0.3f);
    }
    else if(query.find("right") != std::string::npos)
    {
        pStatusField->value = "Moving - Turning Right";
        setTrackSpeeds(0.3f, -0.3f);
    }
    else
    {
        pStatusField->value = "Error";
        setTrackSpeeds(0.0f, 0.0f);
    }


    return webTextList;
}

void ControlLogic::setTrackSpeeds(float leftTrack, float rightTrack)
{
    if((leftTrack >= 1.0f) || (leftTrack <= -1.0f) || (rightTrack >= 1.0f) || (rightTrack <= -1.0f))
    {
        reportError("ControlLogic::setTrackSpeeds : Error: Speed out of range");
        return;
    }

    char left = SPEED_SCALE * leftTrack;
    char right = SPEED_SCALE * rightTrack;

    sendMotorsCommand(left, right);
}

void ControlLogic::sendMotorsCommand(char leftTrackSetting, char rightTrackSetting)
{
    char tmpBuff[2];
    tmpBuff[0] = leftTrackSetting;
    tmpBuff[1] = rightTrackSetting;

    char command = eRobotCommands::SET_MOTORS;

    Base16Message message;
    message.setHeader(&command);
    message.setBody(tmpBuff, 2);
    if(message.encode() == false)
    {
        reportError("Message did not encode");
    }
    serialChannel.sendMessage(message);
}

void ControlLogic::reportError(const char* errorStr)
{
    std::cout << "ControlLogic : Error:" << errorStr << std::endl;
}
