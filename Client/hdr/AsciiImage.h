#pragma once

#include "IImageConvert.h"
#include "AsciiMap.h"

#include "SFML/Graphics/Image.hpp"

class AsciiImage : public sf::Image, public IImageConvert
{
public:
	bool toAscii() override;
	bool toColourAscii() override;
	bool toGrayscale() override;
	bool saveToFileAsAscii(std::string _fileName) override;
};

inline void testConvert()
{
	AsciiImage image;
	image.loadFromFile("/home/warp/test.png");
	image.toGrayscale();
	image.saveToFileAsAscii("/home/warp/g_test.txt");
}