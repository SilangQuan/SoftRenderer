#pragma once
#include "Base/Color.h"
#include "TypeDef.h"
class Vertex;
class Vertex2D;

class Rasterizer
{
public:
	Rasterizer(RenderContext* pcontext);
	~Rasterizer();

	void DrawPixel(int x, int y, Color& c);
private:
	RenderContext *context;
	RenderState renderState;
};