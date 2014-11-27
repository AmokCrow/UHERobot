/**
 * \file
 *
 * \brief User board initialization template
 *
 */

#include <asf.h>
#include <board.h>
#include <conf_board.h>
#include <serial.h>
#include <adcifb.h>

void board_init(void)
{
	/* This function is meant to contain board-specific initialization code
	 * for, e.g., the I/O pins. The initialization can rely on application-
	 * specific board configuration, found in conf_board.h.
	 */
	
	
	gpio_configure_pin(RED_LED_GPIO, (GPIO_DIR_OUTPUT | GPIO_INIT_HIGH));
    gpio_configure_pin(BOARD_POWER_GPIO, (GPIO_DIR_OUTPUT | GPIO_INIT_LOW));
	
    
    /*
	gpio_map_t USART1_GPIO_MAP = 
	{
		{AVR32_USART1_TXD_0_2_PIN, AVR32_USART1_TXD_0_2_FUNCTION},
		{AVR32_USART1_RXD_0_2_PIN, AVR32_USART1_RXD_0_2_FUNCTION}
	};
	
	gpio_enable_module(USART1_GPIO_MAP, sizeof(USART1_GPIO_MAP) / sizeof(USART1_GPIO_MAP[0]));
    */
    
    // Enable debug USART
    gpio_map_t USART2_GPIO_MAP =
    {
        {AVR32_USART2_TXD_0_1_PIN, AVR32_USART2_TXD_0_1_FUNCTION},
        {AVR32_USART2_RXD_0_1_PIN, AVR32_USART2_RXD_0_1_FUNCTION},
        {AVR32_USART3_RXD_0_1_PIN, AVR32_USART3_RXD_0_1_FUNCTION},
        {AVR32_USART3_TXD_0_1_PIN, AVR32_USART3_TXD_0_1_FUNCTION}
    };
    
    gpio_enable_module(USART2_GPIO_MAP, sizeof(USART2_GPIO_MAP) / sizeof(USART2_GPIO_MAP[0]));
    
    static usart_serial_options_t usart_options = {
        .baudrate = USART_DBG_BAUDRATE,
        .charlength = 8,
        .paritytype = USART_NO_PARITY,
        .stopbits = USART_1_STOPBIT,
        .channelmode = USART_NORMAL_CHMODE
    };
    
    usart_serial_init(USART_DBG_PORT, &usart_options);
    usart_serial_init(USART_UPLINK_PORT, &usart_options);
    
    
    gpio_map_t ADC_GPIO_MAP =
    {
        {AVR32_ADCIFB_AD_0_PIN, AVR32_ADCIFB_AD_0_FUNCTION},
        {AVR32_ADCIFB_AD_1_PIN, AVR32_ADCIFB_AD_1_FUNCTION},
        {AVR32_ADCIFB_AD_2_PIN, AVR32_ADCIFB_AD_2_FUNCTION}
    };
        
    gpio_enable_module(ADC_GPIO_MAP, sizeof(ADC_GPIO_MAP) / sizeof(ADC_GPIO_MAP[0]));
    
    
    adcifb_opt_t adc_opt = 
    {
        	/* Resolution mode */
        	.resolution = AVR32_ADCIFB_ACR_RES_12BIT,

        	/* Channels Sample & Hold Time in [0,15] */
        	.shtim = 15,
            /* 1.5MHz ADC clock? -JS */
        	.ratio_clkadcifb_clkadc = (sysclk_get_pba_hz() / 1500000),

        	/*
        	* Startup time in [0,127], where
        	* Tstartup = startup * 8 * Tclk_adc (assuming Tstartup ~ 15us max)
        	*/
        	.startup = 3,

        	/* ADCIFB Sleep Mode disabled */
        	.sleep_mode_enable = false
    };
    
    
    adcifb_configure((avr32_adcifb_t*)AVR32_ADCIFB_ADDRESS, &adc_opt);
    adcifb_configure_trigger((avr32_adcifb_t*)AVR32_ADCIFB_ADDRESS, AVR32_ADCIFB_TRGMOD_NT, 0);
    adcifb_channels_enable((avr32_adcifb_t*)AVR32_ADCIFB_ADDRESS, (1 << 0) | (1 << 1) | (1 << 2));
    
    // Enable fast GPIO
    gpio_local_init();
    
    // Disable OSC32 output on PA20
    gpio_enable_gpio_pin(AVR32_PIN_PA20);
    
    // Init timer 0 for motor control.
    
    
}
