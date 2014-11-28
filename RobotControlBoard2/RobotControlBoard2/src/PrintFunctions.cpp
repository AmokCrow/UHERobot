/*
 * PrintFunctions.cpp
 *
 * Created: 18.10.2014 18:01:34
 *  Author: crow
 */ 

#include "PrintFunctions.h"


void PrintFunctions::init(usart_if port)
{
    mUsart = port;
}

// Prints max 1 + 6 digits. The lowest 6.
uint8_t* PrintFunctions::numberToString(uint8_t* toBuffer, int32_t number)
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
    
    uint16_t charsLeft = 7;
    
    while((uNumber < divisor) && (charsLeft > 1))
    {
        divisor /= 10;
        charsLeft--;
    }
    
    while(charsLeft > 0)
    {
        *toBuffer = (uNumber / divisor) + '0';
        toBuffer++;
        
        charsLeft--;
        uNumber %= divisor;
        divisor /= 10;
    }
    
    // Trailing 0.
    *toBuffer = 0;
    
    return toBuffer;
}

uint8_t* PrintFunctions::floatToStr(uint8_t* toBuffer, float number)
{
    int32_t iNumber = (int32_t) number;
    
    toBuffer = numberToString(toBuffer, iNumber);
    
    number -= iNumber;
    
    if(iNumber < 0)
    {
        number *= -100.0f;
    }
    else
    {
        number *= 100.0f;        
    }
    
    uint16_t uNumber = (uint16_t) number;
    
    *toBuffer = '.';
    toBuffer++;
    
    
    *toBuffer = (uNumber / 10) + '0';
    toBuffer++;
    *toBuffer = (uNumber % 10) + '0';
    toBuffer++;
    *toBuffer = 0;
    
    
    return toBuffer;
}    

void PrintFunctions::printString(const char* str)
{
    printUString((const uint8_t*)str);
}

void PrintFunctions::printUString(const uint8_t* str)
{
    while(*str != 0)
    {
        usart_serial_putchar(mUsart, *str);
        str++;
    }
}
