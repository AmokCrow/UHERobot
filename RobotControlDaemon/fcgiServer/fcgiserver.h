#ifndef FCGISERVER_H
#define FCGISERVER_H

namespace JsWebUtils
{

class FcgiServer
{
public:
    FcgiServer(unsigned int numThreads = 1);
    virtual ~FcgiServer();

    void start();

};

}

#endif // FCGISERVER_H
