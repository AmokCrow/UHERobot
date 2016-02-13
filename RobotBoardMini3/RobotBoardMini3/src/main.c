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
    uint32_t tmp;
    int16_t iTmp;
    
	sysclk_init();
	board_init();

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
            
            // Motor current.
            // The resistor is 12mOhm. OpAmp multiplier is 50x.
            // => 1 Amp = 12mV * 50 = 600mV
            // => 1V = 5/3 A
            tmp = Analogs_getResultMillivolts(0);
            tmp *= 3;
            tmp /= 5;
            *pAdcTx++ = tmp >> 8;
            *pAdcTx++ = tmp & 0xFF;
            
            // Motor voltage.
            // Resistor division is 3k3 / (10k + 3k3).
            tmp = Analogs_getResultMillivolts(1);
            tmp *= 133;
            tmp /= 33;
            *pAdcTx++ = tmp >> 8;
            *pAdcTx++ = tmp & 0xFF;
            
            // CPU battery current.
            // The voltage should be approximately 1.5V / A.
            tmp = Analogs_getResultMillivolts(2);
            tmp *= 2;
            tmp /= 3;
            *pAdcTx++ = tmp >> 8;
            *pAdcTx++ = tmp & 0xFF;
            
            // Main battery voltage.
            // The division is 1k / (10k + 1k).
            tmp = Analogs_getResultMillivolts(3);
            tmp *= 11;
            *pAdcTx++ = tmp >> 8;
            *pAdcTx++ = tmp & 0xFF;
            
            // Temperature sensor MCP9700.
            // Resistor division is 2 (20k / (20k + 20k)).
            tmp = Analogs_getResultMillivolts(4);
            tmp *= 2;
            // Using a signed integer, as temperatures can be negative.
            // V(0C) = 500mV.
            iTmp = tmp - 500;
            // Coefficient is 10mV / C.
            iTmp /= 10;
            *pAdcTx++ = (iTmp >> 8) & 0xFF;
            *pAdcTx++ = iTmp & 0xFF;
            
            /*
            for (i = 0; i < ANALOGS_NUM_CHANNELS; i++)
            {
                tmp = Analogs_getResultMillivolts(i);
                *pAdcTx++ = tmp >> 8;
                *pAdcTx++ = tmp & 0xFF;
            }
            */
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
