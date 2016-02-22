/*
 * ICommandInterpreter.h
 *
 * Created: 10.3.2015 20:42:26
 *  Author: crow
 */ 

#include <stdint.h>

#ifndef COMMAND_INTERPRETER__H
#define COMMAND_INTERPRETER__H

#define UARTCMD_MAX_RX_LENGTH 30

typedef struct
{
    uint8_t cmdParamsLength;
    uint8_t cmdStartReceived;
    uint8_t cmdOddHalfbyteReceived;
    uint8_t cmdHalfbyte;
    uint8_t cmdParams[UARTCMD_MAX_RX_LENGTH];
}
CommIntBuff_t;

// Returns non-zero is a complete message was received
uint8_t interpreterReceiveByte(CommIntBuff_t* inst, uint8_t b);
void interpreterInit(CommIntBuff_t* inst);


uint16_t interpreterFormMessage(uint8_t* outBuff, const uint8_t* bytes, uint16_t numBytes);




#endif /* COMMAND_INTERPRETER__H */