/* 
* Servo4017.h
*
* Created: 11.10.2014 11:38:02
* Author: crow
*/


#ifndef __SERVO4017_H__
#define __SERVO4017_H__

#include <stdint.h>
#include <tc.h>



class Servo4017
{
public:
    Servo4017();
    ~Servo4017(){}
    
    // Init must be called before any other action is taken.
    // It will initialize the dependencies. Namely the timer interface.
    void init(uint16_t gpioPinRst, uint16_t gpioPinClk, volatile avr32_tc_t *timer, uint16_t timerChannel, uint8_t pinsInverted);
        
    // uint16_t represents full scale of motion for servo,
    // or that of 1.0ms..2.0ms output pulse length for servo driver.
    // Halfway-point is therefore max(uint16_t) / 2
    void setPos(uint16_t servo, uint16_t pos);
    
    void interruptCallback(void);    
private:
    Servo4017( const Servo4017 &c );
    Servo4017& operator=( const Servo4017 &c );
    
    static const uint16_t numServoChannels = 8;
    
    // Timer bits
    volatile avr32_tc_t *pmTimer;
    uint16_t mTimerChannel;
    
    // Pins to use for output
    uint16_t mClkPin;
    uint16_t mRstPin;
    
    // The positions of the servos
    volatile uint16_t mNextServo;
    uint16_t mServoPositions[numServoChannels];
    
    
    // Are the output pins inverted in functionality?
    uint8_t mPinsInverted;
};


    

    
    
    
    
#endif //__SERVO4017_H__
