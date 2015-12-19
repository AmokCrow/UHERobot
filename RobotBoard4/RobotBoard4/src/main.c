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

#include <string.h>

#include "CommandInterpreter.h"
#include "ServoTimer.h"
#include "MotorFunctions.h"

#define PIN_DEBUG_TX IOPORT_CREATE_PIN(PORTD, 3)
#define PIN_DEBUG_RX IOPORT_CREATE_PIN(PORTD, 2)

#define PIN_SERVO1 IOPORT_CREATE_PIN(PORTD, 1)
#define PIN_SERVO2 IOPORT_CREATE_PIN(PORTD, 0)

#define PIN_PWR_LED  IOPORT_CREATE_PIN(PORTD, 7)
#define PIN_PWR_CPU  IOPORT_CREATE_PIN(PORTD, 6)

#define PIN_CMD_TX   IOPORT_CREATE_PIN(PORTE, 3)
#define PIN_CMD_RX   IOPORT_CREATE_PIN(PORTE, 2)

#define PIN_RG_LED1 IOPORT_CREATE_PIN(PORTB, 2)
#define PIN_RG_LED2 IOPORT_CREATE_PIN(PORTB, 3)

void set_led_red(ioport_pin_t pin)
{
    ioport_set_pin_level(pin, false);
    ioport_set_pin_dir(pin, IOPORT_DIR_OUTPUT);
}

void set_led_green(ioport_pin_t pin)
{
    ioport_set_pin_level(pin, true);
    ioport_set_pin_dir(pin, IOPORT_DIR_OUTPUT);
}

void set_led_off(ioport_pin_t pin)
{
    ioport_set_pin_dir(pin, IOPORT_DIR_INPUT);
}
/*
ServoTimer_Servo_t servos[2] =
{
    {
        PIN_SERVO1,
        100
    },
    {
        PIN_SERVO2,
        100
    }
};
*/
ServoTimer_Servo_t servos[2] =
{
    {
        PIN_DEBUG_TX,
        100
    },
    {
        PIN_DEBUG_RX,
        100
    }
};

void init_avr_asf_libraries(void)
{
    board_init();
    pmic_init();
    sysclk_init();
    irq_initialize_vectors();
    cpu_irq_enable();
    ioport_init();
}

volatile int16_t m0speed = 0;
volatile int16_t m1speed = 0;
volatile uint8_t spdChange = 0;


volatile CommIntBuff_t cmdBuff;
/*
volatile CommIntBuff_t debugCmdBuff;

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
*/
uint8_t inCommBuff[80];
volatile uint8_t inCommLength = 0;

volatile uint8_t commandsReceived = 0;

ISR(USARTE0_RXC_vect)
{
    if(interpreterReceiveByte(&cmdBuff, usart_getchar(&USARTE0)))
    {
        if((cmdBuff.cmdParams[0] == 23) && (cmdBuff.cmdParams[1] == 0) && (cmdBuff.cmdParamsLength == 23))
        {
            m0speed = (cmdBuff.cmdParams[2] << 8) | cmdBuff.cmdParams[3];
            m1speed = (cmdBuff.cmdParams[4] << 8) | cmdBuff.cmdParams[5];
            spdChange = 1;
            
            motor_set_speed(m0speed, 0);
            motor_set_speed(m1speed, 1);
            
            servos[0].pos = cmdBuff.cmdParams[7];
            servos[1].pos = cmdBuff.cmdParams[9];
        }
        else if((cmdBuff.cmdParams[0] == 3) && (cmdBuff.cmdParamsLength == 5))
        {
            m0speed = (cmdBuff.cmdParams[1] << 8) | cmdBuff.cmdParams[2];
            m1speed = (cmdBuff.cmdParams[3] << 8) | cmdBuff.cmdParams[4];
            spdChange = 1;
            
            motor_set_speed(m0speed, 0);
            motor_set_speed(m1speed, 1);
        }
        
        commandsReceived = 1;
        ioport_set_pin_level(PIN_SERVO1, true);
        ioport_set_pin_level(PIN_SERVO2, true);
        
        memcpy(inCommBuff, cmdBuff.cmdParams, cmdBuff.cmdParamsLength);
        inCommLength = cmdBuff.cmdParamsLength;
    }        
}    

uint8_t rawBuffer[20];
uint8_t txBuffer[40];

