#include <iostream>
#include <string>

#include "ControlLogic.h"


int main(int argc, char* argv[])
{
    std::string portname;

    // TODO: Make a better command line parser
    for(int i = 0; i < (argc - 1); i++)
    {
        if(std::string(argv[i]) == "-p")
        {
            portname = argv[i + 1];
        }
    }

    if(portname.empty())
    {
        std::cout << "Error: Please specify at least the serial port to use" << std::endl;
        exit(1);
    }

    std::cout << "Using port " << portname << std::endl;


    ControlLogic logic(portname.c_str());

    logic.run();

    exit(0);


}
