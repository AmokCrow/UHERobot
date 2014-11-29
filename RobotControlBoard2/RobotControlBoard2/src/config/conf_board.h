/**
 * \file
 *
 * \brief User board configuration template
 *
 */

#ifndef CONF_BOARD_H
#define CONF_BOARD_H

#define RED_LED_GPIO	    AVR32_PIN_PA12
#define ORANGE_LED_GPIO     AVR32_PIN_PA10
#define BOARD_POWER_GPIO    AVR32_PIN_PA17
#define TR_OUTPUT_GPIO      AVR32_PIN_PA19

#define SERVO_CLK_GPIO      AVR32_PIN_PB09
#define SERVO_RST_GPIO      AVR32_PIN_PA04

#define AUX_MOT_GPIO1       AVR32_PIN_PB04
#define AUX_MOT_GPIO2       AVR32_PIN_PA20
#define AUX_MOT_GPIO3       AVR32_PIN_PA13
#define AUX_MOT_GPIO4       AVR32_PIN_PA11

#define AUX_MOT_PWMA_PIN1   AVR32_PWMA_27_PIN
#define AUX_MOT_PWMA_PIN2   AVR32_PWMA_20_PIN
#define AUX_MOT_PWMA_PIN3   AVR32_PWMA_13_PIN
#define AUX_MOT_PWMA_PIN4   AVR32_PWMA_11_PIN
#define AUX_MOT_PWMA_FUNC1  AVR32_PWMA_27_FUNCTION
#define AUX_MOT_PWMA_FUNC2  AVR32_PWMA_20_FUNCTION
#define AUX_MOT_PWMA_FUNC3  AVR32_PWMA_13_FUNCTION
#define AUX_MOT_PWMA_FUNC4  AVR32_PWMA_11_FUNCTION

#define ADC_CHANNEL_VIN     0
#define ADC_CHANNEL_BCURR   1
#define ADC_CHANNEL_BTEMP   2

#define UPLINK_USART_TX_PIN         AVR32_USART3_TXD_0_1_PIN
#define UPLINK_USART_TX_FUNCTION    AVR32_USART3_TXD_0_1_FUNCTION
#define UPLINK_USART_RX_PIN         AVR32_USART3_RXD_0_1_PIN
#define UPLINK_USART_RX_FUNCTION    AVR32_USART3_RXD_0_1_FUNCTION

#define MEM_TWIMS_TWD_PIN           AVR32_TWIMS0_TWD_0_1_PIN
#define MEM_TWIMS_TWD_FUNCTION      AVR32_TWIMS0_TWD_0_1_FUNCTION
#define MEM_TWIMS_TWCK_PIN          AVR32_TWIMS0_TWCK_0_3_PIN
#define MEM_TWIMS_TWCK_FUNCTION     AVR32_TWIMS0_TWCK_0_3_FUNCTION

#define DEBUG_USART_TX_PIN          AVR32_USART2_TXD_0_1_PIN
#define DEBUG_USART_TX_FUNCTION     AVR32_USART2_TXD_0_1_FUNCTION
#define DEBUG_USART_RX_PIN          AVR32_USART2_RXD_0_1_PIN
#define DEBUG_USART_RX_FUNCTION     AVR32_USART2_RXD_0_1_FUNCTION


#define MOTOR_CTRL_IN1_GPIO         AVR32_PIN_PB00
#define MOTOR_CTRL_IN2_GPIO         AVR32_PIN_PA05
#define MOTOR_CTRL_IN3_GPIO         AVR32_PIN_PB03
#define MOTOR_CTRL_IN4_GPIO         AVR32_PIN_PA22

#define MOTOR_CTRL_ENA_GPIO         AVR32_PIN_PB02

#define MOTOR1_TIMER                ((volatile avr32_tc_t*)AVR32_TC0_ADDRESS)
#define MOTOR1_TC_CHANNEL           1
#define MOTOR_I1_TMR_T0A1_PIN       AVR32_TC0_A1_0_3_PIN
#define MOTOR_I1_TMR_T0A1_FUNCTION  AVR32_TC0_A1_0_3_FUNCTION
#define MOTOR_I2_TMR_T0A1_PIN       AVR32_TC0_A1_0_2_PIN
#define MOTOR_I2_TMR_T0A1_FUNCTION  AVR32_TC0_A1_0_2_FUNCTION
#define MOTOR2_TIMER                ((volatile avr32_tc_t*)AVR32_TC0_ADDRESS)
#define MOTOR2_TC_CHANNEL           2
#define MOTOR_I3_TMR_T0B2_PIN       AVR32_TC0_B2_0_2_PIN
#define MOTOR_I3_TMR_T0B2_FUNCTION  AVR32_TC0_B2_0_2_FUNCTION
#define MOTOR_I4_TMR_T0B2_PIN       AVR32_TC0_B2_0_1_PIN
#define MOTOR_I4_TMR_T0B2_FUNCTION  AVR32_TC0_B2_0_1_FUNCTION



#endif // CONF_BOARD_H
