#include <iostream>

#include "ControlLogic.h"


int main()
{
    ControlLogic logic("/dev/ttyACM0");

    logic.run();


}
