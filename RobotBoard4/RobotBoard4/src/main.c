/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# "Insert system clock initialization code here" comment
 * -# Minimal main function that starts with a call to board_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
#include <asf.h>

#include "CommandInterpreter.h"

#define PIN_DEBUG_TX IOPORT_CREATE_PIN(PORTD, 3)
#define PIN_DEBUG_RX IOPORT_CREATE_PIN(PORTD, 2)
#define PIN_PWR_CPU  IOPORT_CREATE_PIN(PORTD, 7)

#define PIN_CMD_TX   IOPORT_CREATE_PIN(PORTE, 3)
#define PIN_CMD_RX   IOPORT_CREATE_PIN(PORTE, 2)

#define PIN_MOT_OC0ALS IOPORT_CREATE_PIN(PORTC, 0)
#define PIN_MOT_OC0AHS IOPORT_CREATE_PIN(PORTC, 1)
#define PIN_MOT_OC0BLS IOPORT_CREATE_PIN(PORTC, 2)
#define PIN_MOT_OC0BHS IOPORT_CREATE_PIN(PORTC, 3)
#define PIN_MOT_OC0CLS IOPORT_CREATE_PIN(PORTC, 4)
#define PIN_MOT_OC0CHS IOPORT_CREATE_PIN(PORTC, 5)
#define PIN_MOT_OC0DLS IOPORT_CREATE_PIN(PORTC, 6)
#define PIN_MOT_OC0DHS IOPORT_CREATE_PIN(PORTC, 7)

void init_avr_asf_libraries(void)
{
    board_init();
    pmic_init();
    sysclk_init();
    irq_initialize_vectors();
    cpu_irq_enable();
    ioport_init();
}

#define MOTOR_TIMER_PERIOD 3200
#define MOTOR_ALWAYS_ON_TIMER_VALUE (MOTOR_TIMER_PERIOD + 10)

void configure_motor_clock(void)
{
    tc_enable(&TCC0);
    tc_write_period(&TCC0, MOTOR_TIMER_PERIOD);
    tc_enable_cc_channels(&TCC0, TC_CCAEN | TC_CCBEN | TC_CCCEN | TC_CCDEN );
    
    tc_write_cc(&TCC0, TC_CCA, MOTOR_ALWAYS_ON_TIMER_VALUE);
    tc_write_cc(&TCC0, TC_CCB, MOTOR_ALWAYS_ON_TIMER_VALUE);
    tc_write_cc(&TCC0, TC_CCC, MOTOR_ALWAYS_ON_TIMER_VALUE);
    tc_write_cc(&TCC0, TC_CCD, MOTOR_ALWAYS_ON_TIMER_VALUE);
    
    tc_set_wgm(&TCC0, TC_WG_SS);
    
    //tc_awex_enable_cwcm(&AWEXC);
    tc_awex_enable_cca_deadtime(&AWEXC);
    tc_awex_enable_ccb_deadtime(&AWEXC);
    tc_awex_enable_ccc_deadtime(&AWEXC);
    tc_awex_enable_ccd_deadtime(&AWEXC);
    
    tc_awex_set_dti_both(&AWEXC, 0x80);
    tc_awex_set_dti_both_buffer(&AWEXC, 0x80);
    tc_awex_set_output_override(&AWEXC, 0xFF);
}

void start_motor_clock(void)
{
    //tc_write_clock_source(&TCC0, TC_CLKSEL_DIV1024_gc);
    tc_write_clock_source(&TCC0, TC_CLKSEL_DIV1_gc);
}

void stop_motor_clock(void)
{
    tc_write_clock_source(&TCC0, TC_CLKSEL_OFF_gc);
}

void enable_motor_pins(void)
{
    ioport_set_pin_dir(PIN_MOT_OC0ALS, IOPORT_DIR_OUTPUT);
    ioport_set_pin_dir(PIN_MOT_OC0AHS, IOPORT_DIR_OUTPUT);
    ioport_set_pin_dir(PIN_MOT_OC0BLS, IOPORT_DIR_OUTPUT);
    ioport_set_pin_dir(PIN_MOT_OC0BHS, IOPORT_DIR_OUTPUT);
    ioport_set_pin_dir(PIN_MOT_OC0CLS, IOPORT_DIR_OUTPUT);
    ioport_set_pin_dir(PIN_MOT_OC0CHS, IOPORT_DIR_OUTPUT);
    ioport_set_pin_dir(PIN_MOT_OC0DLS, IOPORT_DIR_OUTPUT);
    ioport_set_pin_dir(PIN_MOT_OC0DHS, IOPORT_DIR_OUTPUT);
}

// Motor 0 is on pins PC0..PC3. Motor 1 is PC4..PC7.
uint8_t motor0_direction = 0; // 0 for positive speed, 1 for negative speed, 2 for "no change
uint8_t motor1_direction = 0; // 0 for positive speed, 1 for negative speed
uint8_t motors_out_ena_pattern = 0;

void motor0_set_speed(uint8_t direction, uint16_t speed)
{
    if(direction != motor0_direction)
    {
        motor0_direction = direction;
        
        if(direction == 0)
        {
            tc_write_cc_buffer(&TCC0, TC_CCB, MOTOR_ALWAYS_ON_TIMER_VALUE);
        }
        else
        {
            tc_write_cc_buffer(&TCC0, TC_CCA, MOTOR_ALWAYS_ON_TIMER_VALUE);
        }
    }
    
    if(direction == 0)
    {
        tc_write_cc_buffer(&TCC0, TC_CCA, speed);
    }
    else
    {
        tc_write_cc_buffer(&TCC0, TC_CCB, speed);
    }
}

