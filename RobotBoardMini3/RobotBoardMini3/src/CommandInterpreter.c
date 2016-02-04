/*
 * CommandInterpreter.c
 *
 * Created: 10.3.2015 20:43:04
 *  Author: crow
 */ 

#include "CommandInterpreter.h"


// Returns non-zero is a complete message was received
uint8_t interpreterReceiveByte(CommIntBuff_t* inst, uint8_t b)
{
    uint8_t tmp;
    
    if(inst->cmdStartReceived == 0)
    {
        if(b == 'b')
        {
            inst->cmdParamsLength = 0;
            inst->cmdStartReceived = 1;
            inst->cmdOddHalfbyteReceived = 0;
        }
        
        return 0;
    }
    else if((b >= '0') && (b <= '9'))
    {
        tmp = b - '0';
    }
    else if((b >= 'A') && (b <= 'F'))
    {
        tmp = b + 10 - 'A';
    }
    else if(b == '\n')
    {
        inst->cmdStartReceived = 0;
        
        if(inst->cmdParamsLength > 0)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
    else if(b == '\r')
    {
        return 0;
    }
    else
    {
        inst->cmdStartReceived = 0;
        return 0;
    }
    
    if(inst->cmdParamsLength >= (UARTCMD_MAX_RX_LENGTH -  1))
    {
        inst->cmdStartReceived = 0;
        return 0;
    }
    
    if(inst->cmdOddHalfbyteReceived == 0)
    {
        inst->cmdHalfbyte = (tmp << 4);
        inst->cmdOddHalfbyteReceived = 1;
    }
    else
    {
        inst->cmdParams[inst->cmdParamsLength] = inst->cmdHalfbyte | tmp;
        inst->cmdOddHalfbyteReceived = 0;
        inst->cmdParamsLength += 1;
    }
    
    return 0;
}

void interpreterInit(CommIntBuff_t* inst)
{
    inst->cmdStartReceived = 0;
}

uint16_t interpreterFormMessage(uint8_t* outBuff, const uint8_t* bytes, uint16_t numBytes)
{
    uint16_t i;
    uint8_t halfc;
    uint16_t length = 3;
    
    *outBuff = 'b';
    outBuff++;
    
    for(i = 0; i < numBytes; i++)
    {
        halfc = (bytes[i] >> 4);
        
        if(halfc > 9)
        {
            halfc += 'A' - 10;
        }
        else
        {
            halfc += '0';
        }
        
        *outBuff = halfc;
        outBuff++;
        
        halfc = bytes[i] & 0x0F;
        
        if(halfc > 9)
        {
            halfc += 'A' - 10;
        }
        else
        {
            halfc += '0';
        }
        
        *outBuff = halfc;
        outBuff++;
        
        length += 2;
    }
    
    *outBuff = '\r';
    outBuff++;
    *outBuff = '\n';
    
    return length;
}
