/*
 * ServoTimer.c
 *
 * Created: 4.7.2015 23:33:53
 *  Author: crow
 */ 

#include "ServoTimer.h"

static TC0_t* ServoTimer_pTimer;
ServoTimer_Servo_t* ServoTimer_pServos;
volatile uint8_t ServoTimer_numServos;

volatile uint8_t ServoTimer_phaseCounter;
volatile uint16_t ServoTimer_cycleTimeRemaining;

#define ServoTimer_FullCycleTime 20000


void ServoTimer_init(TC0_t* pTimer, ServoTimer_Servo_t* pServos, uint8_t numServos)
{
    uint8_t i;
    
    // Initialize internal variables.
    ServoTimer_pTimer = pTimer;
    ServoTimer_pServos = pServos;
    ServoTimer_numServos = numServos;
    ServoTimer_phaseCounter = numServos;
    ServoTimer_cycleTimeRemaining = 1000;
    
    // Enable timer.
    tc_enable(pTimer);
    
    // Set callbacks.
    tc_set_overflow_interrupt_callback(pTimer, ServoTimer_timerCallback);
    
    // Medium level interrupt, because it is soft-critical to get the timing right.
    tc_set_overflow_interrupt_level(pTimer, TC_INT_LVL_MED);
    
    // Disable PWM modes if any.
    tc_set_wgm(pTimer, TC_WGMODE_NORMAL_gc);
    
    for(i = 0; i < numServos; i++)
    {
        ioport_set_pin_dir(ServoTimer_pServos[i].pin, IOPORT_DIR_OUTPUT);
        ioport_set_pin_level(ServoTimer_pServos[i].pin, 0);
    }
    
    // Call the callback to start the cycle.
    ServoTimer_timerCallback();
    
    // Set clock source. This will start the timer.
    tc_write_clock_source(pTimer, TC_CLKSEL_DIV8_gc);
}

void ServoTimer_timerCallback(void)
{
    uint16_t nextTimePeriod;
    
    // If all servo pins have been gone through.
    if(ServoTimer_phaseCounter >= ServoTimer_numServos)
    {
        // Reset the last servo pin.
        if(ServoTimer_numServos > 0)
        {
            ioport_set_pin_level(ServoTimer_pServos[ServoTimer_phaseCounter - 1].pin, false);
        }
        
        tc_write_period(ServoTimer_pTimer, ServoTimer_cycleTimeRemaining);
        ServoTimer_cycleTimeRemaining = ServoTimer_FullCycleTime;
        ServoTimer_phaseCounter = 0;
        return;
    }
    
    // Reset previous pin, if any.
    if(ServoTimer_phaseCounter > 0)
    {
        ioport_set_pin_level(ServoTimer_pServos[ServoTimer_phaseCounter - 1].pin, false);
    }
    
    // The time period for the pin is between 1 and 2 milliseconds.
    nextTimePeriod = (ServoTimer_pServos[ServoTimer_phaseCounter].pos * 4) + 1000;
    
    // Set the next servo pin.
    ioport_set_pin_level(ServoTimer_pServos[ServoTimer_phaseCounter].pin, true);
    
    // The time the pin needs to be up. Will also be deducted from the time-remaining to keep the cycle constant.
    tc_write_period(ServoTimer_pTimer, nextTimePeriod);
    ServoTimer_cycleTimeRemaining -= nextTimePeriod;
    
    ServoTimer_phaseCounter++;
}
