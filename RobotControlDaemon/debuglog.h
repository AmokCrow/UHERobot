#ifndef DEBUGLOG_H
#define DEBUGLOG_H

#include <queue>
#include <string>
#include <pthread.h>

class DebugLog
{
public:
    DebugLog(int maxItems);

    add(std::string item);
    get(std::string item);

private:
    static int maxItems;
    std::queue<std::string> mMsgQueue;
    pthread_mutex_t mMutex;
};

#endif // DEBUGLOG_H
