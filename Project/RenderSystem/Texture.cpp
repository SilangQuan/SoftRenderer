#include "Texture.h"
#include "Base/simplebmp.h"
#include "Math/Mathf.h"
#include "Base/bitmap_image.hpp"

string& replace_all_distinct(string&   str,const   string&   old_value,const   string&   new_value)   
{   
    for(string::size_type   pos(0);   pos!=string::npos;   pos+=new_value.length())   {   
        if(   (pos=str.find(old_value,pos))!=string::npos   )   
            str.replace(pos,old_value.length(),new_value);   
        else   break;   
    }   
    return   str;   
}   

Texture::Texture(string path) 
{
    mPath = replace_all_distinct(path, "\\", "/");
    SimpleBMP::load(&mWidth, &mHeight, &mPixels, mPath.c_str());
    if(!mPixels)
    {
        qDebug()<<"Error Load tex" << mPath;
    }
}

long ftol(float f) {
    union {
        float f;
        long l;
    } u;

    u.f = f;
    return u.l;
}

/*
inline int32 ftol( float32 f )
{
#ifdef __amigaos4__
	static hexdouble hd;
	__asm__ ( "fctiw %0, %1" : "=f" (hd.d) : "f" (f) );
	return hd.i.lo;
#else
	static int32 tmp;

	#if _MSC_VER > 1000
	__asm
	{
		fld f
		fistp tmp
	}
	#else
	asm volatile( "fistpl %0" : "=m" ( tmp ) : "t" ( f ) : "st" );
	#endif

	return tmp;
#endif
}
*/

uint32 Texture::SampleLinear(float inU, float inV) const
{
	const float32 fX = inU * mWidth, fY = inV * mHeight;
	int iPixelX = ftol( fX ), iPixelY = ftol( fY );

	int iPixelX2 = iPixelX + 1, iPixelY2 = iPixelY + 1;
	if( iPixelX2 >= mWidth ) iPixelX2 = mWidth - 1;
	if( iPixelY2 >= mHeight ) iPixelY2 = mHeight - 1;

	int iIndexRows[2] = { iPixelY * mWidth, iPixelY2 * mWidth };
	const float32 fInterpolation[2] = { fX - iPixelX, fY - iPixelY };

    static Vector3 vColorRows[2];

    static Vector3 colorSquare[4];

    GetRGB(colorSquare[0], iPixelX, iPixelY);
    GetRGB(colorSquare[1], iPixelX2, iPixelY);
    GetRGB(colorSquare[2], iPixelX, iPixelY2);
    GetRGB(colorSquare[3], iPixelX2, iPixelY2);

    Vector3::Lerp( vColorRows[0], colorSquare[0], colorSquare[1], fInterpolation[0] );
    Vector3::Lerp( vColorRows[1], colorSquare[2], colorSquare[3], fInterpolation[0] );
    static Vector3 vFinalColor; 
    Vector3::Lerp( vFinalColor, vColorRows[0], vColorRows[1], fInterpolation[1] );

    return (((int)vFinalColor.x << 16) | ((int)vFinalColor.y << 8) | (int)vFinalColor.z);
}

void Texture::GetRGB(Vector3& col, int x, int y) const
{
    int startIndex = x * 3 + (mHeight - y - 1) * mWidth * 3;
    col.z = mPixels[startIndex ];
    col.y = mPixels[startIndex + 1];
    col.x = mPixels[startIndex + 2];
}


uint32 Texture::Sample(float inU, float inV) const 
{
    float u = inU - floor(inU);
    float v = inV - floor(inV);

    if (mPixels != NULL) {
		int tu = abs(round(u * (mWidth - 1)));
		int tv = abs(round(v * (mHeight - 1)));
        unsigned char red = 0, green = 0, blue = 0;
        red     =   mPixels[tu * 3 + (mHeight - tv - 1) * mWidth * 3 + 2]; 
	    green   =   mPixels[tu * 3 + (mHeight - tv - 1) * mWidth * 3 + 1]; 
	    blue    =   mPixels[tu * 3 + (mHeight - tv - 1) * mWidth * 3 + 0]; 

        return ((red << 16) | (green << 8) | blue);
	}
    return 0;
}

void Texture::Sample(float u, float v, Color& col) const
{
    if (mPixels != NULL) 
    {
		int tu = abs((int)(u * (mWidth - 1)));
		int tv = abs((int)(v * (mHeight - 1)));
        unsigned char red = 0, green = 0, blue = 0;
        red     =   mPixels[tu * 3 + (mHeight - tv - 1) * mWidth * 3 + 2]; 
	    green   =   mPixels[tu * 3 + (mHeight - tv - 1) * mWidth * 3 + 1]; 
	    blue    =   mPixels[tu * 3 + (mHeight - tv - 1) * mWidth * 3 + 0]; 
        col.uintR = red;
        col.uintG = green;
        col.uintB = blue;
        col.uintA = 255;
	}else{
        col = Color::white;
    }
}


int Texture::GetWidth()
{
    return mWidth;
}

int Texture::GetHeight()
{
    return mHeight;
}