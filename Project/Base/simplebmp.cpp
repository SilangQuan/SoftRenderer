
// Date Created: January 03, 2014
// Last Modified: January 05, 2014
// 
// Author: Vallentin <mail@vallentinsource.com>
// 
// Github: https://github.com/VallentinSource/SimpleBMP

#include "simplebmp.h"

#define _CRT_SECURE_NO_WARNINGS
#include <fstream>
#include <memory.h>
#include "Log/Log.h"

const int BMP_MAGIC_ID = 2;
struct bmpfile_magic
{
	unsigned char magic[BMP_MAGIC_ID];
};

struct bmpfile_header
{
	uint32_t file_size;
	uint16_t creator1;
	uint16_t creator2;
	uint32_t bmp_offset;
};

struct bmpfile_dib_info
{
	uint32_t header_size;
	int32_t width;
	int32_t height;
	uint16_t num_planes;
	uint16_t bits_per_pixel;
	uint32_t compression;
	uint32_t bmp_byte_size;
	int32_t hres;
	int32_t vres;
	uint32_t num_colors;
	uint32_t num_important_colors;
};


int SimpleBMP::save(const int width, const int height, const unsigned char *pixels, const char *path)
{
	unsigned char bmp_file_header[14] = { 'B', 'M', 0, 0, 0, 0, 0, 0, 0, 0, 54, 0, 0, 0, };
	unsigned char bmp_info_header[40] = { 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 24, 0, };
	unsigned char bmp_pad[3] = { 0, 0, 0, };

	const int size = 54 + width * height * 3;

	bmp_file_header[2] = static_cast<unsigned char>(size);
	bmp_file_header[3] = static_cast<unsigned char>(size >> 8);
	bmp_file_header[4] = static_cast<unsigned char>(size >> 16);
	bmp_file_header[5] = static_cast<unsigned char>(size >> 24);

	bmp_info_header[4] = static_cast<unsigned char>(width);
	bmp_info_header[5] = static_cast<unsigned char>(width >> 8);
	bmp_info_header[6] = static_cast<unsigned char>(width >> 16);
	bmp_info_header[7] = static_cast<unsigned char>(width >> 24);

	bmp_info_header[8] = static_cast<unsigned char>(height);
	bmp_info_header[9] = static_cast<unsigned char>(height >> 8);
	bmp_info_header[10] = static_cast<unsigned char>(height >> 16);
	bmp_info_header[11] = static_cast<unsigned char>(height >> 24);

	FILE *file = fopen(path, "wb");

	if (file)
	{
		fwrite(bmp_file_header, 1, 14, file);
		fwrite(bmp_info_header, 1, 40, file);

		for (int i = 0; i < height; i++)
		// for (int i = (height - 1); i >= 0; i--)
		{
			fwrite(pixels + (width * i * 3), 3, width, file);
			fwrite(bmp_pad, 1, ((4 - (width * 3) % 4) % 4), file);
		}

		fclose(file);

		return SIMPLEBMP_NO_ERROR;
	}

	return SIMPLEBMP_FOPEN_ERROR;
}

