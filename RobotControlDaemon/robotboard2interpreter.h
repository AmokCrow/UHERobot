#ifndef ROBOTBOARD2INTERPRETER_H
#define ROBOTBOARD2INTERPRETER_H


#include <stdint.h>
#include <pthread.h>
#include <memory>

class RobotBoard2Interpreter
{
public:
    RobotBoard2Interpreter();

    // Values from sensors
    void setParamsSerial(const uint8_t* buff, uint16_t buffLength);
    float getCpuVoltage() { return mCpuVoltage; }
    float getCpuCurrent() { return mCpuCurrent; }
    float getMotorCurrent() { return mMotorCurrent; }
    float getMotorVoltage() { return mMotorVoltage; }
    float getIntTemperature() { return mTemperature; }


    // Values to be sent to the robot
    void setRightMotor(int percent);
    void setLeftMotor(int percent);
    void setCameraPan(int degrees);
    void setCameraTilt(int degrees);
    void setServoPos(unsigned int servoNum, uint16_t pos);
    void setLed(unsigned int ledNum, bool on);
    void getSettingPacket(uint8_t* outPacketLoc, unsigned int &outPacketLength);

    const char* getJson();
    const char* getHtml();
    const char* getPlaintext();

private:

    void enterProtected();
    void exitProtected();

    void extractValues(const uint8_t* buffer, uint16_t length);
    void extractAdcValues(const uint8_t* buffer, uint16_t length);

    void producePlaintext();
    void produceHtml();
    void produceJson();

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
        ServoLen = 2,
        MotorsTotalLen = MotorLen * 2,
        ServosTotalLen = ServoLen * 2
    };

    static const unsigned int TX_BUFF_SIZE = 25;
    static const unsigned int TX_CONTENT_SIZE = 23;

    uint8_t mTxBuff[TX_BUFF_SIZE];

    pthread_mutex_t mMutex;

    struct textBuffer
    {

    };

    char bufferPlaintext1[100];
    char bufferPlaintext2[100];

    char bufferHtml1[200];
    char bufferHtml2[200];

    char bufferJson1[100];
    char bufferJson2[100];

    volatile const char* mpHtml;
    volatile const char* mpJson;
    volatile const char* mpPlaintext;

    char* mpTmp;

    // temp, cpu_curr, cpu_volt, mot_curr
    float mTemperature;
    float mMcuTemperature;
    float mCpuCurrent;
    float mCpuVoltage;
    float mMotorCurrent;
    float mMotorVoltage;

    char mIntBuff[100];
};

#endif // ROBOTBOARD2INTERPRETER_H
