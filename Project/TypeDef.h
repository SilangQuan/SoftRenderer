#pragma once

#include <string>

// other stuff
typedef unsigned char uint8;
typedef unsigned int uint32;
typedef int int32;

const int LINE_INSIDE = 0; // 0000
const int LINE_LEFT = 1;   // 0001
const int LINE_RIGHT = 2;  // 0010
const int LINE_BOTTOM = 4; // 0100
const int LINE_TOP = 8;    // 1000



/// Defines the supported culling modes.
enum CullMode
{
	Cull_None,	///< Back faces won't be culled.
	Cull_Back,		///< Culls faces with clockwise vertices.
	Cull_Front		///< Culls faces with counterclockwise vertices (default).
};


enum Fillmode{
    ShadedMode,
    WireframeMode,
	PointMode,
    DepthMode,
};

enum ZWriteMode{
	ZWrite_On,
	ZWrite_Off,
};

enum RenderState
{
	RenderState_ZWrite,		///< Set this to true(default) to enable writing to the depth-buffer during rasterization. If no depth-buffer is available or has been disabled by setting m3drs_zenable to false, this renderstate has no effect.
	RenderState_Fillmode,			///< Fillmode. Set this renderstate to a member of the enumeration m3dfill. Default: m3dfill_solid.
	RenderState_Cull,			///< Cullmode. Set this renderstate to a member of the enumeration m3dcull. Default: m3dcull_ccw.
	NumRenderstates
};


struct RenderContext
{
	int width;		///< 窗口宽度
	int height;		///< 窗口高度
	int bpp;		///< 像素字节数(4byte)
	uint32* backBuffer; ///< 像素数组首地址
	float* depthBuffer; ///< 深度缓冲首地址
};

struct InitDesc
{
    bool isFullScreen;
    unsigned int width;
    unsigned int height;
    std::string title;
};
