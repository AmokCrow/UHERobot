/*
 * config.c
 *
 * Created: 16.1.2016 20:24:42
 *  Author: crow
 */ 


#include "config.h"

#include "pins.h"

#include "gpio.h"
#include "usart.h"

#include <stdint.h>

static const gpio_map_t function_map =
{
    { PIN_UART_TX, AVR32_USART2_TXD_0_2_FUNCTION },
    { PIN_UART_RX, AVR32_USART2_RXD_0_1_FUNCTION },
    { PIN_SDA, AVR32_TWIMS1_TWD_0_FUNCTION },
    { PIN_SCL, AVR32_TWIMS1_TWCK_0_0_FUNCTION },
    { PIN_SDA_5V, AVR32_TWIMS0_TWD_0_1_FUNCTION },
    { PIN_SCL_5V, AVR32_TWIMS0_TWCK_0_3_FUNCTION },
    { PIN_MOT_1A, AVR32_PWMA_19_FUNCTION },
    { PIN_MOT_1B, AVR32_PWMA_18_FUNCTION },
    { PIN_MOT_2A, AVR32_PWMA_10_FUNCTION },
    { PIN_MOT_2B, AVR32_PWMA_12_FUNCTION },
    // TODO: Figure out how to count the fan speed. And connect to peripheral
    //{ PIN_INPUT_FAN_SPD,  },
    { PIN_ADC_TEMP, AVR32_ADCIFB_AD_8_FUNCTION },
    { PIN_ADC_MAIN_BATT_VOLT, AVR32_ADCIFB_AD_7_FUNCTION },
    { PIN_ADC_MAIN_BATT_CURR, AVR32_ADCIFB_AD_6_FUNCTION },
    { PIN_ADC_MOT_BATT_VOLT, AVR32_ADCIFB_AD_2_FUNCTION },
    { PIN_ADC_MOT_CURR, AVR32_ADCIFB_AD_1_FUNCTION }
};

static const uint16_t function_map_length = sizeof(function_map) / sizeof(function_map[0]);

static const gpio_map_t gpio_map =
{
    { PIN_5V_OUT, 0 },
    { PIN_12V_OUT, 0 },
    { PIN_5V_PD_OUT, 0 },
    { PIN_12V_PD_OUT, 0 },
    { PIN_STATUS_LED, 0 },
    { PIN_ENA_MOT_PWR, 0 },
    { PIN_ENA_FAN_PWR, 0 },
    { PIN_AUX_IO1, 0 },
    { PIN_AUX_OUT3, 0 },
    { PIN_AUX_IO4, 0 }
};


static const uint16_t gpio_map_length = sizeof(gpio_map) / sizeof(gpio_map[0]);

void InitPins(void)
{
    gpio_local_init();
    
    // Disable 32kHz output on the pin post-haste.
    gpio_enable_gpio_pin(AVR32_PIN_PA20);
    
    // Sets the GPO pins in GPIO mode.
    gpio_enable_gpio(gpio_map, gpio_map_length);
    
    // Allocates the other pins according to functionality.
    gpio_enable_module(function_map, function_map_length);
    
    // TWI needs open-collector -mode enabled separately.
    gpio_configure_pin(PIN_SDA, GPIO_OPEN_DRAIN);
    gpio_configure_pin(PIN_SCL, GPIO_OPEN_DRAIN);
    
    gpio_enable_pin_pull_up(PIN_UART_RX);
    
    // Initialize the gpio pin directions etc...
    gpio_local_enable_pin_output_driver(PIN_5V_OUT);
    gpio_local_enable_pin_output_driver(PIN_12V_OUT);
    gpio_local_enable_pin_output_driver(PIN_5V_PD_OUT);
    gpio_local_enable_pin_output_driver(PIN_12V_PD_OUT);
    gpio_local_enable_pin_output_driver(PIN_STATUS_LED);
    gpio_local_enable_pin_output_driver(PIN_ENA_MOT_PWR);
    gpio_local_enable_pin_output_driver(PIN_ENA_FAN_PWR);
    gpio_local_enable_pin_output_driver(PIN_AUX_IO1);
    gpio_local_enable_pin_output_driver(PIN_AUX_OUT3);
    gpio_local_enable_pin_output_driver(PIN_AUX_IO4);
}

static const usart_options_t uart_opts =
{
    57600UL,
    8,
    USART_NO_PARITY,
    USART_1_STOPBIT,
    USART_NORMAL_CHMODE
};

void InitUart(void)
{
    usart_reset(&AVR32_USART2);
    usart_init_rs232(&AVR32_USART2, &uart_opts, 8000000UL);
}