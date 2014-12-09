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
    printableParams[BatteryCurrent].value = 0.0f;
    printableParams[BatteryCurrent].apiMarker = mStrJsonBrdCurr;

    printableParams[Temperature].prefix = mStrBrdTemp;
    printableParams[Temperature].suffix = mStrTemp;
    printableParams[Temperature].value = 0.0f;
    printableParams[Temperature].apiMarker = mStrJsonBrdTemp;


    printableParams[BatteryStatus].prefix = mStrBattCap;
    printableParams[BatteryStatus].suffix = mStrPercent;
    printableParams[BatteryStatus].value = 0.0f;
    printableParams[BatteryStatus].apiMarker = mStrJsonBattStat;

}
