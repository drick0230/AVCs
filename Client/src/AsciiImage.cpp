#include "AsciiImage.h"

#include <fstream>

AsciiImage::AsciiImage(const AsciiImage& _ref) : sf::Image(_ref) {}

bool AsciiImage::toAscii()
{
	toGrayscale();
	for (uint i = 0; i < getSize().x; ++i)
	{
		for (uint j = 0; j < getSize().y; ++j)
		{
			sf::Colour colour = getPixel(i, j);
			colour.r = AsciiMap(colour.r);
			setPixel(i, j, colour);
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
	for (uint i = 0; i < getSize().x; ++i)
	{
		for (uint j = 0; j < getSize().y; ++j)
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
	toAscii();
	std::ofstream file(_fileName, std::ios::out);

	for (uint j = 0; j < getSize().y; ++j)
	{
		for (uint i = 0; i < getSize().x; ++i)
		{
			// file << AsciiMap(getPixel(i, j).r);
			file << getPixel(i, j).r;
		}
		file << std::endl;
	}

	file.close();
	return true;
}

std::unique_ptr<AsciiImage> AsciiImage::shrink(uint _width, uint _height)
{
	uint xblock = (getSize().x / _width);
	uint yblock = (getSize().y / _height);

	if (xblock < 1 || yblock < 1)
	{
		return {};
	}

	auto newImage = std::make_unique<AsciiImage>();
	newImage->create(_width, _height);
	uint iblock = 0;
	uint jblock = 0;

	uint32_t number = xblock * yblock;
	sf::Colour colour;

	for (uint i = 0; i < _width; i += xblock)
	{
		for (uint j = 0; j < _height; j += yblock)
		{
			uint32_t red = 0;
			uint32_t green = 0;
			uint32_t blue = 0;
			uint32_t alpha = 0;
			for (uint a = i; a < i + xblock; ++a)
			{
				for (uint b = j; b < j + yblock; ++b)
				{
					colour = getPixel(a, b);
					red += colour.r;
					green += colour.g;
					blue += colour.b;
					alpha += colour.a;
				}
			}
			colour.r = red / number;
			colour.g = green / number;
			colour.b = blue / number;
			colour.a = alpha / number;
			newImage->setPixel(iblock, jblock, colour);
			sf::Colour pi = getPixel(iblock, jblock);
			++jblock;
		}
		++iblock;
		jblock = 0;
	}

	return newImage;
}
