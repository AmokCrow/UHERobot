#include "robotboard2interpreter.h"
#include <cstring>
#include <cstdint>

#include <iostream>
#include <cstdio>


RobotBoard2Interpreter::RobotBoard2Interpreter()
    : mpPlaintext()
    , mpHtml()
    , mpJson()
    , mTemperature(0.0f)
    , mCpuCurrent(0.0f)
    , mCpuVoltage(0.0f)
    , mMotorCurrent(0.0f)
    , mMotorVoltage(0.0f)
{
    mTxBuff[0] = 23;
    mTxBuff[1] = 0;

    for(unsigned int i = 2; i < TX_BUFF_SIZE; i++)
    {
        mTxBuff[i] = 0;
    }

    producePlaintext();
    produceHtml();
    produceJson();

    pthread_mutex_init(&mMutex, NULL);
}

void RobotBoard2Interpreter::getSettingPacket(uint8_t* outPacketLoc, unsigned int& outPacketLength)
{
    outPacketLength = TX_CONTENT_SIZE;

    enterProtected();
    memcpy(outPacketLoc, mTxBuff, TX_CONTENT_SIZE);
    exitProtected();

    return;
}

// Data: Voltage in mV (2 bytes, signed) | Current in mA (2 bytes, signed) | Temperature in mC (2 bytes, signed)

void RobotBoard2Interpreter::setParamsSerial(const uint8_t *buff, uint16_t buffLength)
{
    /*
    if(!((buffLength == 8) && (buff[1] == 1) && (buff[0] == 8)))
    {
        std::cout << "Bad msg: len " << (unsigned int)buffLength << " - reported " << (unsigned int)buff[0] << ", type " << (unsigned int)buff[1] << std::endl;
        return;
    }
    */

    extractValues(buff, buffLength);
    producePlaintext();
    produceHtml();
    produceJson();
}

void RobotBoard2Interpreter::extractValues(const uint8_t* buffer, uint16_t length)
{
    uint16_t tmp = buffer[0] << 8;
    float voltage;
    tmp |= buffer[1];

    // The ADC has a 12bit result. But for signed measurements, one bit is the sign.
    // The reference is 3.3V / 1.6
    // 2**12 == 4096, 2**11 == 2048

    // Temperature is an internal mesurement, unsigned, no gain.
    // TODO: find out the temperature/voltage coefficient and zero-point. They're individual for processors.
    mTemperature = (tmp * 1.0f) / 4096.0f * (3.3f / 1.6f);

    tmp = buffer[2] << 8;
    tmp |= buffer[3];

    // The full scale of the current sensors is +/-12A. Zero point is Vcc/2.
    // The input has 0.5x gain. Mode is signed (although the result here is not,
    //  as reference is GND), so full-scale is 2**11.
    voltage = (tmp * 1.0f) / 2048.0f * (3.3f * 2.0f / 1.6f);
    // Deduct 2**11 / 2 == 1024 to make result signed and
    mCpuCurrent = ((voltage / 3.3f) - 0.5f) * 2.0f * 12.0f;

    tmp = buffer[4] << 8;
    tmp |= buffer[5];

    // Differential, but with no gain, as input is 1/11 of the battery voltage.
    voltage = (tmp * 1.0f) / 2048.0f * (3.3f / 1.6f);
    mCpuVoltage = voltage * 11.0f;

    tmp = buffer[6] << 8;
    tmp |= buffer[7];

    // Same sensor tupe as CPU current, above.
    voltage = (tmp * 1.0f) / 2048.0f * (3.3f * 2.0f / 1.6f);
    mMotorCurrent = ((voltage / 3.3f) - 0.5f) * 2.0f * 12.0f;
}

void RobotBoard2Interpreter::producePlaintext()
{
    if(mpPlaintext == bufferPlaintext1)
    {
        mpTmp = bufferPlaintext2;
    }
    else
    {
        mpTmp = bufferPlaintext1;
    }

    std::sprintf(mpTmp, "Temperature sensor: %f V \r\n", mTemperature);

    mpPlaintext = mpTmp;
}

