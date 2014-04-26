
#include "ControlLogic.h"

#include "../CommandList.h"
#include "base16message.h"

#include <iostream>
#include <cstdio>

using namespace JsWebUtils;

ControlLogic::ControlLogic(const char* serialPort, float speedChangeRate)
    : serialChannel(serialPort)
    , mServer(this)
    , cfRateOfChangeM(speedChangeRate)
    , fTargetSpeedLeftM(0.0f)
    , fTargetSpeedRightM(0.0f)
    , fCurrSpeedLeftM(0.0f)
    , fCurrSpeedRightM(0.0f)
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

    int roundCounter = 0;
    char tmpBuff[5];
    Base16Message message;

    tmpBuff[0] = SET_MOTORS;
    tmpBuff[1] = 100;
    tmpBuff[2] = 100;
    message.setHeader(tmpBuff);
    message.setBody(&(tmpBuff[1]), 2);
    message.encode();

    mServer.start();

    while(roundCounter < 40)
    {
        usleep(1000000L);
        serialChannel.sendMessage(message);

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
}

const FcgiServiceIf::PrintableParam* ControlLogic::serveCall(const std::string& query)
{
    if(query.find("fastforward") != std::string::npos)
    {
        pStatusField->value = "Moving - FastForward";
    }
    else if(query.find("forward") != std::string::npos)
    {
        pStatusField->value = "Moving - Forward";
    }
    else if(query.find("stop") != std::string::npos)
    {
        pStatusField->value = "Stopped";
    }
    else if(query.find("left") != std::string::npos)
    {
        pStatusField->value = "Moving - Turning Left";
    }
    else if(query.find("right") != std::string::npos)
    {
        pStatusField->value = "Moving - Turning Right";
    }
    else
    {
        pStatusField->value = "Error";
    }


    return webTextList;
}

void ControlLogic::setTargetSpeed(float leftTrack, float rightTrack)
{

}

void ControlLogic::recalculateCurrentSpeedCommand()
{

}

void ControlLogic::sendSpeedsCommand(float leftTrackSpeed, float rightTrackSpeed)
{

}

