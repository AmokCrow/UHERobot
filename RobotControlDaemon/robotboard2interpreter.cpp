#include "robotboard2interpreter.h"
#include <cstring>
#include <cstdint>

#include <iostream>
#include <cstdio>

const char RobotBoard2Interpreter::mStrBattCap[] = "Battery Capacity";
const char RobotBoard2Interpreter::mStrPercent[] = "%";
const char RobotBoard2Interpreter::mStrBattVolt[] = "Battery Voltage";
const char RobotBoard2Interpreter::mStrVolt[] = "V";
const char RobotBoard2Interpreter::mStrCurrent[] = "I";
const char RobotBoard2Interpreter::mStrBrdTemp[] = "Board temperature";
const char RobotBoard2Interpreter::mStrTemp[] = "C";
const char RobotBoard2Interpreter::mStrBrdCurr[] = "Board current";
const char RobotBoard2Interpreter::mStrJsonBattVolt[] = "bv";
const char RobotBoard2Interpreter::mStrJsonBrdCurr[] = "bc";
const char RobotBoard2Interpreter::mStrJsonBrdTemp[] = "bt";
const char RobotBoard2Interpreter::mStrJsonBattStat[] = "bs";

RobotBoard2Interpreter::RobotBoard2Interpreter()
{
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

    mTxBuff[0] = 23;
    mTxBuff[1] = 0;

    for(unsigned int i = 2; i < TX_BUFF_SIZE; i++)
    {
        mTxBuff[i] = 0;
    }

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
    if(!((buffLength == 8) && (buff[1] == 1) && (buff[0] == 8)))
    {
        std::cout << "Bad msg: len " << (unsigned int)buffLength << " - reported " << (unsigned int)buff[0] << ", type " << (unsigned int)buff[1] << std::endl;
        return;
    }

    float ftmp;
    unsigned int itmp;

    itmp = (buff[2] << 8) | buff[3];
    ftmp = itmp;
    ftmp /= 100.0f;
    printableParams[BattVoltage].value = ftmp;
    // Battery voltage area is approximately 12.6V - 8V.
    // This is a bad estimation, but shall suffice for now.
    printableParams[BatteryStatus].value = ((ftmp - 8.0f) / (12.6f - 8.0f)) * 100.0f;

    itmp = (buff[4] << 8) | buff[5];
    ftmp = itmp;
    printableParams[BatteryCurrent].value = ftmp;

    itmp = (buff[6] << 8) | buff[7];
    ftmp = itmp;
    ftmp /= 100.0f;
    printableParams[Temperature].value = ftmp;
}

void RobotBoard2Interpreter::enterProtected()
{
    pthread_mutex_lock(&mMutex);
}

void RobotBoard2Interpreter::exitProtected()
{
    pthread_mutex_unlock(&mMutex);
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
