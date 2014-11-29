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

    virtual ~FcgiServiceIf() { }
    virtual void serveCall(const std::string& query, const DExGeneralParam* &retResponseStatics, unsigned int& retNumItems) = 0;

};


}

#endif // FCGISERVICEIF_H
