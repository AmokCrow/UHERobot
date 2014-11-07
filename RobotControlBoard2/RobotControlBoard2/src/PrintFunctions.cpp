/*
 * PrintFunctions.cpp
 *
 * Created: 18.10.2014 18:01:34
 *  Author: crow
 */ 

#include "PrintFunctions.h"

// Prints max 1 + 6 digits. The lowest 6.
uint8_t* NumberToString(uint8_t* toBuffer, int32_t number)
{
    uint32_t uNumber;
    
    if(number < 0)
    {
        *toBuffer = '-';
        toBuffer++;
        
        uNumber = number * (-1);
    }
    else
    {
        uNumber = number;
    }
    
    
    uint32_t divisor = 1000000UL;
    
    if(uNumber > divisor)
    {
        uNumber %= divisor;
    }
    
    uint16_t charsLeft = 6;
    
    while((uNumber < divisor) && (charsLeft > 1))
    {
        divisor /= 10;
        charsLeft--;
    }
    
    while(charsLeft > 0)
    {
        *toBuffer = (uNumber / divisor) + '0';
        toBuffer++;
        
        uNumber %= divisor;
    }
    
    // Trailing 0.
    *toBuffer = 0;
    
    return toBuffer;
}

uint8_t* FloatToStr(uint8_t* toBuffer, float number)
{
    int32_t iNumber = (int32_t) number;
    
    toBuffer = NumberToString(toBuffer, iNumber);
    
    number -= iNumber;
    
    if(iNumber < 0)
    {
        number *= -100.0f;
    }
    else
    {
        number *= 100.0f;        
    }
    
    iNumber = (uint32_t) number;
    
    *toBuffer = '.';
    toBuffer++;
    
    toBuffer = NumberToString(toBuffer, iNumber);
    
    if(toBuffer[-2] == ' ')
    {
        toBuffer[-2] = '0';
    }
    
    return toBuffer;
}    
