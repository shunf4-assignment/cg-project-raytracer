#include "common.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"
#include <algorithm>

GLImage::GLImage(const char *fileName)
{
	int origChannel;
	imageBuf = stbi_load(fileName, &w, &h, &origChannel, channel);

	if (imageBuf == nullptr)
	{
		throw MyGLException("ERROR: This texture (%s) is not valid", fileName);
	}

	if ((w & (w - 1)) != 0 || (h & (h - 1)) != 0)
	{
		logFileP->error("WARNING: This image (%s, %dx%d) is not power-of-2 dimensions", fileName, w, h);
	}
	reverse();
}

void GLImage::reverse()
{
	int halfHeight = h / 2;
	unsigned char *top = nullptr;
	unsigned char *bottom = nullptr;
	int bytePerLine = h * channel;

	top = imageBuf;
	bottom = imageBuf + (h - 1) * bytePerLine;
	for (int row = 0; row < halfHeight; row++)
	{
		top = imageBuf + row * bytePerLine;
		bottom = imageBuf + (h - row - 1) * bytePerLine;
		std::swap_ranges(top, top + bytePerLine, bottom);
		top += bytePerLine;
		bottom -= bytePerLine;
	}
}

void GLImage::store(const char * fileName)
{
	if (!stbi_write_png(fileName, w, h, channel, imageBuf, w * channel))
	{
		throw MyGLException("ERROR: write png to %s failed", fileName);
	}
}

GLImage::~GLImage()
{
	stbi_image_free(imageBuf);
}

GLPixel *GLImage::operator[](int i)
{
	return reinterpret_cast<GLPixel *>(imageBuf + i * w * channel);
}

GLPixel &GLImage::get(int i)
{
	return *reinterpret_cast<GLPixel *>(imageBuf + i * channel);
}