int SimpleBMP::load2(int *width, int *height, unsigned char **pixels, const char *path)
{
	FILE *file = fopen(path, "rb");

	if (file)
	{

		//bmpfile_magic* magic = (bmpfile_magic*)start;
		//bmpfile_header* header = (bmpfile_header*)(start + sizeof(bmpfile_magic));
		//bmpfile_dib_info* info = (bmpfile_dib_info*)(start + +sizeof(bmpfile_magic) + sizeof(bmpfile_header));
		bmpfile_header* header;
		bmpfile_dib_info* info;
		memset(header, 0, sizeof(bmpfile_header));
		memset(info, 0, sizeof(bmpfile_dib_info));

		fread(header, sizeof(bmpfile_header), 1, file);
		fread(info, sizeof(bmpfile_dib_info), 1, file);

		qDebug()<<"BMP file openup Init OK! bpp:" << *path << info->bits_per_pixel;

		(*width)  = info->width;
		(*height)  = abs(info->height);
		bpp = info->bits_per_pixel / 8;
		pitch = info->width * bpp;




		unsigned char bmp_file_header[14] = { 'B', 'M', 0, 0, 0, 0, 0, 0, 0, 0, 54, 0, 0, 0, };
		unsigned char bmp_info_header[40] = { 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 24, 0, };
		unsigned char bmp_pad[3] = { 0, 0, 0, };

		memset(bmp_file_header, 0, sizeof(bmp_file_header));
		memset(bmp_info_header, 0, sizeof(bmp_info_header));

		fread(bmp_file_header, sizeof(bmp_file_header), 1, file);
		fread(bmp_info_header, sizeof(bmp_info_header), 1, file);


		if ((bmp_file_header[0] != 'B') || (bmp_file_header[1] != 'M'))
		{
			fclose(file);
			return SIMPLEBMP_INVALID_SIGNATURE;
		}

		if ((bmp_info_header[14] != 24) && (bmp_info_header[14] != 32))
		{
			fclose(file);
			return SIMPLEBMP_INVALID_BITS_PER_PIXEL;
		}


		int w = (bmp_info_header[4] + (bmp_info_header[5] << 8) + (bmp_info_header[6] << 16) + (bmp_info_header[7] << 24));
		int h = (bmp_info_header[8] + (bmp_info_header[9] << 8) + (bmp_info_header[10] << 16) + (bmp_info_header[11] << 24));
		

		unsigned char *p = new unsigned char[w * h * 3];


		for (int i = 0; i < h; i++)
		// for (int i = (h - 1); i >= 0; i--)
		{
			fread(p + (w * i * 3), 3, w, file);
			fread(bmp_pad, 1, (4 - (w * 3) % 4) % 4, file);
		}


		(*width) = w;
		(*height) = h;
		(*pixels) = p;


		fclose(file);

		return SIMPLEBMP_NO_ERROR;
	}

	return SIMPLEBMP_FOPEN_ERROR;

}

int SimpleBMP::load(int *width, int *height, unsigned char **pixels, const char *path)
{
	FILE *file = fopen(path, "rb");

	if (file)
	{
		unsigned char bmp_file_header[14] = { 'B', 'M', 0, 0, 0, 0, 0, 0, 0, 0, 54, 0, 0, 0, };
		unsigned char bmp_info_header[40] = { 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 24, 0, };
		unsigned char bmp_pad[3] = { 0, 0, 0, };

		memset(bmp_file_header, 0, sizeof(bmp_file_header));
		memset(bmp_info_header, 0, sizeof(bmp_info_header));

		fread(bmp_file_header, sizeof(bmp_file_header), 1, file);
		fread(bmp_info_header, sizeof(bmp_info_header), 1, file);


		if ((bmp_file_header[0] != 'B') || (bmp_file_header[1] != 'M'))
		{
			fclose(file);
			return SIMPLEBMP_INVALID_SIGNATURE;
		}

		if ((bmp_info_header[14] != 24) && (bmp_info_header[14] != 32))
		{
			fclose(file);
			return SIMPLEBMP_INVALID_BITS_PER_PIXEL;
		}


		int w = (bmp_info_header[4] + (bmp_info_header[5] << 8) + (bmp_info_header[6] << 16) + (bmp_info_header[7] << 24));
		int h = (bmp_info_header[8] + (bmp_info_header[9] << 8) + (bmp_info_header[10] << 16) + (bmp_info_header[11] << 24));
		

		unsigned char *p = new unsigned char[w * h * 3];


		for (int i = 0; i < h; i++)
		// for (int i = (h - 1); i >= 0; i--)
		{
			fread(p + (w * i * 3), 3, w, file);
			fread(bmp_pad, 1, (4 - (w * 3) % 4) % 4, file);
		}


		(*width) = w;
		(*height) = h;
		(*pixels) = p;


		fclose(file);

		return SIMPLEBMP_NO_ERROR;
	}

	return SIMPLEBMP_FOPEN_ERROR;
}



void SimpleBMP::setPixel(const int width, const int height, unsigned char *pixels, const int x, const int y, const unsigned char red, const unsigned char green, const unsigned char blue)
{
	pixels[x * 3 + (height - y - 1) * width * 3 + 2] = red;
	pixels[x * 3 + (height - y - 1) * width * 3 + 1] = green;
	pixels[x * 3 + (height - y - 1) * width * 3 + 0] = blue;
}

