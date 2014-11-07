/* 
* MotorDriver.cpp
*
* Created: 31.10.2014 23:11:18
* Author: crow
*/


#include "MotorDriver.h"

#include <tc.h>
#include <gpio.h>

// default constructor
MotorDriver::MotorDriver()
{
} //MotorDriver

void MotorDriver::init(uint8_t gpioPin1, uint8_t pin1TmrFunc, uint8_t gpioPin2, uint8_t pin2TmrFunc, volatile avr32_tc_t* timer, uint16_t timerChannel, uint16_t frequency, uint8_t dirInverted)
{
    mGpioPin1 = gpioPin1;
    mGpioPin2 = gpioPin2;
    mTimer = timer;
    mTimerChannel = timerChannel;
    mDirInv = dirInverted;
    mSpeed = 0;
    mPin1TmrFunc = pin1TmrFunc;
    mPin2TmrFunc = pin2TmrFunc;
    
    gpio_enable_gpio_pin(mGpioPin1);
    gpio_enable_gpio_pin(mGpioPin2);
    
    gpio_local_clr_gpio_pin(mGpioPin1);
    gpio_local_clr_gpio_pin(mGpioPin2);
    gpio_configure_pin(mGpioPin1, (GPIO_DIR_OUTPUT | GPIO_INIT_LOW));
    gpio_configure_pin(mGpioPin2, (GPIO_DIR_OUTPUT | GPIO_INIT_LOW));
    
    // TODO: Hardware init
    tc_waveform_opt_t tcOpts;
    tcOpts.channel = mTimerChannel;
    tcOpts.bswtrg = TC_EVT_EFFECT_NOOP;
    tcOpts.beevt = TC_EVT_EFFECT_NOOP;
    tcOpts.bcpc = TC_EVT_EFFECT_CLEAR;
    tcOpts.bcpb = TC_EVT_EFFECT_SET;
    tcOpts.aswtrg = TC_EVT_EFFECT_NOOP;
    tcOpts.aeevt = TC_EVT_EFFECT_NOOP;
    tcOpts.acpc = TC_EVT_EFFECT_CLEAR;
    tcOpts.acpa = TC_EVT_EFFECT_SET;
    tcOpts.wavsel = TC_WAVEFORM_SEL_UP_MODE_RC_TRIGGER;
    tcOpts.enetrg = false;
    tcOpts.eevt = TC_EXT_EVENT_SEL_XC0_OUTPUT;
    tcOpts.eevtedg = TC_SEL_NO_EDGE;
    tcOpts.cpcdis = false;  // Don't disable on RC compare
    tcOpts.cpcstop = false; // Don't stop on RC compare
    tcOpts.burst = TC_BURST_NOT_GATED;
    tcOpts.clki = TC_CLOCK_RISING_EDGE;
    tcOpts.tcclks = TC_CLOCK_SOURCE_TC2; // = PBAclock / 2
    
    int resp = tc_init_waveform (mTimer, &tcOpts);
    
    tc_interrupt_t tcInts;
    tcInts.covfs = false;
    tcInts.cpas = false;
    tcInts.cpbs = false;
    tcInts.cpcs = false;
    tcInts.etrgs = false;
    tcInts.ldras = false;
    tcInts.ldrbs = false;
    tcInts.lovrs = false;
    
    tc_configure_interrupts(mTimer, mTimerChannel, &tcInts);
    
    // This should make for a 2.5kHz output wave.
    // Start with an all-low output though.
    tc_write_ra(mTimer, mTimerChannel, 4001);
    tc_write_rb(mTimer, mTimerChannel, 4001);
    tc_write_rc(mTimer, mTimerChannel, 4000);
    tc_start(mTimer, mTimerChannel);
}

void MotorDriver::setSpeed(int16_t speed)
{
    if(mSpeed == speed)
    {
        return;
    }
    
    if(speed == -32769)
    {
        speed = -32768;
    }
    
    if(speed == 0)
    {
        gpio_enable_gpio_pin(mGpioPin1);
        gpio_enable_gpio_pin(mGpioPin2);
        
        gpio_local_clr_gpio_pin(mGpioPin1);
        gpio_local_clr_gpio_pin(mGpioPin2);
    }
    else if(speed > 0)
    {
        // Maximum is 4000
        uint32_t tmpSpeed = 4000 - (((uint32_t)speed * 4000) >> 15);
        
        // Direction change needed
        if(mSpeed <= 0)
        {
            if(mDirInv)
            {
                gpio_enable_gpio_pin(mGpioPin2);
                gpio_local_clr_gpio_pin(mGpioPin2);
                gpio_enable_module_pin(mGpioPin1, mPin1TmrFunc);
            }
            else
            {
                gpio_enable_gpio_pin(mGpioPin1);
                gpio_local_clr_gpio_pin(mGpioPin1);
                gpio_enable_module_pin(mGpioPin2, mPin2TmrFunc);
            }
        }
        
        tc_write_ra(mTimer, mTimerChannel, tmpSpeed);
        tc_write_rb(mTimer, mTimerChannel, tmpSpeed);
    }
    else // speed < 0
    {
        // Maximum is 4000
        uint32_t tmpSpeed = 4000 - (((uint32_t)speed * -4000) >> 15);
        
        // Direction change needed
        if(mSpeed >= 0)
        {
            if(mDirInv)
            {
                gpio_enable_gpio_pin(mGpioPin1);
                gpio_local_clr_gpio_pin(mGpioPin1);
                gpio_enable_module_pin(mGpioPin2, mPin2TmrFunc);
            }
            else
            {
                gpio_enable_gpio_pin(mGpioPin2);
                gpio_local_clr_gpio_pin(mGpioPin2);
                gpio_enable_module_pin(mGpioPin1, mPin1TmrFunc);
            }
        }
        
        tc_write_ra(mTimer, mTimerChannel, tmpSpeed);
        tc_write_rb(mTimer, mTimerChannel, tmpSpeed);
    }
    
    mSpeed = speed;
}

int16_t MotorDriver::getSpeed()
{
    return mSpeed;
}
