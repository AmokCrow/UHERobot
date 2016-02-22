/*
 * Motors.h
 *
 * Created: 23.1.2016 10:21:49
 *  Author: crow
 */ 


#ifndef MOTORS_H_
#define MOTORS_H_

#include <stdint.h>

void motor_init(void);
void motor_set_speed(int speed, uint16_t motorNum);



#endif /* MOTORS_H_ */