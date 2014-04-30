#ifndef FCGISERVER_H
#define FCGISERVER_H


#include <pthread.h>
#include <vector>

#include <fcgiapp.h>

#include "fcgiserviceif.h"

namespace JsWebUtils
{

class FcgiServer
{
public:
    FcgiServer(FcgiServiceIf* client);
    virtual ~FcgiServer();

    void start();
    void stop();

private:

    static void* statRun(void* instance) { ((FcgiServer*)instance)->run(); return NULL; }
    void run();

    FcgiServiceIf* pClientM;

    pthread_t threadM;
    FCGX_Request requestM;
};

}

#endif // FCGISERVER_H
