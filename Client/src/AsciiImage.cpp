#include "AsciiImage.h"

#include <fstream>

bool AsciiImage::toAscii()
{
	char asciichar;
	toGrayscale();
	for (unsigned int i = 0; i < getSize().x; ++i)
	{
		for (unsigned int j = 0; j < getSize().y; ++j)
		{
			// asciichar = 
		}
	}
	return true;
}

bool AsciiImage::toColourAscii()
{
	return true;
}

bool AsciiImage::toGrayscale()
{
	for (unsigned int i = 0; i < getSize().x; ++i)
	{
		for (unsigned int j = 0; j < getSize().y; ++j)
		{
			sf::Colour colour = getPixel(i, j);
			sf::Uint8 grayscale =
				RED_COEF * colour.r
				+ GREEN_COEF * colour.g
				+ BLUE_COEF * colour.b;
			colour.a = -1;
			colour.r = grayscale;
			colour.g = 0;
			colour.b = 0;
			setPixel(i, j, colour);
		}
	}
	return true;
}

bool AsciiImage::saveToFileAsAscii(std::string _fileName)
{
	std::ofstream file(_fileName, std::ios::out);

	for (unsigned int j = 0; j < getSize().y; ++j)
	{
		for (unsigned int i = 0; i < getSize().x; ++i)
		{
			file << AsciiMap(getPixel(i, j).r);
		}
		file << std::endl;
	}

	file.close();
	return true;
}
