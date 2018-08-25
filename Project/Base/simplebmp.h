
// Date Created: January 03, 2014
// Last Modified: January 05, 2014
// 
// Author: Vallentin <mail@vallentinsource.com>
// 
// Github: https://github.com/VallentinSource/SimpleBMP

#ifndef SIMPLEBMP_H
#define SIMPLEBMP_H



#define SIMPLEBMP_NO_ERROR 0
#define SIMPLEBMP_FOPEN_ERROR 1

#define SIMPLEBMP_INVALID_SIGNATURE 2
#define SIMPLEBMP_INVALID_BITS_PER_PIXEL 3



class SimpleBMP
{
public:

	static int save(const int width, const int height, const unsigned char *pixels, const char *path);

	// Example:
	// 
	// int width = 0, height = 0;
	// unsigned char *pixels = nullptr;
	// 
	// bool success = simplebmp_load(&width, &height, &pixels);
	static int load(int *width, int *height, unsigned char **pixels, const char *path);
	int load2(int *width, int *height, unsigned char **pixels, const char *path);


	static void setPixel(const int width, const int height, unsigned char *pixels, const int x, const int y, const unsigned char red, const unsigned char green, const unsigned char blue);
	static void getPixel(const int width, const int height, const unsigned char *pixels, const int x, const int y, unsigned char *red, unsigned char *green, unsigned char *blue);

	static void setRGB(const int width, const int height, unsigned char *pixels, const int x, const int y, const int rgb);
	static int getRGB(const int width, const int height, const unsigned char *pixels, const int x, const int y);


#ifdef SIMPLEBMP_OPENGL
	static void glReadPixels(const int width, const int height, unsigned char *pixels)
	{
		::glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, pixels);
	}

	static void glTexImage2D(const int width, const int height, unsigned char *pixels)
	{
		::glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, pixels);
	}

	static void glTexImage2D(GLenum target, const int width, const int height, unsigned char *pixels)
	{
		::glTexImage2D(target, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, pixels);
	}
#endif


public:

	int width = 0, height = 0;
	unsigned char *pixels = nullptr;
	int bpp,pitch;

	SimpleBMP(void) {}
	SimpleBMP(const int square);
	SimpleBMP(const int width, const int height, unsigned char *pixels = nullptr);
	~SimpleBMP(void);


	int save(const char *path) const;
	int load(const char *path);


#ifdef SIMPLEBMP_OPENGL
	void glReadPixels(void)
	{
		::glReadPixels(0, 0, this->width, this->height, GL_BGR, GL_UNSIGNED_BYTE, this->pixels);
	}

	void glTexImage2D(GLenum target = GL_TEXTURE_2D) const
	{
		::glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, this->width, this->height, 0, GL_BGR, GL_UNSIGNED_BYTE, this->pixels);
	}
#endif


	void setPixel(const int x, const int y, const unsigned char red, const unsigned char green, const unsigned char blue);
	void getPixel(const int x, const int y, unsigned char *red, unsigned char *green, unsigned char *blue) const;

	void setRGB(const int x, const int y, const int rgb);
	int getRGB(const int x, const int y) const;


	void setPixels(unsigned char *pixels);
	void setPixels(const int width, const int height, unsigned char *pixels);


	void setSize(const int width, const int height);


	void destroy(void);


	bool isValid(void) const;
	bool isValid(const int x, const int y) const;


	int getWidth(void) const;
	int getHeight(void) const;

	unsigned char* getPixels(void) const;
};

#endif