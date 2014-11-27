/* 
* Base16MsgParser.h
*
* Created: 3.11.2014 21:13:18
* Author: crow
*/


#ifndef __BASE16MSGPARSER_H__
#define __BASE16MSGPARSER_H__

#include <stdint.h>

class Base16MsgParser
{
public:
	Base16MsgParser();
    void init(uint8_t* buffer, uint8_t bufferLength);
    
    uint8_t decodeChar(uint8_t ch);
    uint8_t decodeBuffer(uint8_t* buf, uint8_t bufLen, uint8_t* numParsed);
    uint8_t numBytesStillExpected();
    
    void encodeChar(uint8_t ch);
    void encodeBuffer(uint8_t* buffer, uint8_t length);
    uint8_t finalizeMsg();
    
    void reset();
    
protected:
private:
    // The location and length of the external working buffer
    uint8_t* mpBuffer;
    uint8_t mBuffLength;
    
    uint8_t mLastHalfbyteValueReceived;
    uint8_t mLastHalfbyteWasOddParity;
    uint8_t mNumBytesInBuffer;
    
    uint8_t mNumBytesTotalExpected;
    uint8_t mValidStart;

	Base16MsgParser( const Base16MsgParser &c );
	Base16MsgParser& operator=( const Base16MsgParser &c );

}; //Base16MsgParser

#endif //__BASE16MSGPARSER_H__
