
#include "ControlLogic.h"

ControlLogic::ControlLogic(const char* pipeName, const char* serialPort)
    : serialChannel(serialPort)
{

}

void ControlLogic::run()
{
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
        usleep(500000L);
        serialChannel.sendMessage(message);

        roundCounter++;
    }

}
