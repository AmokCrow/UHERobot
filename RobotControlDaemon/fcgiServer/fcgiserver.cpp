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

    const DExGeneralParam* clientReturnItems;
    unsigned int numReturnedItems;

    while(FCGX_Accept_r(&requestM) >= 0)
    {
        uri = FCGX_GetParam("REQUEST_URI", requestM.envp);
        query = FCGX_GetParam("QUERY_STRING", requestM.envp);
        method = FCGX_GetParam("METHOD", requestM.envp);

        if(method == "POST")
        {
            // TODO: Fish the string out of the body
        }

        std::cout << "Got request. URI: " << uri << "QUERY: " << query << std::endl;
        pClientM->serveCall(query, clientReturnItems, numReturnedItems);
        std::cout << "Served" << std::endl;
        if(std::string::npos != uri.find("api"))
        {
            // Serve with JSON
            FCGX_FPrintF(requestM.out, "Content-type: application/json\r\n\r\n");

            FCGX_FPrintF(requestM.out, "{");
            for(unsigned int i = 0; i < numReturnedItems; i++)
            {
                std::cout << clientReturnItems[i].apiMarker << std::endl;
                if(i == (numReturnedItems - 1))
                {
                    FCGX_FPrintF(requestM.out, " \"%s\" : \"%f\" ", clientReturnItems[i].apiMarker, clientReturnItems[i].value);
                }
                else
                {
                    FCGX_FPrintF(requestM.out, " \"%s\" : \"%f\", ", clientReturnItems[i].apiMarker, clientReturnItems[i].value);
                }
            }

            FCGX_FPrintF(requestM.out, "}\r\n");
        }
        else // Serve as a normal HTML query
        {
            FCGX_FPrintF(requestM.out, "Content-type: text/html\r\n\r\n");

            FCGX_FPrintF(requestM.out, "<h1>Server says</h1>\r\n<p>\r\n");
            for(unsigned int i = 0; i < numReturnedItems; i++)
            {
                std::cout << clientReturnItems[i].prefix << clientReturnItems[i].value << clientReturnItems[i].suffix << std::endl;
                FCGX_FPrintF(requestM.out, "%s : %f %s <br>\r\n", clientReturnItems[i].prefix, clientReturnItems[i].value, clientReturnItems[i].suffix);
            }

            FCGX_FPrintF(requestM.out, "</p>\r\n");
        }
        std::cout << "Finished" << std::endl;
        FCGX_Finish_r(&requestM);

    }
}

}
