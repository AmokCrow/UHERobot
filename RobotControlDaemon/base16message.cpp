#include "base16message.h"

#include <string>
#include <iostream>

Base16Message::Base16Message()
    : mNumBytesUnencoded(0)
    , mNumBytesEncoded(0)
    , mIsEncoded(false)
    , mIsDecoded(false)
    , mHasHeader(false)
    , mHasData(false)
{

}

char Base16Message::getHeaderByte(int byteNum)
{
    mHeader[byteNum];
}

char* const Base16Message::encodedBytesPtr()
{
    if(mIsEncoded)
    {
        return mEncodedBuff;
    }
    else if(mIsDecoded)
    {
        encode();
    }
    else
    {
        return NULL;
    }
}

char* const Base16Message::decodedBytesPtr()
{
    if(mIsDecoded)
    {
        return mUnencodedBuff;
    }
    else if(mIsEncoded)
    {
        decode();
    }
    else
    {
        return NULL;
    }
}

int Base16Message::encodedLength()
{
    return mNumBytesEncoded;
}

int Base16Message::decodedLength()
{
    return mNumBytesUnencoded;
}

bool Base16Message::isValid()
{
    if(mIsEncoded || mIsDecoded)
    {
        return true;
    }

    return false;
}

bool Base16Message::setHeader(const char * const header)
{
    memcpy(mHeader, header, UHEROBOT_BOARD_MSG_HEADER_MAX_LEN);

    mHasHeader = true;

    if(mHasData)
    {
        mIsDecoded = true;
    }

    return isValid();
}

bool Base16Message::setBody(const char * const msg, int msgLen)
{
    if(msgLen > UHEROBOT_BOARD_MSG_MAX_LEN_BYTES)
    {
        classyError("Too long payload");
    }

    memcpy(mUnencodedBuff, msg, msgLen);

    mHasData = true;

    if(mHasHeader)
    {
        mIsDecoded = true;
    }

    return isValid();
}

eMsgStates Base16Message::feedRawMsgByte(char rawByte)
{
    // There was already a message here. Clear it and continue receiving.
    if(mIsEncoded)
    {
        clear();
    }

    // End of message. Try to decode it.
    if(rawByte == 's')
    {
        putByteToRawBuff(rawByte);

        if(decode())
        {
            return FINISHED;
        }
        else
        {
            classyError("Could not decode message properly");
            clear();
            return MSG_ERR;
        }
    }
    // Beginning of a message. Clear buffer.
    else if(rawByte == 'b')
    {
        clear();
        putByteToRawBuff(rawByte);
        return CONTINUE;
    }
    // Allow endline characters between messages
    else if((rawByte == '\r') || (rawByte == '\n'))
    {
        if(mNumBytesEncoded == 0)
        {
            return CONTINUE;
        }
        classyError("Bad character in message");
        clear();
        return MSG_ERR;
    }
    else if(((rawByte >= '0') && (rawByte <= '9')) || ((rawByte >= 'A') && (rawByte <= 'F')))
    {
        if(mNumBytesEncoded >= MAX_BUFF_SIZE - 2)
        {
            classyError("Buffer full");
            clear();
            return MSG_ERR;
        }

        if(mEncodedBuff[0] != 'b')
        {
            classyError("Badly formed message");
            clear();
            return MSG_ERR;
        }

        putByteToRawBuff(rawByte);
        return CONTINUE;
    }
    else
    {
        classyError("Bad character: Not Base16");
        clear();
        return MSG_ERR;
    }
}

void Base16Message::classyError(const char* const msg)
{
    std::cout << "Base16Message: Error: " << msg << std::endl;
}

void Base16Message::clear()
{
    mNumBytesEncoded = 0;
    mNumBytesUnencoded = 0;
    mIsEncoded = false;
    mIsDecoded = false;
    mHasData = false;
    mHasHeader = false;
}

// The buffBytes argument referred to is updated to contain the number of bytes
//  not handled before either a message was completed, an error encountered or
//  the buffer was consumed. The return value will tell which one of these happened.
eMsgStates Base16Message::feedRawMsgBuff(const char* const rawBuff, int& buffBytes)
{
    int buffPos = 0;
    eMsgStates lastState = CONTINUE;

    while((lastState == CONTINUE) && (buffBytes > 0))
    {
        lastState = feedRawMsgByte(rawBuff[buffPos]);
        buffPos++;
        buffBytes--;
    }

    return lastState;
}

bool Base16Message::encode()
{
    // Check if there is anything to encode
    if(mIsDecoded == false)
    {
        return false;
    }

    mNumBytesEncoded = 1;
    mEncodedBuff[0] = 'b';

    for(int i = 0; i < MAX_MSG_HEADER_LEN; i++)
    {
        byteToB16(mHeader[i], &(mEncodedBuff[mNumBytesEncoded]));
        mNumBytesEncoded += 2;
    }

    for(int i = 0; i < mNumBytesUnencoded; i++)
    {
        byteToB16(mUnencodedBuff[i], &(mEncodedBuff[mNumBytesEncoded]));
        mNumBytesEncoded += 2;
    }

    mEncodedBuff[mNumBytesEncoded] = 's';

    mNumBytesEncoded++;
}

bool Base16Message::decode()
{
    // The number of characters in the buffer should be 2n + 4, where n is the number of payload bytes.
    if((mNumBytesEncoded % 2) != 0)
    {
        classyError("Odd number of half-bytes in a message");
        clear();
        return false;
    }

    if(mNumBytesEncoded < (2 + (MAX_MSG_HEADER_LEN * 2)))
    {
        classyError("Too few half-bytes in a message");
        clear();
        return false;
    }

    for(int  i = 0; i < MAX_MSG_HEADER_LEN; i++)
    {
        mHeader[i] = B16ToByte(&(mEncodedBuff[1 + i * 2]));
    }

    mHasHeader = true;

    // start from beginning of message body and hop per Base16 pair
    int rawPos = 1 + (MAX_MSG_HEADER_LEN * 2);
    int dataPos = 0;
    while(rawPos < mNumBytesEncoded)
    {
        mUnencodedBuff[dataPos] = B16ToByte(&(mEncodedBuff[rawPos]));
        rawPos += 2;
        dataPos++;
    }

    if(dataPos != 0)
    {
        mHasData = true;
    }

    mIsEncoded = true;
    mIsDecoded = true;

    mNumBytesUnencoded = dataPos;

    return true;
}

void Base16Message::putByteToRawBuff(char ch)
{
    // This check should be unnecessary, but we all know how that goes...
    if(mNumBytesEncoded >= MAX_BUFF_SIZE)
    {
        classyError("Buffer overflow in pushing bytes in to buffer");
        clear();
        return;
    }

    mEncodedBuff[mNumBytesEncoded] = ch;
    mNumBytesEncoded++;
}

void byteToB16(char src, char* dest)
{
    char tmp = ((src & 0xF0) >> 4);

    if(tmp < 10)
    {
        tmp += '0';
    }
    else
    {
        tmp += 'A';
    }

    dest[0] = tmp;

    tmp = src & 0x0F;

    if(tmp < 10)
    {
        tmp += '0';
    }
    else
    {
        tmp += 'A' - 10;
    }

    dest[1] = tmp;
}

char B16ToByte(const char * const src)
{
    char returnables[2];

    for(int i = 0; i < 2; i++)
    {
        char tmp = src[i];

        if((tmp > '0') && (tmp < '9'))
        {
            tmp -= '0';
        }
        else
        {
            tmp += 10;
            tmp -= 'A';
        }

        returnables[i] = tmp;
    }

    return (returnables[0] << 4) | returnables[1];
}

