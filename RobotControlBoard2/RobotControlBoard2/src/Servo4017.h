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
    Servo4017(){}
    
    // Warning!: gpio_local_init() MUST BE DONE BEFORE calling this function.
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
    volatile uint16_t mTimerChannel;
    
    // Pins to use for output
    volatile uint16_t mClkPin;
    volatile uint16_t mRstPin;
    
    
    // Indicates the servo channel currently being counted out
    volatile uint16_t mServoNum;
    
    // The positions of the servos, in timer ticks, plus 0.25ms added for minimum interrupt execution time
    uint16_t mServoPositions[numServoChannels];
    
    // In phase 0, the Clk pin is set high for 0.75ms. For phase 1, Clk is set low for the duration of 0.25ms + (1.0ms * servo position).
    // Phase 3 happens when the last servo has been through phase 1, and in it the Rst pin is set high for 1.0ms.
    // In phase 4 the Rst pin is set low for 20.0ms - (sum of all servos total signalling time). The 20.0ms is
    // in theory not necessary to be timed accurately. However, some rare servos and other effectors may be sensitive, so time it out.
    // Phase 4 will be divided to parts that can be counted with the timer channel's current 6.5ms maximum.
    volatile uint8_t mPhase;
    
    // Keeps count of how much longer the current timing cycle must last. All servo signalling times will be deducted
    // and the remainder waited out at the end of each cycle.
    volatile uint32_t mCycleTimeLeft;
    
    // Are the output pins inverted in functionality?
    //  Note: Documentative comments do not take into account 
    //  output polarity unless specifically stating otherwise.
    volatile uint8_t mPinsInverted;
};


    

    
    
    
    
#endif //__SERVO4017_H__
