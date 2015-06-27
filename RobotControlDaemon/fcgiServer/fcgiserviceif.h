#ifndef FCGISERVICEIF_H
#define FCGISERVICEIF_H

#include <string>
#include <list>

#include "DataExchangeStructures.h"

namespace JsWebUtils
{

class FcgiServiceIf
{
public:

    enum eResponseType
    {
        HtmlResponse,
        JsonResponse,
        TextResponse
    };

    virtual ~FcgiServiceIf() { }
    virtual void serveCall(const std::string& query, const std::string& uri, const char* &response, eResponseType &respType) = 0;

};


}

#endif // FCGISERVICEIF_H
