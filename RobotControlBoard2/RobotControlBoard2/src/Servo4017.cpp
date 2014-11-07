/* 
* Servo4017.cpp
*
* Created: 11.10.2014 11:38:02
* Author: crow
*/


#include "Servo4017.h"

#include <tc.h>
#include <gpio.h>

void Servo4017::init(uint16_t gpioPinRst, uint16_t gpioPinClk, volatile avr32_tc_t *timer, uint16_t timerChannel, uint8_t pinsInverted)
{
    // Set the initial position to be at 50% of range.
    // That is, start centered.
    for(uint16_t i = 0; i < numServoChannels; i++)
    {
        mServoPositions[i] = 32768;
    }
    
    mServoNum = 0;
    
    mClkPin = gpioPinClk;
    mRstPin = gpioPinRst;
    mPinsInverted = pinsInverted;
    
    pmTimer = timer;
    mTimerChannel = timerChannel;
    
    // Initialize to the end of the cycle with reset high and clock low.
    mCycleTimeLeft = 1000;
    mPhase = 3;
    
    gpio_enable_gpio_pin(mClkPin);
    gpio_enable_gpio_pin(mRstPin);
    
    // Take into account the possibly reversed state.
    if(mPinsInverted == 0)
    {
        gpio_configure_pin(mRstPin, (GPIO_DIR_OUTPUT | GPIO_INIT_HIGH));
        gpio_configure_pin(mClkPin, (GPIO_DIR_OUTPUT | GPIO_INIT_LOW));
    }
    else
    {
        gpio_configure_pin(mRstPin, (GPIO_DIR_OUTPUT | GPIO_INIT_LOW));
        gpio_configure_pin(mClkPin, (GPIO_DIR_OUTPUT | GPIO_INIT_HIGH));
    }
    
    gpio_local_enable_pin_output_driver(mRstPin);
    gpio_local_enable_pin_output_driver(mClkPin);
    
    
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
    tcOpts.cpcdis = false;  // Don't disable on RC compare
    tcOpts.cpcstop = false; // Don't stop on RC compare
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
    
    // Start the timer. 1.0ms is given for the assertion of reset to take effect.
    tc_write_rc(pmTimer, mTimerChannel, 1000);
    tc_start(pmTimer, mTimerChannel);
}

void Servo4017::setPos(uint16_t servo, uint16_t pos)
{
    if(servo >= numServoChannels)
    {
        return;
    }
    
    // Fit the desired servo position to the timing range.
    uint32_t tmp = pos;
    tmp *= 10000UL;
    tmp /= 65535UL;
    
    // Add the minimum interrupt protection part.
    // (It's deducted elsewhere, so don't worry about that.)
    tmp += 2500;
    
    mServoPositions[servo] = tmp;
}

void Servo4017::interruptCallback(void)
{
    tc_read_sr(pmTimer, mTimerChannel);
    
    // Operate as non-volatile in the outermost if-structure
    uint16_t tmp = mPhase;
    
    // This was the 0.75ms high phase for Clk pin
    if(tmp == 0)
    {
        // Set Clk low for Phase 1.
        if(mPinsInverted == 0)
        {
            gpio_local_clr_gpio_pin(mClkPin);
        }
        else
        {
            gpio_local_set_gpio_pin(mClkPin);
        }
        
        // Set the timer and calculate the remaining time
        tc_write_rc(pmTimer, mTimerChannel, mServoPositions[mServoNum] / 2);
        mCycleTimeLeft -= mServoPositions[mServoNum];
        mServoNum++;
        
        mPhase = 1;
    }
    // This was the 0.25ms + (servo position) low time for the Clk pin
    else if(tmp == 1)
    {
        // Not the last servo yet.
        if(mServoNum < numServoChannels)
        {
            // Set Clk high and calculate and set the time
            if(mPinsInverted == 0)
            {
                gpio_local_set_gpio_pin(mClkPin);
            }
            else
            {
                gpio_local_clr_gpio_pin(mClkPin);
            }
            
            mPhase = 0;
            
            tc_write_rc(pmTimer, mTimerChannel, 7500 / 2);
        }
        // Was last servo
        else
        {
            mPhase = 3;
            
            // Set Rst high
            if(mPinsInverted == 0)
            {
                gpio_local_set_gpio_pin(mRstPin);
            }
            else
            {
                gpio_local_clr_gpio_pin(mRstPin);
            }
            
            // The remaining cycle time is 4-12ms. Half that is 2 to 6 ms.
            //  The maximum 60,000 ticks just fits to the counter.
            tc_write_rc(pmTimer, mTimerChannel, mCycleTimeLeft/4);
        }
        
    }
    else if(tmp == 3)
    {
        mPhase = 4;
        
        // Set Rst low
        if(mPinsInverted == 0)
        {
            gpio_local_clr_gpio_pin(mRstPin);
        }
        else
        {
            gpio_local_set_gpio_pin(mRstPin);
        }
        
        // The remaining cycle time, 2 to 6 ms.
        tc_write_rc(pmTimer, mTimerChannel, mCycleTimeLeft/4);
    }
    else // tmp == 4, hopefully...
    {
        // Phase 4 is always followed by phase 0.
        mPhase = 0;
        
        // Full cycle time is 20ms = 200,000 ticks.
        mCycleTimeLeft = 200000;
        
        // Start from the first servo again.
        mServoNum = 0;
        
        if(mPinsInverted == 0)
        {
            gpio_local_set_gpio_pin(mClkPin);
        }
        else
        {
            gpio_local_clr_gpio_pin(mClkPin);
        }
        
        
    }
}