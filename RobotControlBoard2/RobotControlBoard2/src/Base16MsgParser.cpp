/* 
* Base16MsgParser.cpp
*
* Created: 3.11.2014 21:13:17
* Author: crow
*/


#include "Base16MsgParser.h"

// default constructor
Base16MsgParser::Base16MsgParser()
{
} //Base16MsgParser

void Base16MsgParser::init(uint8_t* buffer, uint8_t bufferLength)
{
    mpBuffer = buffer;
    mBuffLength = bufferLength;
    reset();
}

uint8_t Base16MsgParser::decodeChar(uint8_t ch)
{
    // Not validly started message
    if(mValidStart == 0)
    {
        // Msg start
        if(ch == 'b')
        {
            reset();
            mValidStart = 1;
            return 0;
        }
        else
        {
            reset();
            return 0;
        }
    }
    
    // Buffer overflow protection
    if(mNumBytesInBuffer >= mBuffLength)
    {
        reset();
        return 0;
    }
    
    uint8_t halfbyte;
    
    if((ch >= '0') && (ch <= '9'))
    {
        halfbyte = ch - '0';
    }
    else if((ch >= 'A') && (ch <= 'F'))
    {
        halfbyte = (ch + 10) - 'A';
    }
    // Not a valid half-byte. Check for other known characters.
    else
    {
        // Msg start
        if(ch == 'b')
        {
            reset();
            mValidStart = 1;
            return 0;
        }
        
        // Msg end
        if(ch == 'e')
        {
            if((mLastHalfbyteWasOddParity == 0) && (mNumBytesInBuffer != 0) && (mNumBytesInBuffer == mNumBytesTotalExpected))
            {
                uint8_t tmp = mNumBytesInBuffer;
                reset();
                return tmp;
            }
        }
        
        // Not a valid situation. Reset communication.
        reset();
        return 0;
    }
    
    // By this point, the character received is a valid half-byte
    if(mLastHalfbyteWasOddParity)
    {
        mpBuffer[mNumBytesInBuffer] = (mLastHalfbyteValueReceived << 4) | halfbyte;
        mNumBytesInBuffer++;
        mLastHalfbyteWasOddParity = 0;
        
        if(mNumBytesInBuffer == 1)
        {
            mNumBytesTotalExpected = mpBuffer[0];
        }
        else
        {
            if(mNumBytesTotalExpected < mNumBytesInBuffer)
            {
                reset();
                return 0;
            }
        }
    }
    else
    {
        mLastHalfbyteValueReceived = halfbyte;
        mLastHalfbyteWasOddParity = 1;
    }
    
    return 0;
}

uint8_t Base16MsgParser::decodeBuffer(uint8_t* buf, uint8_t bufLen, uint8_t* numParsed)
{
    uint8_t tmp;
    
    for(uint16_t i = 0; i < bufLen; i++)
    {
        tmp = decodeChar(buf[i]);
        
        if(numParsed != 0)
        {
            *numParsed = i + 1;
            return tmp;
        }
    }
    
    return 0;
}

uint8_t Base16MsgParser::numBytesStillExpected()
{
    return mNumBytesTotalExpected - mNumBytesInBuffer;
}

void Base16MsgParser::encodeChar(uint8_t ch)
{
    if(mNumBytesInBuffer == 0)
    {
        mpBuffer[0] = 'b';
        mNumBytesInBuffer++;
    }
    else if(mNumBytesInBuffer >= (mBuffLength - 2))
    {
        return;
    }
    
    uint8_t tmp = (ch >> 4) & 0x0F;
    
    if(tmp < 10)
    {
        mpBuffer[mNumBytesInBuffer] = tmp + '0';
    }
    else
    {
         mpBuffer[mNumBytesInBuffer] = (tmp - 10) + 'A';
    }
    mNumBytesInBuffer++;
    
    tmp = ch & 0x0F;
    
    if(tmp < 10)
    {
        mpBuffer[mNumBytesInBuffer] = tmp + '0';
    }
    else
    {
        mpBuffer[mNumBytesInBuffer] = (tmp - 10) + 'A';
    }
    mNumBytesInBuffer++;
}

void Base16MsgParser::encodeBuffer(uint8_t* buffer, uint8_t length)
{
    for(uint16_t i = 0; i < length; i++)
    {
        encodeChar(buffer[i]);
    }
}

uint8_t Base16MsgParser::finalizeMsg()
{
    mpBuffer[mNumBytesInBuffer] = 'e';
    mNumBytesInBuffer++;
    return mNumBytesInBuffer;
}

void Base16MsgParser::reset()
{
    mLastHalfbyteValueReceived = 0;
    mNumBytesInBuffer = 0;
    mLastHalfbyteWasOddParity = 0;
    mValidStart = 0;
    mNumBytesTotalExpected = 0;
}