void configure_uarts(void)
{
    usart_serial_options_t uOpts;
    
    //uOpts.baudrate = 9600;
    uOpts.charlength = USART_CHSIZE_8BIT_gc;
    uOpts.paritytype = USART_PMODE_DISABLED_gc;
    uOpts.stopbits = false;
    
    //ioport_set_pin_mode(PIN_DEBUG_TX, IOPORT_MODE_TOTEM);
    //ioport_set_pin_dir(PIN_DEBUG_TX, IOPORT_DIR_OUTPUT);
    
    ioport_set_pin_mode(PIN_PWR_CPU, IOPORT_MODE_TOTEM);
    ioport_set_pin_level(PIN_PWR_CPU, true);
    ioport_set_pin_dir(PIN_PWR_CPU, IOPORT_DIR_OUTPUT);
    
    //usart_serial_init(&USARTD0, &uOpts);
    //usart_set_rx_interrupt_level(&USARTD0, USART_INT_LVL_LO);
    
    ioport_set_pin_mode(PIN_CMD_TX, IOPORT_MODE_TOTEM);
    ioport_set_pin_dir(PIN_CMD_TX, IOPORT_DIR_OUTPUT);
    
    uOpts.baudrate = 57600UL;
    usart_serial_init(&USARTE0, &uOpts);
    // Warning: Interrupt level must match that of the 1s periodic timer.
    usart_set_rx_interrupt_level(&USARTE0, USART_INT_LVL_LO);
    
}

typedef struct  
{
    uint8_t mux;
    uint8_t ctrl;
    uint8_t isSigned;
}
AdcLocation_t;

void adc_startOnSetting(const AdcLocation_t* loc)
{
    irqflags_t flags;

    flags = cpu_irq_save();

    if(loc->isSigned)
    {
        ADCA.CTRLB |= 0x10;
    }
    else
    {
        ADCA.CTRLB &= ~0x10;
    }

    ADCA.CH0.CTRL = loc->ctrl;
    ADCA.CH0.MUXCTRL = loc->mux;

    cpu_irq_restore(flags);
    
    adc_start_conversion(&ADCA, ADC_CH0);
}

uint16_t adc_secondConversion()
{
    adc_wait_for_interrupt_flag(&ADCA, ADC_CH0);
    adc_get_unsigned_result(&ADCA, ADC_CH0);
    
    adc_start_conversion(&ADCA, ADC_CH0);
    
    adc_wait_for_interrupt_flag(&ADCA, ADC_CH0);
    return adc_get_unsigned_result(&ADCA, ADC_CH0);
}

void adc_init(void)
{
    struct adc_config adc_conf;
    struct adc_channel_config adcch_conf;
    adc_read_configuration(&ADCA, &adc_conf);
    adc_set_conversion_parameters(&adc_conf, ADC_SIGN_ON, ADC_RES_12,
    ADC_REF_VCC);
    adc_set_conversion_trigger(&adc_conf, ADC_TRIG_MANUAL, 1, 0);
    adc_conf.ctrlb |= 0x80;
    adc_set_clock_rate(&adc_conf, 150000UL);
    adc_write_configuration(&ADCA, &adc_conf);
    
    adcch_read_configuration(&ADCA, ADC_CH0, &adcch_conf);
    adcch_set_input(&adcch_conf, ADCCH_POS_PIN0, ADCCH_NEG_NONE, 1);
    adcch_write_configuration(&ADCA, ADC_CH0, &adcch_conf);
}

const AdcLocation_t adcLoc_IntTempSensor = { ADC_CH_MUXINT_TEMP_gc, ADC_CH_INPUTMODE_INTERNAL_gc | ADC_CH_GAIN_1X_gc, 0 };
const AdcLocation_t adcLoc_TempSensor = { ADC_CH_MUXPOS_PIN1_gc | ADC_CH_MUXNEG_GND_MODE4_gc, ADC_CH_INPUTMODE_DIFFWGAIN_gc | ADC_CH_GAIN_DIV2_gc, 1 };
const AdcLocation_t adcLoc_CpuCurrent = { ADC_CH_MUXPOS_PIN2_gc | ADC_CH_MUXNEG_GND_MODE4_gc, ADC_CH_INPUTMODE_DIFFWGAIN_gc | ADC_CH_GAIN_DIV2_gc, 1 };
//const AdcLocation_t adcLoc_CpuCurrent = { ADC_CH_MUXPOS_PIN5_gc | ADC_CH_MUXNEG_GND_MODE4_gc, ADC_CH_INPUTMODE_DIFFWGAIN_gc | ADC_CH_GAIN_DIV2_gc, 1 };
const AdcLocation_t adcLoc_CpuVoltage = { ADC_CH_MUXPOS_PIN3_gc | ADC_CH_MUXNEG_GND_MODE3_gc, ADC_CH_INPUTMODE_DIFF_gc, 1 };
//const AdcLocation_t adcLoc_CpuVoltage = { ADC_CH_MUXPOS_PIN6_gc | ADC_CH_MUXNEG_GND_MODE3_gc, ADC_CH_INPUTMODE_DIFF_gc, 1 };
const AdcLocation_t adcLoc_MotorCurrent = { ADC_CH_MUXPOS_PIN7_gc | ADC_CH_MUXNEG_PIN4_gc, ADC_CH_INPUTMODE_DIFFWGAIN_gc | ADC_CH_GAIN_DIV2_gc, 1 }; // TODO: Adjust gain after installing component
//const AdcLocation_t adcLoc_MotorCurrent = { ADC_CH_MUXPOS_PIN10_gc | ADC_CH_MUXNEG_GND_MODE3_gc, ADC_CH_INPUTMODE_DIFF_gc, 1 };
const AdcLocation_t adcLoc_MotorVoltage = { ADC_CH_MUXPOS_PIN6_gc | ADC_CH_MUXNEG_GND_MODE3_gc, ADC_CH_INPUTMODE_DIFF_gc, 1 }; // 

