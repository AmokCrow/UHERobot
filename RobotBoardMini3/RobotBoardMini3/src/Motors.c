/*
 * Motors.c
 *
 * Created: 23.1.2016 10:22:09
 *  Author: crow
 */ 

#include "Motors.h"

#include <avr32/uc3l032.h>
#include "pm_uc3l.h"
#include "scif_uc3l.h"
#include <asf.h>

static const uint64_t channel_mask[4] = 
{
    (1 << 19),
    (1 << 18),
    (1 << 10),
    (1 << 12)
};

#define NUM_MOTORS  2
#define TOP_VALUE   200

// With a 8MHz PBA (and system) clock, this should give about 1.6MHz clock into PWMA.
static const scif_gclk_opt_t scif_genclock_opts = 
{
    SCIF_GCCTRL_PBACLOCK,
    5,
    1,
    8000000UL
};

void motor_init(void)
{
    // Enable the clock input with settings.
    scif_start_gclk(AVR32_PM_GCLK_PWMA, &scif_genclock_opts);
    
    // Enable the channels. This should set the TOP value to around 200.
    pwma_config_enable(&AVR32_PWMA, 6000UL, 6000UL * 200UL, 0);
    
    motor_set_speed(0, 0);
    motor_set_speed(0, 1);
}

void motor_set_speed(int speed, uint16_t motorNum)
{
    if (motorNum > NUM_MOTORS)
    {
        return;
    }
    
    uint16_t motor_pin_l = motorNum * 2;
    uint16_t motor_pin_r = (motorNum * 2) + 1;
    uint16_t spd_abs;
    
    if (speed < 0)
    {
        spd_abs = speed * (-1);
    }
    else
    {
        spd_abs = speed;
    }
    
    if (spd_abs >= 100)
    {
        spd_abs = 0;
    }
    else
    {
        spd_abs = TOP_VALUE - (spd_abs * 2);
    }
    
    if (speed == 0)
    {
        pwma_set_channels_value(&AVR32_PWMA, channel_mask[motor_pin_l] | channel_mask[motor_pin_r], TOP_VALUE);
    }
    else if (speed > 0)
    {
        pwma_set_channels_value(&AVR32_PWMA, channel_mask[motor_pin_l], TOP_VALUE);
        pwma_set_channels_value(&AVR32_PWMA, channel_mask[motor_pin_r], spd_abs);
    }
    else
    {
        pwma_set_channels_value(&AVR32_PWMA, channel_mask[motor_pin_r], TOP_VALUE);
        pwma_set_channels_value(&AVR32_PWMA, channel_mask[motor_pin_l], spd_abs);
    }
}