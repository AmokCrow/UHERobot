#ifndef ROBOTBOARD2INTERPRETER_H
#define ROBOTBOARD2INTERPRETER_H


#include "DataExchangeStructures.h"

#include <stdint.h>
#include <pthread.h>

class RobotBoard2Interpreter
{
public:
    RobotBoard2Interpreter();

    // Values from sensors
    void setParamsSerial(const uint8_t* buff, uint16_t buffLength);
    float getBattVoltage() { return printableParams[BattVoltage].value; }
    float getBattStatus() { return printableParams[BatteryStatus].value; }
    float getBattCurrent() { return printableParams[BatteryCurrent].value; }
    float getBoardTemperature() { return printableParams[Temperature].value; }


    // Values to be sent to the robot
    void setRightMotor(int percent);
    void setLeftMotor(int percent);
    void setCameraPan(int degrees);
    void setCameraTilt(int degrees);
    void setServoPos(unsigned int servoNum, uint16_t pos);
    void setLed(unsigned int ledNum, bool on);
    void getSettingPacket(uint8_t* outPacketLoc, unsigned int &outPacketLength);

    // For serving web requests
    const JsWebUtils::DExGeneralParam* getPrintables() { return (const JsWebUtils::DExGeneralParam*) printableParams; }
    unsigned int getNumPrintables() { return NumParams; }

private:

    void enterProtected();
    void exitProtected();

    static const char mStrBattCap[];
    static const char mStrPercent[];
    static const char mStrBattVolt[];
    static const char mStrVolt[];
    static const char mStrCurrent[];
    static const char mStrBrdTemp[];
    static const char mStrTemp[];
    static const char mStrBrdCurr[];
    static const char mStrJsonBattVolt[];
    static const char mStrJsonBrdCurr[];
    static const char mStrJsonBrdTemp[];
    static const char mStrJsonBattStat[];

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

    enum eParamWidths
    {
        MsgLengthLen = 1,
        MsgTypeLen = 1,
        LedsLen = 1,
        MotorLen = 2,
        ServoLen = 2
    };

    static const unsigned int TX_BUFF_SIZE = 25;
    static const unsigned int TX_CONTENT_SIZE = 23;

    JsWebUtils::DExGeneralParam printableParams[NumParams];
    uint8_t mTxBuff[TX_BUFF_SIZE];

    pthread_mutex_t mMutex;

};

#endif // ROBOTBOARD2INTERPRETER_H
