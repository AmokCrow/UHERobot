#include "fcgiserver.h"


#include <sys/types.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <fcgi_config.h>
#include <string>

#include <iostream>

namespace JsWebUtils
{

FcgiServer::FcgiServer(FcgiServiceIf* client)
    : pClientM(client)
    , threadM()
    , requestM()
{
    FCGX_Init();
}

FcgiServer::~FcgiServer()
{

}

void FcgiServer::start()
{
    FCGX_InitRequest(&requestM, 0, 0);

    // TODO: Change back to C++11 threads after RasPi or other targets get
    // library update to fix threads crash bug. Already fixed in Ubuntu,
    // but not in Raspbian...
    // threadM = std::thread(&FcgiServer::run, this);

    int status = pthread_create(&threadM, NULL, statRun, this);

    if(status != 0)
    {
        std::cout << "Error: pthread_create failed" << std::endl;
    }
}

void FcgiServer::stop()
{
    FCGX_ShutdownPending();

    // TODO: Change to C++11 thread along with thread creation
    // threadM.join();

    pthread_join(threadM, NULL);
}

void FcgiServer::run()
{
    std::string query;
    std::string uri;
    std::string method;

    int tmp;

    const char* response;
    const char* cpTmp;
    FcgiServiceIf::eResponseType respType;

    while(FCGX_Accept_r(&requestM) >= 0)
    {

        cpTmp = FCGX_GetParam("REQUEST_URI", requestM.envp);
        if(cpTmp != NULL)
        {
            uri = cpTmp;
        }
        else
        {
            uri.clear();
        }

        cpTmp = FCGX_GetParam("QUERY_STRING", requestM.envp);
        if(cpTmp != NULL)
        {
            query = cpTmp;
        }
        else
        {
            query.clear();
        }

        cpTmp = FCGX_GetParam("REQUEST_METHOD", requestM.envp);
        if(cpTmp != NULL)
        {
            method = cpTmp;
        }
        else
        {
            method.clear();
        }

        if(method == "POST")
        {
            tmp = FCGX_GetStr(mPostInputBuffer, cmPostInputBufferSize - 1, requestM.in);
            mPostInputBuffer[tmp] = 0;
            query.append(mPostInputBuffer);
        }

        std::cout << "Got request. URI: " << uri << " QUERY: " << query << " METHOD: " << method << std::endl;

        if(std::string::npos != uri.find("api"))
        {
            // Serve with JSON
            FCGX_FPrintF(requestM.out, "Content-type: application/json\r\n\r\n");

            pClientM->serveCall(query, std::string("api"), response, respType);
            std::cout << "Served" << std::endl;

            FCGX_PutS(response, requestM.out);
            //std::cout << response << std::endl;

            FCGX_FPrintF(requestM.out, "\r\n");
        }
        else // Serve as a normal HTML query
        {
            FCGX_FPrintF(requestM.out, "Content-type: text/html\r\n\r\n");

            FCGX_FPrintF(requestM.out, "<h1>Server says</h1>\r\n<p>\r\n");


            pClientM->serveCall(query, std::string("Web"), response, respType);
            std::cout << "Served" << std::endl;

            FCGX_PutS(response, requestM.out);

            FCGX_FPrintF(requestM.out, "<p>Finished</p>\r\n");

        }

        std::cout << "Finished" << std::endl;
        FCGX_Finish_r(&requestM);

    }
}

}
