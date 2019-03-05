#pragma once
//#define STB_IMAGE_STATIC


class Image
{
public:
	bool Load(const char* filepath);
	int GetWith();
	int GetHeight();
	int GetComponents();
	unsigned char* GetPixels();
	void UnLoad();

private:
	int width;
	int height;
	int components;
	int channels;
	unsigned char* pixels;
};