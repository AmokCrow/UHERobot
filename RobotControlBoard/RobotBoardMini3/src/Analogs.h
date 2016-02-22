/*
 * Analogs.h
 *
 * Created: 23.1.2016 12:55:04
 *  Author: crow
 */ 


#ifndef ANALOGS_H_
#define ANALOGS_H_

#include <stdint.h>

#define ADC_JS_NUM_OVERSAMPLE      1000

typedef enum
{
    ANe_AD_CHAN_MOT_CURR = 0,
    ANe_AD_CHAN_MOT_VOLT,
    ANe_AD_CHAN_BAT_CURR,
    ANe_AD_CHAN_BAT_VOLT,
    ANe_AD_CHAN_PCB_TEMP,
    ANALOGS_NUM_CHANNELS
}
AnalogsChannels_e;

// Will initialize the HW and start a periodic sampling.
void Analogs_Init(void);
// Returns 10bit ADC conversion result x ADC_NUM_OVERSAMPLE samples.
uint32_t Analogs_getResult(uint16_t chan);
// Same as above, but pre-calculated to millivolts.
uint16_t Analogs_getResultMillivolts(uint16_t chan);
//Returns 1 if there are new results. 0 if no change since last checked.
uint16_t Analogs_HasResults(void);

#endif /* ANALOGS_H_ */