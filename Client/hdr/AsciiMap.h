#pragma once

#include "GeneralDefs.h"

// 70 characters scale
constexpr unsigned char ASCII_MAP[] =
"$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\\|()1{}[]?-_+~<>i!lI;:,\"^`'. ";

inline unsigned char AsciiMap(uint8_t _value)
{
	return ASCII_MAP[(uint8_t)((double)_value*70.0/255.0)];
}