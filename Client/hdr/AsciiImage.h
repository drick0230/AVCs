#pragma once

#include "IImageConvert.h"
#include "AsciiMap.h"

#include "SFML/Graphics/Image.hpp"

#include <memory>

class AsciiImage : public sf::Image, public IImageConvert
{
public:
	AsciiImage() : sf::Image() {}
	AsciiImage(const AsciiImage& _ref);
	~AsciiImage() {};
	bool toAscii() override;
	std::unique_ptr<AsciiImage> shrink(uint _width, uint _height);
	bool toColourAscii() override;
	bool toGrayscale() override;
	bool saveToFileAsAscii(std::string _fileName) override;
};

inline void testConvert()
{
	AsciiImage image;
	image.loadFromFile("/home/warp/test.png");
	// auto newImage = image.shrink(32,32);
	auto newImage = image.shrink(37,58);
	newImage->saveToFileAsAscii("/home/warp/g_test.txt");
	newImage->saveToFile("/home/warp/g_test.png");
}