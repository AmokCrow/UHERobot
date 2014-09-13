#ifndef FCGISERVICEIF_H
#define FCGISERVICEIF_H

#include <string>
#include <list>

namespace JsWebUtils
{

class FcgiServiceIf
{
public:



    struct PrintableParamDyn
    {
        std::string name;
        std::string value;
    };

    struct PrintableParamStat
    {
        const char* name;
        const char* value;
        PrintableParamStat* next;
    };

    virtual ~FcgiServiceIf() { }
    virtual void serveCall(const std::string& query, const PrintableParamStat* &responseStatics, std::list<PrintableParamDyn>& responseDynamics) = 0;

};


}

#endif // FCGISERVICEIF_H
