/*
 * PrintFunctions.h
 *
 * Created: 18.10.2014 18:02:01
 *  Author: crow
 */ 


#ifndef PRINTFUNCTIONS_H_
#define PRINTFUNCTIONS_H_

#include <stdint.h>
#include "usart_serial.h"

class PrintFunctions
{
    public:
    PrintFunctions(){}
    
    void init(usart_if port);
    
    uint8_t* numberToString(uint8_t* toBuffer, int32_t number);

    // Turns a float to a max 6 + 1 + 2 character string,
    // while trying to be as small as possible.
    // Adds to printed string a trailing 0, and returns a pointer to that character.
    uint8_t* floatToStr(uint8_t* toBuffer, float number);

    void printString(const char* str);
    void printUString(const uint8_t* str);
    
    private:
    usart_if mUsart;
};




#endif /* PRINTFUNCTIONS_H_ */