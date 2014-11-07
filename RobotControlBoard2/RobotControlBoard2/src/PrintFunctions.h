/*
 * PrintFunctions.h
 *
 * Created: 18.10.2014 18:02:01
 *  Author: crow
 */ 


#ifndef PRINTFUNCTIONS_H_
#define PRINTFUNCTIONS_H_

#include <stdint.h>

uint8_t* NumberToString(uint8_t* toBuffer, int32_t number);

// Turns a float to a max 6 + 1 + 2 character string,
// while trying to be as small as possible.
// Adds to printed string a trailing 0, and returns a pointer to that character.
uint8_t* FloatToStr(uint8_t* toBuffer, float number);



#endif /* PRINTFUNCTIONS_H_ */