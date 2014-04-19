#ifndef FCGISERVICEIF_H
#define FCGISERVICEIF_H

#include <string>

namespace JsWebUtils
{

class FcgiServiceIf
{
public:



    struct PrintableParam
    {
        const char* name;
        const char* value;
        PrintableParam* next;
    };

    virtual ~FcgiServiceIf();
    const PrintableParam** serveCall(const std::string& query) = 0;

};


}

#endif // FCGISERVICEIF_H
