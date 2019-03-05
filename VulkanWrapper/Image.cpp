#include "Image.h"
#define STB_IMAGE_IMPLEMENTATION   // use of stb functions once and for all
#include <stb_image.h>

bool Image::Load(const char* filepath)
{
	pixels = stbi_load(filepath, &width, &height, &channels, STBI_rgb_alpha);
	components = channels;
	if (!pixels) {
		// image loading failed
		return false;
	}

	// image loading is successful
	return true;
}

int Image::GetWith()
{
	return width;
}

int Image::GetHeight()
{
	return height;
}

int Image::GetComponents()
{
	return components;
}

unsigned char* Image::GetPixels()
{
	return pixels;
}

void Image::UnLoad()
{
	stbi_image_free(pixels);
	width = 0;
	height = 0;
	components = 0;
	pixels = nullptr;
}