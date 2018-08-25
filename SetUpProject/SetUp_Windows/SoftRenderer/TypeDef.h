#pragma once
// other stuff
typedef unsigned char uint8;
typedef unsigned int uint32;
typedef int int32;

const int LINE_INSIDE = 0; // 0000
const int LINE_LEFT = 1;   // 0001
const int LINE_RIGHT = 2;  // 0010
const int LINE_BOTTOM = 4; // 0100
const int LINE_TOP = 8;    // 1000

enum RenderState
{
	RENDER_STATE_WIREFRAME = 1,
	RENDER_STATE_TEXTURE = 2,
	RENDER_STATE_COLOR = 4,
	RENDER_STATE_POINT = 8,
};

struct RenderContext
{
	int width;		///< 窗口宽度
	int height;		///< 窗口高度
	int bpp;		///< 像素字节数(4byte)
	uint32* backBuffer; ///< 像素数组首地址
	float* depthBuffer; ///< 深度缓冲首地址
};