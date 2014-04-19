#include <iostream>

#include "ControlLogic.h"


int main()
{
    ControlLogic logic("logicpipe", "/dev/ttyACM0");

    logic.run();


}
