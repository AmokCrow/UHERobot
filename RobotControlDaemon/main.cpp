#include <iostream>

#include "ControlLogic.h"


int main()
{
    ControlLogic logic("logicpipe", "/dev/ttyAMA0");

    logic.run();


}
