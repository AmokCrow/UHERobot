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
    threadM = std::thread(&FcgiServer::run, this);
    FCGX_InitRequest(&requestM, 0, 0);
}

void FcgiServer::stop()
{
    FCGX_ShutdownPending();
    threadM.join();
}

void FcgiServer::run()
{
    std::string query;
    std::string uri;

    while(FCGX_Accept_r(&requestM) >= 0)
    {
        uri = FCGX_GetParam("REQUEST_URI", requestM.envp);
        query = FCGX_GetParam("QUERY_STRING", requestM.envp);
        std::cout << "Got request. URI: " << uri << "QUERY: " << query << std::endl;
        const FcgiServiceIf::PrintableParam* par = pClientM->serveCall(query);
        std::cout << "Served" << std::endl;
        if(std::string::npos != uri.find("api"))
        {
            // Serve with JSON
        }
        else // Serve as a normal HTML query
        {
            FCGX_FPrintF(requestM.out, "Content-type: text/html\r\n\r\n");

            FCGX_FPrintF(requestM.out, "<h1>Server says</h1>\r\n<p>\r\n");
            for(; par != NULL; par = par->next)
            {
                std::cout << par->name << std::endl;
                FCGX_FPrintF(requestM.out, "%s : %s <br>\r\n", par->name, par->value);
            }
            FCGX_FPrintF(requestM.out, "</p>\r\n");
        }
        std::cout << "Finished" << std::endl;
        FCGX_Finish_r(&requestM);

    }
}

}
