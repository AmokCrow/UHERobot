/* 
* Servo4017.cpp
*
* Created: 11.10.2014 11:38:02
* Author: crow
*/


#include "Servo4017.h"

#include <tc.h>

void Servo4017::init(uint16_t gpioPinRst, uint16_t gpioPinClk, volatile avr32_tc_t *timer, uint16_t timerChannel, uint8_t pinsInverted)
{
    // Set the initial position to be at 50% of range.
    // That is, start centered.
    for(uint16_t i = 0; i < numServoChannels; i++)
    {
        mServoPositions[i] = 32768;
    }
    
    mNextServo = 0;
    
    mClkPin = gpioPinRst;
    mRstPin = gpioPinRst;
    mPinsInverted = pinsInverted;
    
    pmTimer = timer;
    mTimerChannel = timerChannel;
    
    // TODO: Hardware init
    tc_waveform_opt_t tcOpts;
    tcOpts.channel = mTimerChannel;
    tcOpts.bswtrg = TC_EVT_EFFECT_NOOP;
    tcOpts.beevt = TC_EVT_EFFECT_NOOP;
    tcOpts.bcpc = TC_EVT_EFFECT_NOOP;
    tcOpts.bcpb = TC_EVT_EFFECT_NOOP;
    tcOpts.aswtrg = TC_EVT_EFFECT_NOOP;
    tcOpts.aeevt = TC_EVT_EFFECT_NOOP;
    tcOpts.acpc = TC_EVT_EFFECT_NOOP;
    tcOpts.acpa = TC_EVT_EFFECT_NOOP;
    tcOpts.wavsel = TC_WAVEFORM_SEL_UP_MODE_RC_TRIGGER;
    tcOpts.enetrg = false;
    tcOpts.eevt = TC_EXT_EVENT_SEL_XC0_OUTPUT;
    tcOpts.eevtedg = TC_SEL_NO_EDGE;
    tcOpts.cpcdis = false;
    tcOpts.cpcstop = false;
    tcOpts.burst = TC_BURST_NOT_GATED;
    tcOpts.clki = TC_CLOCK_RISING_EDGE;
    tcOpts.tcclks = TC_CLOCK_SOURCE_TC2; // = PBAclock / 2

    // Init timer 1 for servo and second-counter duty.
    int resp = tc_init_waveform (pmTimer, &tcOpts);
    
    tc_interrupt_t tcInts;
    tcInts.covfs = false;
    tcInts.cpas = false;
    tcInts.cpbs = false;
    tcInts.cpcs = true;
    tcInts.etrgs = false;
    tcInts.ldras = false;
    tcInts.ldrbs = false;
    tcInts.lovrs = false;
    
    tc_configure_interrupts(pmTimer, mTimerChannel, &tcInts);
    
    // TODO: Check the pin invertedness and:
    // TODO: Reset the 4017.
    // TODO: Start first inter-sequence wait by setting the channel's RC to 10ms ( = 10MHz / 0.01 => 10000? ).
}

void Servo4017::setPos(uint16_t servo, uint16_t pos)
{
    if(servo >= numServoChannels)
    {
        return;
    }
    
    mServoPositions[servo] = pos;
}

void Servo4017::interruptCallback(void)
{
    
}