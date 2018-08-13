#pragma once

uint8_t tools_char2Bytes(char cValue1, char cValue2);

uint8_t tools_itoa(int32_t val, char* dst, int32_t radix);

int tools_byteToHexStrs(const uint8_t* source, char* dest, uint32_t sourceLen);
