#include "StdAfx.h"
#include "ToolsBytes.h"


// TODO: reference any additional headers you need in STDAFX.H
// and not in this file

//return length, not include 0
uint8_t tools_itoa(int32_t val, char* dst, int32_t radix)
{
	uint8_t length = 0;

    char *_pdst = dst;   
    if (!val) //允许val等于0 
    {
        *_pdst = '0';
        *++_pdst = '\0';
		length = _pdst- dst + 1;
        return length;
    }        

    if(val <0)
    {
        *_pdst++ = '-';
        val = -val;
    }
    char *_first = _pdst;     
    char _cov;           
    uint16_t _rem;   
    while(val > 0)
    {
        _rem = (unsigned int)(val % radix);
        val /= radix;//每次计算一位 ，从低到高
        if  (_rem > 9)//16进制
            *_pdst++ = (char)(_rem - 10 + 'a'); 
        else
            *_pdst++ = (char)(_rem + '0');      
    }      
    *_pdst-- = '\0';
	length = _pdst - dst + 1;
    
	do
	{ 
		//由于数据是地位到高位储存的，需要转换位置
        _cov = *_pdst;
        *_pdst = *_first;
        *_first = _cov;
        _pdst--;
        _first++;        
    }while(_first < _pdst);  

    return length;
}

uint8_t tools_char2Bytes(char cValue1, char cValue2)
{
    char arrayChar[] = "0123456789ABCDEF";
    int lowValue = -1, highValue = -1;

    for (int i = 0; i < sizeof(arrayChar); i++)
    {
        if (arrayChar[i] == cValue1)
        {
            highValue = i;
            if (lowValue != -1)
            {
                break;
            }
        }
        
        if (arrayChar[i] == cValue2)
        {
            lowValue = i;
            if (highValue != -1)
            {
                break;
            }
        }
    }

    uint8_t nValue = (uint8_t)((highValue << 4) + lowValue);

    return nValue;
}

int tools_byteToHexStrs(const uint8_t* source, char* dest, uint32_t sourceLen)
{
    uint32_t i;
    unsigned char highByte, lowByte;

    for (i = 0; i < sourceLen; i++)
    {
        highByte = source[i] >> 4;
        lowByte = source[i] & 0x0f ;

        highByte += 0x30;

        if (highByte > 0x39)
                dest[i * 2] = highByte + 0x07;
        else
                dest[i * 2] = highByte;

        lowByte += 0x30;
        if (lowByte > 0x39)
            dest[i * 2 + 1] = lowByte + 0x07;
        else
            dest[i * 2 + 1] = lowByte;
    }

    return sourceLen*2;
}
