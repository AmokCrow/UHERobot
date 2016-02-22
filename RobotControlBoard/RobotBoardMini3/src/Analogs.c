/*
 * Analogs.c
 *
 * Created: 23.1.2016 12:55:19
 *  Author: crow
 */ 

#include "Analogs.h"

#include <asf.h>

#define AD_CHN_MOT_CURR         1
#define AD_CHN_MOT_BATT_VOLT    2
#define AD_CHN_MAIN_BATT_CURR   6
#define AD_CHN_MAIN_BATT_VOLT   7
#define AD_CHN_PCB_TEMP         8

#define ADC_NUM_CHANS_IN_USE    5
#define ADC_NUM_REGSETS         3

static volatile uint32_t adc_accumulators[ADC_NUM_CHANS_IN_USE * ADC_NUM_REGSETS];
static volatile uint16_t adc_read_regSet = 3;

__attribute__((__interrupt__)) void adc_results_handler(void)
{
    uint32_t result = adcifb_get_last_data(&AVR32_ADCIFB);
    uint8_t channel = (result >> 16) & 0xFF;
    uint16_t adcValue = result & 0x03FF; // The lowest 10 bits
    static uint16_t num_samples = 0;
    static uint32_t* pRegSet = adc_accumulators;
    static uint16_t regSet = 0;
    
    if(channel == AD_CHN_MOT_CURR)
    {
        pRegSet[0] += adcValue;
    }
    else if(channel == AD_CHN_MOT_BATT_VOLT)
    {
        pRegSet[1] += adcValue;
    }
    else if(channel == AD_CHN_MAIN_BATT_CURR)
    {
        pRegSet[2] += adcValue;
    }
    else if(channel == AD_CHN_MAIN_BATT_VOLT)
    {
        pRegSet[3] += adcValue;
    }
    else if (channel == AD_CHN_PCB_TEMP)
    {
        pRegSet[4] += adcValue;
        
        // Switch registers every ADC_NUM_OVERSAMPLE rounds.
        num_samples++;
        if (num_samples >= ADC_JS_NUM_OVERSAMPLE)
        {
            num_samples = 0;
            
            adc_read_regSet = regSet;
            regSet++;
            if (regSet >= ADC_NUM_REGSETS)
            {
                regSet = 0;
            }
            
            pRegSet = &(adc_accumulators[regSet * ADC_NUM_CHANS_IN_USE]);
            
            pRegSet[0] = 0;
            pRegSet[1] = 0;
            pRegSet[2] = 0;
            pRegSet[3] = 0;
            pRegSet[4] = 0;
        }
    }
    
    adcifb_clear_all_interrupt_flags((avr32_adcifb_t*)AVR32_ADCIFB_ADDRESS);
}

uint32_t Analogs_getResult(uint16_t chan)
{
    return adc_accumulators[(adc_read_regSet * ADC_NUM_CHANS_IN_USE) + chan];
}

uint16_t Analogs_getResultMillivolts(uint16_t chan)
{
    // The maximum value of the result with 1024 oversampling is 2**10 * 1024 = 1048576.
    uint32_t adValue = adc_accumulators[(adc_read_regSet * ADC_NUM_CHANS_IN_USE) + chan];
    
    // ADC voltage is 1.8V .
    uint32_t multiplier = 18;
    
    // mV = adValue * (18/10)V / 1000oversample * 1000mV/V / 1024/adcRange
    //    = adValue * 18 / (10 * (1000 / 1000) * 1024);
    uint32_t divider = 10UL * 1024UL;
    
    uint32_t result = (adValue * multiplier) / divider;
    
    return result;
}

uint16_t Analogs_HasResults(void)
{
    static uint16_t lastSet = 3;
    
    if (lastSet == adc_read_regSet)
    {
        return 0;
    }
    else
    {
        lastSet = adc_read_regSet;
        return 1;
    }
}

static const adcifb_opt_t adc_opt = 
{
    /* Resolution mode */
    .resolution = AVR32_ADCIFB_ACR_RES_10BIT,

    /* Channels Sample & Hold Time in [0,15] */
    // Maximum sample and hold time, for now.
    .shtim = 15,
            
    // This should get a 1MHz ADC clock. Maximum is 6MHz. -JS
    .ratio_clkadcifb_clkadc = 8,

    /*
    * Startup time in [0,127], where
    * Tstartup = startup * 8 * Tclk_adc (assuming Tstartup ~ 15us max)
    */
    .startup = 8,

    /* ADCIFB Sleep Mode disabled */
    .sleep_mode_enable = false
};

void Analogs_Init(void)
{
    adcifb_configure(&AVR32_ADCIFB, &adc_opt);
    // Trigger mode to continuous. Trigger at rate of 1MHz / 1000 -> 4000Hz.
    adcifb_configure_trigger(&AVR32_ADCIFB, AVR32_ADCIFB_TRGMOD_PT, 1000);
    adcifb_channels_enable(&AVR32_ADCIFB, (1 << AD_CHN_MOT_CURR) | (1 << AD_CHN_MOT_BATT_VOLT) | (1 << AD_CHN_MAIN_BATT_CURR) | (1 << AD_CHN_MAIN_BATT_VOLT) | (1 << AD_CHN_PCB_TEMP));
    
    INTC_register_interrupt(adc_results_handler, AVR32_ADCIFB_IRQ, AVR32_INTC_INT1);
    adcifb_enable_data_ready_interrupt(&AVR32_ADCIFB);
    
    // With the conversions automatic, only the first one should need triggering.
    adcifb_start_conversion_sequence(&AVR32_ADCIFB);
}
