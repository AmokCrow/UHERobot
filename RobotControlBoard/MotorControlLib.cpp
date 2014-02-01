#include <Wire.h>

#include "MotorControlLib.h"

void SeeedMC::init(uint8_t addr)
{
  I2CMotorDriverAdd = addr;
}

void SeeedMC::MotorControlBySignedBytes(char motA, char motB)
{
  unsigned char tmp = 0;
  
  if(motA > 0)
  {
    tmp = 0b1000;
  }
  else if(motA < 0)
  {
    tmp = 0b0100;
  }
  
  if(motB > 0)
  {
    tmp |= 0b0010;
  }
  else if(motB < 0)
  {
    tmp |= 0b0001;
  }
  
  MotorDirectionSet(tmp);
  
  // The signed range goes from 0 to 127 or 128, and the speed control goes to 255.
  // This is probably not optimal, but map was used elsewhere already, so using it doesn't take any more flash.
  unsigned char speedA = map(abs(motA), 0, 128, 0, 255);
  unsigned char speedB = map(abs(motB), 0, 128, 0, 255);
  
  MotorSpeedSetAB(speedA, speedB);
}

void SeeedMC::MotorSpeedSetAB(unsigned char MotorSpeedA , unsigned char MotorSpeedB)  {
  Wire.beginTransmission(I2CMotorDriverAdd); // transmit to device I2CMotorDriverAdd
  Wire.write(MotorSpeedSet);        // set pwm header 
  Wire.write(MotorSpeedA);              // send pwma 
  Wire.write(MotorSpeedB);              // send pwmb    
  Wire.endTransmission();    // stop transmitting
}

void SeeedMC::MotorPWMFrequenceSet(unsigned char Frequence)  {    
  Wire.beginTransmission(I2CMotorDriverAdd); // transmit to device I2CMotorDriverAdd
  Wire.write(PWMFrequenceSet);        // set frequence header
  Wire.write(Frequence);              //  send frequence 
  Wire.write(Nothing);              //  need to send this byte as the third byte(no meaning)  
  Wire.endTransmission();    // stop transmitting
}

void SeeedMC::MotorDirectionSet(unsigned char Direction)  {     //  Adjust the direction of the motors 0b0000 I4 I3 I2 I1
  Wire.beginTransmission(I2CMotorDriverAdd); // transmit to device I2CMotorDriverAdd
  Wire.write(DirectionSet);        // Direction control header
  Wire.write(Direction);              // send direction control information
  Wire.write(Nothing);              // need to send this byte as the third byte(no meaning)  
  Wire.endTransmission();    // stop transmitting 
}

void SeeedMC::MotorDriectionAndSpeedSet(unsigned char Direction,unsigned char MotorSpeedA,unsigned char MotorSpeedB)  {  //you can adjust the driection and speed together
  MotorDirectionSet(Direction);
  MotorSpeedSetAB(MotorSpeedA,MotorSpeedB);  
}