void motor1_set_speed(uint8_t direction, uint16_t speed)
{
    if(direction != motor1_direction)
    {
        motor1_direction = direction;
        
        if(direction == 0)
        {
            tc_write_cc_buffer(&TCC0, TC_CCD, MOTOR_ALWAYS_ON_TIMER_VALUE);
        }
        else
        {
            tc_write_cc_buffer(&TCC0, TC_CCC, MOTOR_ALWAYS_ON_TIMER_VALUE);
        }
    }
    
    if(direction == 0)
    {
        tc_write_cc_buffer(&TCC0, TC_CCC, speed);
    }
    else
    {
        tc_write_cc_buffer(&TCC0, TC_CCD, speed);
    }
}

void motor_set_speed(int16_t speed, uint8_t motor)
{
    uint8_t direction = 0;
    
    if(speed >= 0)
    {
        direction = 0;
    }
    else if(speed < 0)
    {
        direction = 1;
        speed *= (-1);
    }
    
    if(speed > MOTOR_TIMER_PERIOD)
    {
        speed = MOTOR_TIMER_PERIOD;
    }
    
    // The timers want the speeds backwards so that the high and low side work correctly
    speed = MOTOR_TIMER_PERIOD - speed;
    
    if(motor == 0)
    {
        motor0_set_speed(direction, speed);
    }
    else
    {
        motor1_set_speed(direction, speed);
    }
    
}

volatile int16_t m0speed = 0;
volatile int16_t m1speed = 0;
volatile uint8_t spdChange = 0;

volatile CommIntBuff_t debugCmdBuff;
volatile CommIntBuff_t cmdBuff;

ISR(USARTD0_RXC_vect)
{
    if(interpreterReceiveByte(&debugCmdBuff, usart_getchar(&USARTD0)))
    {
        if((debugCmdBuff.cmdParams[0] == 3) && (debugCmdBuff.cmdParamsLength == 5))
        {
            m0speed = (debugCmdBuff.cmdParams[1] << 8) | debugCmdBuff.cmdParams[2];
            m1speed = (debugCmdBuff.cmdParams[3] << 8) | debugCmdBuff.cmdParams[4];
            spdChange = 1;
            
            motor_set_speed(m0speed, 0);
            motor_set_speed(m1speed, 1);
        }
        else if((debugCmdBuff.cmdParams[0] == 4) && (debugCmdBuff.cmdParamsLength == 2))
        {
            if(debugCmdBuff.cmdParams[1] == 0)
            {
                ioport_set_pin_level(PIN_PWR_CPU, false);
            }
            else
            {
                ioport_set_pin_level(PIN_PWR_CPU, true);
            }
        }
    }
}

ISR(USARTE0_RXC_vect)
{
    if(interpreterReceiveByte(&cmdBuff, usart_getchar(&USARTD0)))
    {
        if((cmdBuff.cmdParams[0] == 10) && (cmdBuff.cmdParams[1] == 1) && (cmdBuff.cmdParamsLength == 10))
        {
            m0speed = (cmdBuff.cmdParams[2] << 8) | cmdBuff.cmdParams[3];
            m1speed = (cmdBuff.cmdParams[4] << 8) | cmdBuff.cmdParams[5];
            spdChange = 1;
            
            motor_set_speed(m0speed, 0);
            motor_set_speed(m1speed, 1);
            
            // TODO: Set servo positions.
        }
    }        
}    

uint8_t rawBuffer[20];
uint8_t txBuffer[40];

int main (void)
{
    uint16_t txLength;
    uint8_t i;
    
	// Insert system clock initialization code here (sysclk_init()).
    init_avr_asf_libraries();

    configure_motor_clock();
    
    enable_motor_pins();
    
    start_motor_clock();
    
    usart_serial_options_t uOpts;
    
    uOpts.baudrate = 9600;
    uOpts.charlength = USART_CHSIZE_8BIT_gc;
    uOpts.paritytype = USART_PMODE_DISABLED_gc;
    uOpts.stopbits = false;
    
    ioport_set_pin_mode(PIN_DEBUG_TX, IOPORT_MODE_TOTEM);
    ioport_set_pin_dir(PIN_DEBUG_TX, IOPORT_DIR_OUTPUT);
    
    ioport_set_pin_mode(PIN_PWR_CPU, IOPORT_MODE_TOTEM);
    ioport_set_pin_level(PIN_PWR_CPU, true);
    ioport_set_pin_dir(PIN_PWR_CPU, IOPORT_DIR_OUTPUT);
    
    usart_serial_init(&USARTD0, &uOpts);
    usart_set_rx_interrupt_level(&USARTD0, USART_INT_LVL_LO);
    
    ioport_set_pin_mode(PIN_CMD_TX, IOPORT_MODE_TOTEM);
    ioport_set_pin_dir(PIN_CMD_TX, IOPORT_DIR_OUTPUT);
    
    uOpts.baudrate = 57600UL;
    usart_serial_init(&USARTE0, &uOpts);
    usart_set_rx_interrupt_level(&USARTE0, USART_INT_LVL_LO);
    
    // TODO: Set pull-up on RX-pins.
    
    cpu_irq_enable(); // A new name for sei()

	while(1)
    {
        usart_serial_putchar(&USARTD0, 'A');
        if(spdChange)
        {
            spdChange = 0;
            rawBuffer[0] = m0speed >> 8;
            rawBuffer[1] = m0speed & 0xFF;
            rawBuffer[2] = m1speed >> 8;
            rawBuffer[3] = m1speed & 0xFF;
            txLength = interpreterFormMessage(txBuffer, rawBuffer, 4);
            
            for(i = 0; i < txLength; i++)
            {
                usart_serial_putchar(&USARTD0, txBuffer[i]);
            }
        }
        
        delay_s(1);
    }
}