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

    for(unsigned int i = 0; i < 8; i++)
    {
        setServoPos(i, 90);
    }
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
    if((buffer[0] == 1) && length >= 12)
    {
        extractAdcValues(&(buffer[2]), length - 2);
    }
}

void RobotBoard2Interpreter::extractAdcValues(const uint8_t* buffer, uint16_t length)
{
    int16_t iTmp;
    uint16_t tmp;

    // --PCB temperature--
    iTmp = buffer[2] << 8;
    iTmp |= buffer[3];

    mTemperature = (iTmp * 1.0f) / 10.0f;

    // --CPU current--
    tmp = buffer[4] << 8;
    tmp |= buffer[5];

    mCpuCurrent = (tmp * 1.0f) / 1000.0f;

    // --CPU battery voltage--
    tmp = buffer[6] << 8;
    tmp |= buffer[7];

    mCpuVoltage = (tmp * 1.0f) / 1000.0f;

    // --Motor current--
    tmp = buffer[8] << 8;
    tmp |= buffer[9];

    mMotorCurrent = (tmp * 1.0f) / 1000.0f;

    // --Motor voltage--
    tmp = buffer[10] << 8;
    tmp |= buffer[11];

    mMotorVoltage = (tmp * 1.0f) / 1000.0f;
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

    std::sprintf(mpTmp, "<p>Temperature sensor: %fV<br> CPU Battery: %fV <br> CPU Current: %fA<br>Motor Voltage: %fV<br>Motor Current: %fA</p>",
                 mTemperature, mCpuVoltage, mCpuCurrent, mMotorVoltage ,mMotorCurrent);

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

    std::sprintf(mpTmp, "{ \"temp\": %.1f, \"mv\": %.1f, \"mc\": %.1f, \"cv\": %.1f, \"cc\": %.1f }",
                 mTemperature, mMotorVoltage, mMotorCurrent, mCpuVoltage, mCpuCurrent );

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
        mTxBuff[MsgLengthLen + MsgTypeLen + MotorsTotalLen + ServosTotalLen] |= (1 << ledNum);
    }
    else
    {
        mTxBuff[MsgLengthLen + MsgTypeLen + MotorsTotalLen + ServosTotalLen] &= ~(1 << ledNum);
    }
}

void RobotBoard2Interpreter::setRightMotor(int percent)
{
    if(percent > 100)
    {
        percent = 100;
    }
    else if(percent < -100)
    {
        percent = -100;
    }

    int16_t tmp = percent;
    tmp *= 32;

    enterProtected();

    mTxBuff[MsgLengthLen + MsgTypeLen] = (tmp >> 8) & 0xFF;
    mTxBuff[MsgLengthLen + MsgTypeLen + 1] = tmp & 0xFF;

    exitProtected();
}

void RobotBoard2Interpreter::setLeftMotor(int percent)
{
    if(percent > 100)
    {
        percent = 100;
    }
    else if(percent < -100)
    {
        percent = -100;
    }

    int16_t tmp = percent;
    tmp *= 32;

    enterProtected();

    mTxBuff[MsgLengthLen + MsgTypeLen + MotorLen] = (tmp >> 8) & 0xFF;
    mTxBuff[MsgLengthLen + MsgTypeLen + MotorLen + 1] = tmp & 0xFF;

    exitProtected();
}

void RobotBoard2Interpreter::setServoPos(unsigned int servoNum, uint16_t pos)
{
    if(servoNum >= 8)
    {
        return;
    }

    if(pos > 250)
    {
        pos = 250;
    }

    enterProtected();

    mTxBuff[MsgLengthLen + MsgTypeLen + (2 * MotorLen) + (servoNum * ServoLen)] = (pos >> 8) & 0xFF;
    mTxBuff[MsgLengthLen + MsgTypeLen + (2 * MotorLen) + (servoNum * ServoLen) + 1] = pos & 0xFF;

    exitProtected();
}

void RobotBoard2Interpreter::setCameraPan(int degrees)
{
    if((degrees > 180) || (degrees < 0))
    {
        return;
    }

    float fTmp = degrees;

    fTmp /= 180.0f;
    fTmp *= 250.0f;

    setServoPos(0, fTmp);
}

void RobotBoard2Interpreter::setCameraTilt(int degrees)
{
    if((degrees > 180) || (degrees < 0))
    {
        return;
    }

    float fTmp = degrees;

    fTmp /= 180.0f;
    fTmp *= 250.0f;

    setServoPos(1, fTmp);
}
