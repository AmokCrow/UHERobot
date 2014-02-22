#ifndef PIPEMESSAGER_H
#define PIPEMESSAGER_H

#include <string>


class PipeMessager
{
public:
    PipeMessager(bool openForRead, const char* pipeFileName);

    struct sCgiMessage
    {
        std::string header;
        std::string body;
    };

private:
    // The string gets written raarely enough and we have no way to know in advance the size of the filename.
    std::string mPipeName;


};

#endif // PIPEMESSAGER_H
