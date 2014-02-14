#include "base16message.h"

#include <string>

Base16Message::Base16Message()
    : mNumBytesUnencoded(0)
    , mNumBytesEncoded(0)
    , mIsEncoded(false)
    , mIsDecoded(false)
    , mHasHeader(false)
    , mHasData(false)
{

}
Base16Message();

char getHeaderByte(int byteNum);
char* const encodedBytesPtr();
char* const decodedBytesPtr();

bool isValid();

bool setHeader(const char * const header, int headerLen);
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
eMsgStates feedRawMsgBuff(const char* const rawBuff, int& buffBytes);

private:

static const int MAX_MSG_LEN = UHEROBOT_BOARD_MSG_MAX_LEN_BYTES;
static const int MAX_MSG_HEADER_LEN = UHEROBOT_BOARD_MSG_HEADER_MAX_LEN;
// The start and end bytes take 2 bytes
static const int MAX_BUFF_SIZE = (MAX_MSG_HEADER_LEN + MAX_MSG_LEN) * 2 + 2;

char mHeader[MAX_MSG_HEADER_LEN];
char mUnencodedBuff[MAX_MSG_LEN];

