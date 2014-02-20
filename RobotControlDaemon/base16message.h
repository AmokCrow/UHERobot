#ifndef BASE16MESSAGE_H
#define BASE16MESSAGE_H

#include "../CommandList.h"

/*
 * Usage: feeding encoded (may be referred to as "raw") data
 * makes there be an unencoded message available upon completion.
 * Feeding encoded data until completion will result in ability
 * to read an unencoded (or decoded message). Overwriting just the
 * header or body of a decoded message can be done; the rest will
 * be encoded back if encoded data is accessed.
 * Errors: Trying to access unfinished or invalid data will return
 * a NULL pointer.
 */

class Base16Message
{
  public:

    Base16Message();

    char getHeaderByte(int byteNum);
    const char* encodedBytesPtr();
    int encodedLength();
    int decodedLength();
    const char* decodedBytesPtr();

    bool isValid();

    bool setHeader(const char * const header);
    bool setBody(const char * const msg, int msgLen);

    enum eMsgStates {
        FINISHED,
        MSG_ERR,
        CONTINUE
    };

    eMsgStates feedRawMsgByte(char rawByte);
    // The buffBytes argument referred to is updated to contain the number of bytes
    //  not handled before either a message was completed, an error encountered or
    //  the buffer was consumed. The return value will tell which one of these happened.
    eMsgStates feedRawMsgBuff(const char* rawBuff, int& buffBytes);

    bool encode();
    bool decode();

  private:

    void putByteToRawBuff(char ch);
    void byteToB16(char src, char* dest);
    char B16ToByte(const char * const src);
    void clear();

    void classyError(const char* const msg);

    static const int MAX_MSG_LEN = UHEROBOT_BOARD_MSG_MAX_LEN_BYTES;
    static const int MAX_MSG_HEADER_LEN = UHEROBOT_BOARD_MSG_HEADER_MAX_LEN;
    // The start and end bytes take 2 bytes
    static const int MAX_BUFF_SIZE = (MAX_MSG_HEADER_LEN + MAX_MSG_LEN) * 2 + 2;

    char mHeader[MAX_MSG_HEADER_LEN];
    char mUnencodedBuff[MAX_MSG_LEN];
    int mNumBytesUnencoded;

    // The start and end bytes take
    char mEncodedBuff[MAX_BUFF_SIZE];
    int mNumBytesEncoded;

    // These two represent whether there IS encoded or unencoded data or both.
    // They DO NOT represent the last action performed.
    bool mIsEncoded;
    bool mIsDecoded;

    // These represent whether there is header and/or data in the unencoded side.
    bool mHasHeader;
    bool mHasData;
};

#endif // BASE16MESSAGE_H
