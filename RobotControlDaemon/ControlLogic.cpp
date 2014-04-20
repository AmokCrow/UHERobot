
#include "ControlLogic.h"

#include "../CommandList.h"
#include "base16message.h"

#include <iostream>

ControlLogic::ControlLogic(const char* pipeName, const char* serialPort)
    : serialChannel(serialPort)
{

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

    while(roundCounter < 20)
    {
        usleep(1000000L);
        serialChannel.sendMessage(message);

        roundCounter++;
    }

    serialChannel.stopThread();

}

void ControlLogic::msgRxNotification(Base16Message* msg)
{
    std::cout << "Message:" << std::endl;
    std::cout << "  Header: 0:" << (int)msg->getHeaderByte(0) << " , 1:" << (int)msg->getHeaderByte(1) << std::endl;
    int length = msg->decodedLength();
    std::cout << "  Body(" << length << "):";

    for(int i = 0; i < length; i++)
    {
        std::cout << " " << (int)(msg->decodedBytesPtr()[i]);
    }
    std::cout << std::endl << std::endl;

    if(msg->getHeaderByte(0) == ANALOG_READING)
    {
        std::cout << "Got analog reading, length "
                  << msg->decodedLength()
                  << ", content "
                  << (int)(msg->decodedBytesPtr()[0])
                  << " " << (int)(msg->decodedBytesPtr()[1]) << std::endl;
    }
}

