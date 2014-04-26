#include <iostream>

#include "ControlLogic.h"


int main()
{
    ControlLogic logic("/dev/ttyAMA0");

    logic.run();


}
