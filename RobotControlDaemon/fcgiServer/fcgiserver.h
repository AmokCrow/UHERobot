#ifndef FCGISERVER_H
#define FCGISERVER_H


#include <thread>
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

    void run();

    FcgiServiceIf* pClientM;

    std::thread threadM;
    FCGX_Request requestM;
};

}

#endif // FCGISERVER_H
