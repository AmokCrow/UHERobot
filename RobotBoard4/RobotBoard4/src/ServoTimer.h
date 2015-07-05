/*
 * ServoTimer.h
 *
 * Created: 4.7.2015 23:34:21
 *  Author: crow
 */ 


#ifndef SERVOTIMER_H_
#define SERVOTIMER_H_

#include <tc.h>
#include <ioport.h>

typedef struct 
{
    uint8_t pin;
    uint8_t pos;
}
ServoTimer_Servo_t;

// Note!: Must be called with interrupts disabled.
void ServoTimer_init(TC0_t* timer, ServoTimer_Servo_t* pServos, uint8_t numServos);

void ServoTimer_timerCallback(void);





#endif /* SERVOTIMER_H_ */