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
#include <math.h>
#include <string.h>
#include <usart.h>

#include "Servo4017.h"
#include "usart_serial.h"
#include "adcifb.h"
#include "Base16MsgParser.h"
#include "MotorDriver.h"
#include "PrintFunctions.h"

volatile uint16_t inputVoltage = 0;
volatile uint16_t boardCurrent = 0;
volatile uint16_t boardTemperature = 0;

volatile uint8_t gotMessage = 0;
volatile uint16_t msgTimeoutCounter = 0;

uint8_t ctrlMsgRxBuffer[100];
uint8_t ctrlMsgTxBuffer[100];

Servo4017 servos;
Base16MsgParser msgParser;
Base16MsgParser txParser;
MotorDriver motor1;
MotorDriver motor2;
PrintFunctions dbgSerial;

#define CTRL_FROM_DBG 1

enum CtrlMsg
{
    LedsOffset = 0,
    ServosOffset = 1,
    ServosNum = 8,
    MotorsOffset = 1 + (8 * 2),
    MotorsNum = 2
};

__attribute__((__interrupt__)) static void adc_data_interrupt(void)
{
    
    uint32_t result = adcifb_get_last_data(&AVR32_ADCIFB);
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

void decipherCtrlMsg(uint8_t length)
{
    uint8_t msgType = ctrlMsgRxBuffer[1];
    uint8_t msgLength = ctrlMsgRxBuffer[0];
    
    if((msgType == 0) && (msgLength == 23))
    {
        uint8_t tmp = ctrlMsgRxBuffer[2];
        
        // The first data byte is the led states
        if(tmp & (1 << 0))
        {
            gpio_local_set_gpio_pin(RED_LED_GPIO);
        }
        else
        {
            gpio_local_clr_gpio_pin(RED_LED_GPIO);
        }
        
        motor1.setSpeed((ctrlMsgRxBuffer[3] << 8) | ctrlMsgRxBuffer[4]);
        motor2.setSpeed((ctrlMsgRxBuffer[5] << 8) | ctrlMsgRxBuffer[6]);
        
        for(uint16_t i = 0; i < 8; i++)
        {
            servos.setPos(i, ((uint16_t)ctrlMsgRxBuffer[7 + i] << 8) | ctrlMsgRxBuffer[8 + i]);
        }
    }
    else if(msgType == 1) // Debug led test message, with only the led byte
    {
        uint8_t tmp = ctrlMsgRxBuffer[2];
        if(tmp & (1 << 0))
        {
            gpio_local_set_gpio_pin(RED_LED_GPIO);
        }
        else
        {
            gpio_local_clr_gpio_pin(RED_LED_GPIO);
        }
    }
    else if(msgType == 2)
    {
        servos.setPos(ctrlMsgRxBuffer[2], ((uint16_t)ctrlMsgRxBuffer[3] << 8) | ctrlMsgRxBuffer[4]);
    }
    else if(msgType == 3)
    {
        motor1.setSpeed((ctrlMsgRxBuffer[2] << 8) | ctrlMsgRxBuffer[3]);
        motor2.setSpeed((ctrlMsgRxBuffer[4] << 8) | ctrlMsgRxBuffer[5]);
    }
    
    gotMessage = 1;
    // If it was not a good message, the next incoming byte will take care of it in any case
}

__attribute__((__interrupt__)) static void ctrl_uart_interrupt(void)
{
    uint8_t tmpChar;
    
    if(usart_test_hit(USART_DBG_PORT))
    //while(usart_test_hit(USART_UPLINK_PORT))
    {
        tmpChar = 0xFF & usart_getchar(USART_DBG_PORT);
        usart_putchar(USART_DBG_PORT, tmpChar);
        //tmpChar = 0xFF & usart_getchar(USART_UPLINK_PORT);
        tmpChar = msgParser.decodeChar(tmpChar);
        
        if(tmpChar != 0)
        {
            decipherCtrlMsg(tmpChar);
        }
    }
    else
    {
        // gotMessage = 1;
    }
}

float voltFromAdc(uint16_t ana)
{
    // 2**12 = 4096 = ADC dynamic
    // Analog range is 1.8V
    return (1.8f / 4096.0f) * ana;
}

__attribute__((__interrupt__)) static void servo_timer_interrupt(void)
{
    servos.interruptCallback();
}

int main (void)
{	
    
    uint8_t printBuffer[100];
    volatile float ftmp;
    volatile float fvoltage;
    
	sysclk_init();
	board_init();
    
    const char* test_str = "Test String\r\n";
    
    adcifb_enable_data_ready_interrupt((avr32_adcifb_t*)AVR32_ADCIFB_ADDRESS);
    
    // Disable all interrupts.
    Disable_global_interrupt();

    // init the interrupts
    INTC_init_interrupts();

    INTC_register_interrupt(&adc_data_interrupt, AVR32_ADCIFB_IRQ, AVR32_INTC_INT1);
    
    INTC_register_interrupt(&servo_timer_interrupt, AVR32_TC1_IRQ0, AVR32_INTC_INT1);
    
    // Enable all interrupts.
    Enable_global_interrupt();
    
    gpio_set_pin_high(BOARD_POWER_GPIO);
    
    // gpio_local_init() was called inside board_init()
    servos.init(SERVO_RST_GPIO, SERVO_CLK_GPIO, &AVR32_TC1, 0, 1);
    
    const uint8_t readyStr[] = "Ready...\r\n";
    const uint8_t actStr[] = "\r\nCommand accepted\r\n\r\n";
    
    //usart_serial_write_packet(USART_DBG_PORT, readyStr, strlen((const char*)readyStr));
    dbgSerial.init(USART_DBG_PORT);
    //dbgSerial.printString("Ready...\r\n");
    
    msgParser.init(ctrlMsgRxBuffer, 100);
    txParser.init(ctrlMsgTxBuffer, 100);
    
    INTC_register_interrupt(&ctrl_uart_interrupt, AVR32_USART2_IRQ, AVR32_INTC_INT1);
    AVR32_USART2.ier |= (1 << 0);
    
    //INTC_register_interrupt(&ctrl_uart_interrupt, AVR32_USART3_IRQ, AVR32_INTC_INT1);
    //AVR32_USART3.ier |= (1 << 0);
    
    gpio_enable_gpio_pin(MOTOR_CTRL_ENA_GPIO);
    gpio_configure_pin(MOTOR_CTRL_ENA_GPIO, (GPIO_DIR_OUTPUT | GPIO_INIT_HIGH));
    
    //gpio_configure_pin(MOTOR_I1_TMR_T0A1_PIN, (GPIO_DIR_OUTPUT | GPIO_INIT_HIGH));
    //gpio_configure_pin(MOTOR_I2_TMR_T0A1_PIN, (GPIO_DIR_OUTPUT | GPIO_INIT_LOW));
    motor1.init(MOTOR_I1_TMR_T0A1_PIN, MOTOR_I1_TMR_T0A1_FUNCTION, MOTOR_I2_TMR_T0A1_PIN, MOTOR_I2_TMR_T0A1_FUNCTION, MOTOR1_TIMER, MOTOR1_TC_CHANNEL, 4000, 0);
    motor2.init(MOTOR_I3_TMR_T0B2_PIN, MOTOR_I3_TMR_T0B2_FUNCTION, MOTOR_I4_TMR_T0B2_PIN, MOTOR_I4_TMR_T0B2_FUNCTION, MOTOR2_TIMER, MOTOR2_TC_CHANNEL, 4000, 0);
    
	while(1)
	{
        /*
        for(uint16_t pos = 0; pos < 60000; pos += 5000)
        {
            delay_s(1);
            
            for(uint16_t i = 0; i < 8; i++)
            {
                servos.setPos(i, pos);
            }
            
            delay_s(1);
        }
        */
        
        if(gotMessage)
        {
            msgTimeoutCounter = 0;
            gotMessage = 0;
            // usart_serial_write_packet(USART_DBG_PORT, actStr, strlen((const char*)actStr));
        }
        else
        {
            msgTimeoutCounter++;
            
            if(msgTimeoutCounter > 4)
            {
                Disable_global_interrupt();
                motor1.setSpeed(0);
                motor2.setSpeed(0);
                Enable_global_interrupt();
            }
        }
		
        adcifb_start_conversion_sequence((avr32_adcifb_t*)AVR32_ADCIFB_ADDRESS);
        
        
        //usart_serial_write_packet((volatile avr32_usart_t *)USART_DBG_PORT, (const uint8_t*)test_str, strlen(test_str));
		
        delay_s(1);
        
        
        ftmp = voltFromAdc(inputVoltage);
        // Vadc = Vin / (10k + 1k) * 1k
        // -> Vin = Vadc * 11;
        ftmp *= 11;
        
        int16_t stmp;
        uint8_t ctmp;
        
#if CTRL_FROM_DBG
        txParser.reset();
        txParser.encodeChar(8);
        txParser.encodeChar(1);
        
        stmp = ftmp * 100.0f;
        ctmp = (stmp >> 8) & 0xFF;
        txParser.encodeChar(ctmp);
        ctmp = stmp & 0xFF;
        txParser.encodeChar(ctmp);
#else
        dbgSerial.printString("Vin: ");
        dbgSerial.floatToStr(printBuffer, ftmp);
        dbgSerial.printUString(printBuffer);
        dbgSerial.printString("V\r\n");
#endif
        // Vcurr = I * R
        // R = 0.04 Ohm
        // Vcurrmeas = 100 * Vcurr
        // Vcurradc = Vcurrmeas / (10k + 1k) * 1k
        // 
        // I = Vcurr / R
        // Vcurr = Vcurrmeas / 100
        // Vcurrmeas = Vcurradc * 11
        //
        // -> I = ((Vcurradc * 11.0) / 100.0) / 0.04 = Vcurradc * 11 / 100 / 0.04 = Vcurradc * 2.75
        ftmp = voltFromAdc(boardCurrent);
        ftmp *= 2750.0f;
        
#if CTRL_FROM_DBG
        stmp = ftmp;
        ctmp = (stmp >> 8) & 0xFF;
        txParser.encodeChar(ctmp);
        ctmp = stmp & 0xFF;
        txParser.encodeChar(ctmp);
#else       
        dbgSerial.printString("Iin: ");
        dbgSerial.floatToStr(printBuffer, ftmp);
        dbgSerial.printUString(printBuffer);
        dbgSerial.printString("mA\r\n");
#endif
        //sprintf((char*)printBuffer, "Iin: 0x%X - %u\r\n", boardCurrent, boardCurrent);
        //usart_serial_write_packet(USART_DBG_PORT, printBuffer, strlen((const char*)printBuffer));
        
        // Vin = Vadc / 2 -> Vout = Vadc * 2
        fvoltage = voltFromAdc(boardTemperature) * 2.0f;
        // Sensor is MCP9700AT
        // Tc = 10.0mV/degC (typical)
        // Vout at 0 degC (V0) = 500mV
        // Vout = temp * Tc + V0
        // temp = (Vout - V0) / Tc
        ftmp = (fvoltage - 0.5f) / 0.01f;

#if CTRL_FROM_DBG
        stmp = ftmp * 100.0f; // As a special case, temperature is in 100th part increments
        ctmp = (stmp >> 8) & 0xFF;
        txParser.encodeChar(ctmp);
        ctmp = stmp & 0xFF;
        txParser.encodeChar(ctmp);
#else     
        dbgSerial.printString("Temp: ");
        dbgSerial.floatToStr(printBuffer, ftmp);
        dbgSerial.printUString(printBuffer);
        dbgSerial.printString("degC\r\n\r\n");
#endif
        //sprintf(printBuffer, "Temp: 0x%X - %u - %.2fdegC\r\n\r\n", boardTemperature, boardTemperature, ftmp);
        //usart_serial_write_packet(USART_DBG_PORT, printBuffer, strlen(printBuffer));
        
#if CTRL_FROM_DBG
        // The parser returns the length at finalization
        ctmp = txParser.finalizeMsg();
        usart_serial_write_packet(USART_DBG_PORT, ctrlMsgTxBuffer, ctmp);
#endif
        
	}

	// Insert application code here, after the board has been initialized.
}
