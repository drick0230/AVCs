#pragma once

#include <cinttypes>
#include "SFML/Graphics/Color.hpp"

namespace sf
{
	typedef Color Colour;
}

#ifdef _WIN32
#define printf() printf_s()
#else
#endif