#include "debuglog.h"

DebugLog::DebugLog()
    : maxItems(0)
    , mMsgQueue()
    , mMutex()
{
    pthread_mutex_init(&txMutexM, NULL);
}

DebugLog::add(std::string item)
{

}

DebugLog::get(std::string item)
{

}
