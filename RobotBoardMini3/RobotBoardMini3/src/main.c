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

#include "pins.h"

#include "config.h"
#include "Motors.h"
#include "Analogs.h"
#include "CommandInterpreter.h"

#include <stdint.h>

volatile int16_t m0speed = 0;
volatile int16_t m1speed = 0;
volatile uint8_t spdChange = 0;
volatile CommIntBuff_t cmdBuff;
volatile uint8_t commandsReceived = 0;

__attribute__((__interrupt__)) void uart_rx_handler(void)
{
    usart_reset_status(&AVR32_USART2); 
    
    while (usart_test_hit(&AVR32_USART2))
    {
        if(interpreterReceiveByte(&cmdBuff, (AVR32_USART2.rhr) & 0xFF))
        {
            if((cmdBuff.cmdParams[0] == 23) && (cmdBuff.cmdParams[1] == 0) && (cmdBuff.cmdParamsLength == 23))
            {
                m0speed = (cmdBuff.cmdParams[2] << 8) | cmdBuff.cmdParams[3];
                m1speed = (cmdBuff.cmdParams[4] << 8) | cmdBuff.cmdParams[5];
                spdChange = 1;
            
                motor_set_speed(m0speed, 0);
                motor_set_speed(m1speed, 1);
            
                //servos[0].pos = cmdBuff.cmdParams[7];
                //servos[1].pos = cmdBuff.cmdParams[9];
            }
        
            commandsReceived = 1;
        }
    }
}

uint8_t adcResultsTxRawBuff[14];
uint8_t txBuff[40];

int main (void)
{
    uint8_t ledPos = 0;
    uint8_t* pAdcTx;
    uint16_t i;
    uint16_t tmp;
    
	sysclk_init();
	board_init();
    
    //gpio_local_init();
    //gpio_enable_gpio_pin(AVR32_PIN_PB02);
    //gpio_local_enable_pin_output_driver(AVR32_PIN_PB02);

    Disable_global_interrupt();
    INTC_init_interrupts();
    
    InitPins();
    InitUart();
    motor_init();
    
    Analogs_Init();
    
    INTC_register_interrupt(uart_rx_handler, AVR32_USART2_IRQ, AVR32_INTC_INTLEVEL_INT1);
    AVR32_USART2.ier = AVR32_USART_IER_RXRDY_MASK;
    
    Enable_global_interrupt();
    
    // Enable power to the CPU and motors.
    gpio_local_set_gpio_pin(PIN_5V_OUT);
    gpio_local_set_gpio_pin(PIN_ENA_MOT_PWR);
    
    adcResultsTxRawBuff[0] = 0x01;
    adcResultsTxRawBuff[1] = 12;
    adcResultsTxRawBuff[2] = 0; // The internal temperature sensor is not now in use.
    adcResultsTxRawBuff[3] = 0;
    
    while (1)
    {
        if (Analogs_HasResults())
        {
            pAdcTx = &(adcResultsTxRawBuff[4]);
            
            for (i = 0; i < ANALOGS_NUM_CHANNELS; i++)
            {
                tmp = Analogs_getResultMillivolts(i);
                *pAdcTx++ = tmp >> 8;
                *pAdcTx++ = tmp & 0xFF;
            }
            
            tmp = interpreterFormMessage(txBuff, adcResultsTxRawBuff, 14);
            
            // Null-terminate for the following string function.
            txBuff[tmp] = 0;
            
            usart_write_line(&AVR32_USART2, (const char*)txBuff);
            
            if (ledPos == 0)
            {
                ledPos = 1;
                gpio_local_set_gpio_pin(PIN_STATUS_LED);
            }
            else
            {
                ledPos = 0;
                gpio_local_clr_gpio_pin(PIN_STATUS_LED);
            }
        }
        
        
        //gpio_local_set_gpio_pin(PIN_5V_OUT);
        //delay_ms(1000);
        //gpio_local_clr_gpio_pin(PIN_5V_OUT);
        //delay_ms(1000);
        //usart_write_char(&AVR32_USART2, 'a');
    }
}