void period_check_timer_callback(void)
{
    if(commandsReceived == 0)
    {
        motor_set_speed(0, 0);
        motor_set_speed(0, 1);
        
        ioport_set_pin_level(PIN_SERVO1, false);
        ioport_set_pin_level(PIN_SERVO2, false);
    }
    
    commandsReceived = 0;
}

void config_period_check_timer(TC0_t* pTimer)
{
    // Enable timer.
    tc_enable(pTimer);
    
    // Set callbacks.
    tc_set_overflow_interrupt_callback(pTimer, period_check_timer_callback);
    
    // Low interrupt level; timing is not very tight.
    tc_set_overflow_interrupt_level(pTimer, TC_INT_LVL_LO);
    
    // Disable PWM modes if any.
    tc_set_wgm(pTimer, TC_WGMODE_NORMAL_gc);
    
    // System clock is 8MHz. This should therefore give an approximately 1.5 second timeout.
    tc_write_period(pTimer, 12000);
    
    // Set clock source. This will start the timer.
    tc_write_clock_source(pTimer, TC_CLKSEL_DIV1024_gc);
}

int main (void)
{
    uint16_t txLength;
    uint8_t i;
    int16_t adcResult;
    
    uint8_t* pBuff;
    
    uint8_t cycleCounter = 0;
    
	// Insert system clock initialization code here (sysclk_init()).
    init_avr_asf_libraries();

    configure_motor_clock();
    
    enable_motor_pins();
    
    start_motor_clock();
    
    ioport_set_pin_dir(PIN_SERVO1, IOPORT_DIR_OUTPUT);
    ioport_set_pin_dir(PIN_SERVO2, IOPORT_DIR_OUTPUT);
    
    configure_uarts();
    
    // TODO: Set pull-up on RX-pins.
    
    config_period_check_timer(&TCE0);
    
    adc_init();
    
    adc_enable(&ADCA);
    
    
    ServoTimer_init(&TCD0, servos, 2);
    
    
    cpu_irq_enable(); // A new name for sei()


	while(1)
    {
        cycleCounter++;
        
        if((cycleCounter % 2) == 0)
        {
            set_led_green(PIN_RG_LED1);
        }
        else
        {
            set_led_red(PIN_RG_LED1);
        }
        
        pBuff = rawBuffer;
        *pBuff++ = 0x01; // Tag for the adc results data
        *pBuff++ = 12; // Length for the adc results data
        
        adc_startOnSetting(&adcLoc_IntTempSensor);
        adcResult = adc_secondConversion();
        *pBuff++ = adcResult >> 8;
        *pBuff++ = adcResult & 0xFF;
        
        adc_startOnSetting(&adcLoc_TempSensor);
        adcResult = adc_secondConversion();
        *pBuff++ = adcResult >> 8;
        *pBuff++ = adcResult & 0xFF;
        
        adc_startOnSetting(&adcLoc_CpuCurrent);
        adcResult = adc_secondConversion();
        *pBuff++ = adcResult >> 8;
        *pBuff++ = adcResult & 0xFF;
        
        adc_startOnSetting(&adcLoc_CpuVoltage);
        adcResult = adc_secondConversion();
        *pBuff++ = adcResult >> 8;
        *pBuff++ = adcResult & 0xFF;
        
        adc_startOnSetting(&adcLoc_MotorCurrent);
        adcResult = adc_secondConversion();
        *pBuff++ = adcResult >> 8;
        *pBuff++ = adcResult & 0xFF;
        
        adc_startOnSetting(&adcLoc_MotorVoltage);
        adcResult = adc_secondConversion();
        *pBuff++ = adcResult >> 8;
        *pBuff++ = adcResult & 0xFF;
        
        //rawBuffer[6] = 0;
        //rawBuffer[7] = 0;
        
        txLength = interpreterFormMessage(txBuffer, rawBuffer, 14);
        
        for(i = 0; i < txLength; i++)
        {
            usart_serial_putchar(&USARTE0, txBuffer[i]);
        }
        
        // USARTD0 has been disabled.
        /*
        for(i = 0; i < txLength; i++)
        {
            usart_serial_putchar(&USARTD0, txBuffer[i]);
        }
        
        if(inCommLength != 0)
        {
            txLength = interpreterFormMessage(txBuffer, inCommBuff, inCommLength);
            for(i = 0; i < txLength; i++)
            {
                usart_serial_putchar(&USARTD0, txBuffer[i]);
            }
            
            inCommLength = 0;
        }
        
        //usart_serial_putchar(&USARTD0, 'A');
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
        */
        
        delay_s(1);
    }
}
