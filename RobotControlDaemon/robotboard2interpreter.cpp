#include "robotboard2interpreter.h"

RobotBoard2Interpreter::RobotBoard2Interpreter()
{
    /*mStrPercent[] = "%";
    static const char mStrBattVolt[] = "Battery Voltage";
    static const char mStrVolt[] = "V";
    static const char mStrCurrent[] = "I";
    static const char mStrBrdTemp[] = "Board temperature";
    static const char mStrTemp[] = "C";
    static const char mStrBrdCurr[] = "Board current";
    static const char mStrJsonBattVolt[] = "bv";
    static const char mStrJsonBrdCurr[] = "bc";
    static const char mStrJsonBrdTemp[] = "bt";
    static const char mStrJsonBattStat[] = "bs";
*/
    printableParams[BattVoltage].prefix = mStrBattVolt;
    printableParams[BattVoltage].suffix = mStrVolt;
    printableParams[BattVoltage].value = 0.0f;
    printableParams[BattVoltage].apiMarker = mStrJsonBattVolt;

    printableParams[BatteryCurrent].prefix = mStrBrdCurr;
    printableParams[BatteryCurrent].suffix = mStrCurrent;
    printableParams[Temperature] = {};
    printableParams[BatteryStatus] = {};

}
