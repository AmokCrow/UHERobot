/* 
* MotorDriver.h
*
* Created: 31.10.2014 23:11:18
* Author: crow
*/


#ifndef __MOTORDRIVER_H__
#define __MOTORDRIVER_H__

#include <stdint.h>
#include <tc.h>

class MotorDriver
{
//variables
public:
protected:
private:

//functions
public:
	MotorDriver();
    
    void init(uint8_t gpioPin1, uint8_t pin1TmrFunc, uint8_t gpioPin2, uint8_t pin2PwmFunc, volatile avr32_tc_t* timer, uint16_t timerChannel, uint16_t frequency, uint8_t dirInverted);
    void setSpeed(int16_t speed);
    int16_t getSpeed();
protected:
private:
	MotorDriver( const MotorDriver &c );
	MotorDriver& operator=( const MotorDriver &c );
    
    int16_t mSpeed;
    uint8_t mDirInv;
    volatile avr32_tc_t* mTimer;
    uint16_t mTimerChannel;
    uint8_t mGpioPin1;
    uint8_t mGpioPin2;
    uint8_t mPin1TmrFunc;
    uint8_t mPin2TmrFunc;
}; //MotorDriver

#endif //__MOTORDRIVER_H__
