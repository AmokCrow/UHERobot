
#ifndef MOTOR_CONTROL_LIB__H
#define MOTOR_CONTROL_LIB__H

#include <Arduino.h>
#include <Wire.h>
#include "CommandList.h"

class SeeedMC
{
public:
  static const uint8_t MotorSpeedSet = 0x82;
  static const uint8_t PWMFrequenceSet = 0x84;
  static const uint8_t DirectionSet = 0xaa;
  static const uint8_t MotorSetA = 0xa1;
  static const uint8_t MotorSetB = 0xa5;
  static const uint8_t Nothing = 0x01;
  
  void init(uint8_t addr);
  void MotorControlBySignedBytes(char motA, char motB);
  void MotorSpeedSetAB(unsigned char MotorSpeedA , unsigned char MotorSpeedB);
  void MotorPWMFrequenceSet(unsigned char Frequence);
  void MotorDirectionSet(unsigned char Direction);
  void MotorDriectionAndSpeedSet(unsigned char Direction,unsigned char MotorSpeedA,unsigned char MotorSpeedB);
  
private:
  uint8_t I2CMotorDriverAdd;
  
};

#endif // MOTOR_CONTROL_LIB__H