void SimpleBMP::getPixel(const int width, const int height, const unsigned char *pixels, const int x, const int y, unsigned char *red, unsigned char *green, unsigned char *blue)
{
	if (red)   { (*red)   = pixels[x * 3 + (height - y - 1) * width * 3 + 2]; }
	if (green) { (*green) = pixels[x * 3 + (height - y - 1) * width * 3 + 1]; }
	if (blue)  { (*blue)  = pixels[x * 3 + (height - y - 1) * width * 3 + 0]; }
}


void SimpleBMP::setRGB(const int width, const int height, unsigned char *pixels, const int x, const int y, const int rgb)
{
	const int red = ((rgb >> 16) & 0xFF);
	const int green = ((rgb >> 8) & 0xFF);
	const int blue = (rgb & 0xFF);
	// const int alpha = ((rgb >> 24) & 0xFF);

	SimpleBMP::setPixel(width, height, pixels, x, y, red, green, blue);
}

int SimpleBMP::getRGB(const int width, const int height, const unsigned char *pixels, const int x, const int y)
{
	unsigned char red, green, blue;

	SimpleBMP::getPixel(width, height, pixels, x, y, &red, &green, &blue);

	return ((red << 16) | (green << 8) | blue);
	// return ((alpha << 24) | (red << 16) | (green << 8) | blue);
}



// SimpleBMP::SimpleBMP(void) {}

SimpleBMP::SimpleBMP(const int square)
{
	this->setSize(square, square);
}

SimpleBMP::SimpleBMP(const int width, const int height, unsigned char *pixels)
{
	if (pixels)
	{
		this->setPixels(width, height, pixels);
	}
	else
	{
		this->setSize(width, height);
	}
}

SimpleBMP::~SimpleBMP(void)
{
	this->destroy();
}


int SimpleBMP::save(const char *path) const
{
	return SimpleBMP::save(this->width, this->height, this->pixels, path);
}

int SimpleBMP::load(const char *path)
{
	this->destroy();
	
	return SimpleBMP::load(&this->width, &this->height, &this->pixels, path);
}



void SimpleBMP::setPixel(const int x, const int y, const unsigned char red, const unsigned char green, const unsigned char blue) { SimpleBMP::setPixel(this->width, this->height, this->pixels, x, y, red, green, blue); }
void SimpleBMP::getPixel(const int x, const int y, unsigned char *red, unsigned char *green, unsigned char *blue) const { SimpleBMP::getPixel(this->width, this->height, this->pixels, x, y, red, green, blue); }

void SimpleBMP::setRGB(const int x, const int y, const int rgb) { SimpleBMP::setRGB(this->width, this->height, this->pixels, x, y, rgb); }
int SimpleBMP::getRGB(const int x, const int y) const { return SimpleBMP::getRGB(this->width, this->height, this->pixels, x, y); }


void SimpleBMP::setPixels(unsigned char *pixels)
{
	for (int i = (this->width * this->height * 3 - 1); i >= 0; i--)
	{
		this->pixels[i] = pixels[i];
	}
}

void SimpleBMP::setPixels(const int width, const int height, unsigned char *pixels)
{
	this->setSize(width, height);
	this->setPixels(pixels);
}


void SimpleBMP::setSize(const int width, const int height)
{
	this->destroy();

	this->width = width;
	this->height = height;

	this->pixels = new unsigned char[width * height * 3]; // 3 = R, G, B
}


void SimpleBMP::destroy(void)
{
	this->width = 0;
	this->height = 0;

	if (this->pixels)
	{
		delete[] this->pixels;

		this->pixels = nullptr;
	}
}


bool SimpleBMP::isValid(void) const
{
	if (this->pixels)
	{
		return ((this->width > 0) && (this->height > 0));
	}

	return true;
}

bool SimpleBMP::isValid(const int x, const int y) const
{
	if (this->isValid())
	{
		if (x < 0) { return false; }
		if (y < 0) { return false; }

		if (x >= this->width) { return false; }
		if (y >= this->height) { return false; }

		return true;
	}

	return false;
}


int SimpleBMP::getWidth(void) const { return this->width; }
int SimpleBMP::getHeight(void) const { return this->height; }

unsigned char* SimpleBMP::getPixels(void) const { return this->pixels; }