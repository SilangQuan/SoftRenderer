#pragma once
#include "Base/Color.h"

#include <iostream>  
#include <vector>  
#include <string>  
#include "Log/Log.h"
#include "Base/CTypes.h"


class Texture {
public:
	Texture(string path);

	virtual ~Texture() {
	};


	uint32 Sample(float u, float v) const;
	uint32 SampleLinear(float inU, float inV) const;
	void Sample(float u, float v, Color& col) const;
	int GetWidth();
	int GetHeight();

private:
	int mWidth;
	int mHeight;
	unsigned char *mPixels;
	string mPath;

	void GetRGB(Vector3& col, int x, int y) const; 
	
};
