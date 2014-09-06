#ifndef DEBUGLOG_H
#define DEBUGLOG_H

#include <queue>
#include <string>
#include <pthread.h>

class DebugLog
{
public:
    DebugLog(unsigned int maxItems);

    void add(std::string item);
    std::string get();
    bool isEmpty();

private:
    const unsigned int mMaxItems;
    std::queue<std::string> mMsgQueue;
    pthread_mutex_t mMutex;
};

#endif // DEBUGLOG_H
