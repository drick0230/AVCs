#pragma once

#include "GeneralDefs.h"
#include <vector>
#include <string>

class IImageConvert
{
public:
	virtual bool toAscii() = 0;
	// virtual bool toAsciiW() = 0;
	virtual bool toColourAscii() = 0;
	// virtual bool toColourAsciiW() = 0;
	virtual bool toGrayscale() = 0;
	virtual bool saveToFileAsAscii(std::string _fileName) = 0;
protected:
	static constexpr double RED_COEF = 0.2126;
	static constexpr double GREEN_COEF = 0.7152;
	static constexpr double BLUE_COEF = 0.0722;
};