void RobotBoard2Interpreter::produceHtml()
{
    if(mpHtml == bufferHtml1)
    {
        mpTmp = bufferHtml2;
    }
    else
    {
        mpTmp = bufferHtml1;
    }

    std::sprintf(mpTmp, "<p>Temperature sensor: %fV<br> CPU Battery: %fV <br> CPU Current: %fA<br>Motor Voltage: N/A<br>Motor Current: %fA </p>",
                 mTemperature, mCpuVoltage, mCpuCurrent, mMotorCurrent);

    mpHtml = mpTmp;
}

void RobotBoard2Interpreter::produceJson()
{
    if(mpJson == bufferJson1)
    {
        mpTmp = bufferJson2;
    }
    else
    {
        mpTmp = bufferJson1;
    }

    std::sprintf(mpTmp, "{ \"temp\" : \"%f\" ", mTemperature);

    mpJson = mpTmp;
}

void RobotBoard2Interpreter::enterProtected()
{
    pthread_mutex_lock(&mMutex);
}

void RobotBoard2Interpreter::exitProtected()
{
    pthread_mutex_unlock(&mMutex);
}

const char* RobotBoard2Interpreter::getJson()
{
    return (const char*)mpJson;
}

const char* RobotBoard2Interpreter::getHtml()
{
    return (const char*)mpHtml;
}

const char *RobotBoard2Interpreter::getPlaintext()
{
    return (const char*)mpPlaintext;
}

void RobotBoard2Interpreter::setLed(unsigned int ledNum, bool on)
{
    if(ledNum >= 8)
    {
        return;
    }

    if(on)
    {
        mTxBuff[MsgLengthLen + MsgTypeLen] |= (1 << ledNum);
    }
    else
    {
        mTxBuff[MsgLengthLen + MsgTypeLen] &= ~(1 << ledNum);
    }
}

void RobotBoard2Interpreter::setRightMotor(int percent)
{
    int32_t tmp = percent;
    tmp *= 32767;
    tmp /= 100;

    enterProtected();

    mTxBuff[MsgLengthLen + MsgTypeLen + LedsLen] = (tmp >> 8) & 0xFF;
    mTxBuff[MsgLengthLen + MsgTypeLen + LedsLen + 1] = tmp & 0xFF;

    exitProtected();
}

void RobotBoard2Interpreter::setLeftMotor(int percent)
{
    int32_t tmp = percent;
    tmp *= 32767;
    tmp /= 100;

    enterProtected();

    mTxBuff[MsgLengthLen + MsgTypeLen + LedsLen + MotorLen] = (tmp >> 8) & 0xFF;
    mTxBuff[MsgLengthLen + MsgTypeLen + LedsLen + MotorLen + 1] = tmp & 0xFF;

    exitProtected();
}

void RobotBoard2Interpreter::setServoPos(unsigned int servoNum, uint16_t pos)
{
    if(servoNum >= 8)
    {
        return;
    }

    enterProtected();

    mTxBuff[MsgLengthLen + MsgTypeLen + LedsLen + (2 * MotorLen) + (servoNum * ServoLen)] = (pos >> 8) & 0xFF;
    mTxBuff[MsgLengthLen + MsgTypeLen + LedsLen + (2 * MotorLen) + (servoNum * ServoLen) + 1] = pos & 0xFF;

    exitProtected();
}

void RobotBoard2Interpreter::setCameraPan(int degrees)
{
    if((degrees > 90) || (degrees < (-90)))
    {
        return;
    }

    int32_t tmp = degrees + 90;
    tmp *= 65535;
    tmp /= 180;

    setServoPos(0, tmp);
}

void RobotBoard2Interpreter::setCameraTilt(int degrees)
{
    if((degrees > 90) || (degrees < (-90)))
    {
        return;
    }

    int32_t tmp = degrees + 90;
    tmp *= 65535;
    tmp /= 180;

    setServoPos(1, tmp);
}
