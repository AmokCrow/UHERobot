/*
 * pins.h
 *
 * Created: 16.1.2016 20:25:29
 *  Author: crow
 */ 


#ifndef PINS_H_
#define PINS_H_

#include <avr32/uc3l032.h>

#define PIN_5V_OUT      AVR32_PIN_PB02
#define PIN_12V_OUT     AVR32_PIN_PB03
#define PIN_5V_PD_OUT   AVR32_PIN_PA22
#define PIN_12V_PD_OUT  AVR32_PIN_PA06
#define PIN_STATUS_LED  AVR32_PIN_PA20

#if 1
#define PIN_ENA_MOT_PWR AVR32_PIN_PA11
#else
#define PIN_ENA_MOT_PWR AVR32_PIN_PA14
#endif

#define PIN_ENA_FAN_PWR AVR32_PIN_PA04
#define PIN_AUX_IO1     AVR32_PIN_PA13
#define PIN_AUX_OUT3     AVR32_PIN_PA00
#define PIN_AUX_IO4     AVR32_PIN_PB11

#define PIN_MOT_1A      AVR32_PIN_PA19
#define PIN_MOT_1B      AVR32_PIN_PA18

#define PIN_MOT_2A      AVR32_PIN_PA10
#define PIN_MOT_2B      AVR32_PIN_PA12

#define PIN_UART_TX     AVR32_PIN_PA02
#define PIN_UART_RX     AVR32_PIN_PA07

// Pins are already configured as 5V pull-down at startup.
#define PIN_SDA_5V      AVR32_PIN_PA21
#define PIN_SCL_5V      AVR32_PIN_PB05

#define PIN_SDA         AVR32_PIN_PA17
#define PIN_SCL         AVR32_PIN_PA05

#define PIN_ADC_TEMP            AVR32_PIN_PB08
#define PIN_ADC_MAIN_BATT_VOLT  AVR32_PIN_PB07
#define PIN_ADC_MAIN_BATT_CURR  AVR32_PIN_PB06
#define PIN_ADC_MOT_BATT_VOLT   AVR32_PIN_PA16
#define PIN_ADC_MOT_CURR        AVR32_PIN_PA15

#define PIN_INPUT_FAN_SPD       AVR32_PIN_PB09


#endif /* PINS_H_ */