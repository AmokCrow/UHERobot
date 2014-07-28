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
#include <delay.h>
#include <serial.h>
#include <string.h>
#include <stdio.h>

volatile uint16_t inputVoltage = 0;
volatile uint16_t boardCurrent = 0;
volatile uint16_t boardTemperature = 0;


__attribute__((__interrupt__)) static void adc_data_interrupt(void)
{
    uint32_t result = adcifb_get_last_data((avr32_adcifb_t*)AVR32_ADCIFB_ADDRESS);
    uint8_t channel = (result >> 16) & 0xFF;
    uint16_t adcValue = result & 0x0FFF; // The lowest 12 bits
    
    if(channel == 0)
    {
        inputVoltage = adcValue;
        
    }
    else if(channel == 1)
    {
        boardCurrent = adcValue;
    }
    else if(channel == 2)
    {
        boardTemperature = adcValue;
    }
    
    adcifb_clear_all_interrupt_flags((avr32_adcifb_t*)AVR32_ADCIFB_ADDRESS);
}


int main (void)
{	
    uint8_t printBuffer[100];
    
	sysclk_init();
	board_init();
    
    const char* test_str = "Test String\r\n";
    
    adcifb_enable_data_ready_interrupt((avr32_adcifb_t*)AVR32_ADCIFB_ADDRESS);
    
    // Disable all interrupts.
    Disable_global_interrupt();

    // init the interrupts
    INTC_init_interrupts();

    INTC_register_interrupt(&adc_data_interrupt, AVR32_ADCIFB_IRQ, AVR32_INTC_INT1);
    
    // Enable all interrupts.
    Enable_global_interrupt();
    
	while(1)
	{
		gpio_set_pin_high(LED0_GPIO);
		
		delay_s(1);
		
		gpio_set_pin_low(LED0_GPIO);
		
        adcifb_start_conversion_sequence((avr32_adcifb_t*)AVR32_ADCIFB_ADDRESS);
        usart_serial_write_packet(USART_DBG_PORT, test_str, strlen(test_str));
		delay_s(1);
        
        sprintf(printBuffer, "Vin: 0x%X - %u\r\n", inputVoltage, inputVoltage);
        usart_serial_write_packet(USART_DBG_PORT, printBuffer, strlen(printBuffer));
        sprintf(printBuffer, "Iin: 0x%X - %u\r\n", boardCurrent, boardCurrent);
        usart_serial_write_packet(USART_DBG_PORT, printBuffer, strlen(printBuffer));
        sprintf(printBuffer, "Temp: 0x%X - %u\r\n", boardTemperature, boardTemperature);
        usart_serial_write_packet(USART_DBG_PORT, printBuffer, strlen(printBuffer));
	}

	// Insert application code here, after the board has been initialized.
}
