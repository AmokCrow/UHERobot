#include "debuglog.h"

DebugLog::DebugLog(unsigned int maxItems)
    : mMaxItems(maxItems)
    , mMsgQueue()
    , mMutex()
{
    pthread_mutex_init(&mMutex, NULL);
}

void DebugLog::add(std::string item)
{
    pthread_mutex_lock(&mMutex);

    if(mMsgQueue.size() < mMaxItems)
    {
        mMsgQueue.push(item);
    }

    pthread_mutex_unlock(&mMutex);
}

std::string DebugLog::get()
{
    std::string retString;

    pthread_mutex_lock(&mMutex);

    if(mMsgQueue.empty() == false)
    {
        retString = mMsgQueue.front();
        mMsgQueue.pop();
    }

    pthread_mutex_unlock(&mMutex);

    return retString;
}

bool DebugLog::isEmpty()
{
    pthread_mutex_lock(&mMutex);

    bool bEmpty = mMsgQueue.empty();

    pthread_mutex_unlock(&mMutex);

    return bEmpty;
}
