#ifndef ROBOTBOARD2INTERPRETER_H
#define ROBOTBOARD2INTERPRETER_H


#include "DataExchangeStructures.h"

class RobotBoard2Interpreter
{
public:
    RobotBoard2Interpreter();

    // Values from sensors
    void setParamsSerial(uint8_t buff, uint16_t buffLength);
    float getBattVotage();
    float getBattCurrent();
    float getBoardTemperature();

    // Values to be sent to the robot
    void setRightMotor(int percent);
    void setLeftMotor(int percent);
    void setCameraPan(int percent);
    void setCameraTilt(int percent);
    void getSettingPacket(const uint8_t*& outPacketLoc, uint16_t& outPacketLength);

    // For serving web requests
    const JsWebUtils::DExGeneralParam* getPrintables();
    const unsigned int getNumPrintables() { return NumParams; }

private:

    static const char mStrPercent[] = "%";
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

    enum eParams
    {
        BattVoltage = 0,
        BatteryCurrent = 1,
        Temperature = 2,
        BatteryStatus = 3,
        /*RightMotor = 4,
        LeftMotor = 5,
        CamPan = 6,
        CamTilt = 7,*/
        NumParams
    };

    JsWebUtils::DExGeneralParam printableParams[NumParams];

};

#endif // ROBOTBOARD2INTERPRETER_H
