#include "stdafx.h"
#include "Shape/Line2d.h"
#include "Rasterizer.h"
#include "Base/Vertex.h"
#include "Log/Log.h"
#include "RenderSystem/Shader.h"

Rasterizer::Rasterizer(RenderContext* pcontext)
{
	mRenderContext = pcontext;
}

Rasterizer::~Rasterizer()
{

}

bool Rasterizer::CohenSutherlandLineClip(Line2d *line, Vector2 min, Vector2 max)
{
	int x0 = line->start.x;
	int x1 = line->end.x;

	int y0 = line->start.y;
	int y1 = line->end.y;

	int outcode0 = EnCode(line->start, min, max);
	int outcode1 = EnCode(line->end, min, max);
	bool accept = false;

	Vector2 tmpVec2;

	while (true) {
		// Bitwise OR is 0. Trivially accept and get out of loop. start and end all in center.
		if (!(outcode0 | outcode1))
		{
			accept = true;
			break;
		}
		else if (outcode0 & outcode1) { // Bitwise AND is not 0. Trivially reject and get out of loop
			break;
		}
		else {
			// failed both tests, so calculate the line segment to clip
			// from an outside point to an intersection with clip edge
			double x, y;

			// At least one endpoint is outside the clip rectangle; pick it.
			int outcodeOut = outcode0 ? outcode0 : outcode1;

			// Now find the intersection point;
			// use formulas y = y0 + slope * (x - x0), x = x0 + (1 / slope) * (y - y0)
			if (outcodeOut & LINE_TOP) {           // point is above the clip rectangle
				x = x0 + (x1 - x0) * (max.y - y0) / (y1 - y0);
				y = max.y;
			}
			else if (outcodeOut & LINE_BOTTOM) { // point is below the clip rectangle
				x = x0 + (x1 - x0) * (min.y - y0) / (y1 - y0);
				y = min.y;
			}
			else if (outcodeOut & LINE_RIGHT) {  // point is to the right of clip rectangle
				y = y0 + (y1 - y0) * (max.x - x0) / (x1 - x0);
				x = max.x;
			}
			else if (outcodeOut & LINE_LEFT) {   // point is to the left of clip rectangle
				y = y0 + (y1 - y0) * (min.x - x0) / (x1 - x0);
				x = min.x;
			}

			// Now we move outside point to intersection point to clip
			// and get ready for next pass.
			if (outcodeOut == outcode0) {
				x0 = x;
				y0 = y;
				tmpVec2.x = x0;
				tmpVec2.y = y0;
				outcode0 = EnCode(tmpVec2, min, max);
			}
			else {
				x1 = x;
				y1 = y;
				tmpVec2.x = x1;
				tmpVec2.y = y1;
				outcode1 = EnCode(tmpVec2, min, max);
			}
		}
	}
	if (accept) {
		line->start.x = x0;
		line->start.y = y0;
		line->end.x = x1;
		line->end.y = y1;
	}
	return accept;
}

Vector3 Rasterizer::CalcBarycentric(Vector2& A, Vector2& B, Vector2& C, Vector2i& P)
{
	Vector3 s[2];
	for (int i=2; i--; ) {
		s[i][0] = C[i]-A[i];
		s[i][1] = B[i]-A[i];
		s[i][2] = A[i]-P[i];
	}
	Vector3 u = Vector3::Cross(s[0], s[1]);
	if (std::abs(u[2])>1e-2) // dont forget that u[2] is integer. If it is zero then triangle ABC is degenerate
		return Vector3(1.f-(u.x+u.y)/u.z, u.y/u.z, u.x/u.z);
	return Vector3(-1,1,1); // in this case generate negative coordinates, it will be thrown away by the rasterizator
}

Vector3 Rasterizer::CalcBarycentric(Vector3& A, Vector3& B, Vector3& C, Vector2i& P)
{
	Vector3 s[2];
	for (int i=2; i--; ) {
		s[i][0] = C[i]-A[i];
		s[i][1] = B[i]-A[i];
		s[i][2] = A[i]-P[i];
	}
	Vector3 u = Vector3::Cross(s[0], s[1]);

if(u.z ==0)
{
	qDebug()<<"Fuck"  ;
}

	if (std::abs(u[2])>1e-2) // dont forget that u[2] is integer. If it is zero then triangle ABC is degenerate
		return Vector3(1.f-(u.x+u.y)/u.z, u.y/u.z, u.x/u.z);
	return Vector3(-1,1,1); // in this case generate negative coordinates, it will be thrown away by the rasterizator
}

Vector3 BarycentricFast(Vector3&  a, Vector3&  b, Vector3&  c, Vector3&  p, bool& isInLine)
{
    Vector3 v0 = b - a, v1 = c - a, v2 = p - a;

	float d00 = v0.x * v0.x + v0.y * v0.y;
    float d01 = v0.x * v1.x + v0.y * v1.y;
    float d11 = v1.x * v1.x + v1.y * v1.y;
    float d20 = v2.x * v0.x + v2.y * v0.y;
    float d21 = v2.x * v1.x + v2.y * v1.y;

    float denom = d00 * d11 - d01 * d01;
	//三角形变成了一条线
	if(Mathf::Abs(denom) <0.000001)
	{
		isInLine = true;
		return -1 * Vector3::one;
	}else
	{
		isInLine = false;
	}


    float v = (d11 * d20 - d01 * d21) / denom;
    float w = (d00 * d21 - d01 * d20) / denom;
    float u = 1.0f - v - w;

	return Vector3(u,v,w);
}

Vector3 BarycentricFast2(Vector4&  a, Vector4&  b, Vector4&  c, Vector4&  p, bool& isInLine)
{
    Vector4 v0 = b - a, v1 = c - a, v2 = p - a;

	float d00 = v0.x * v0.x + v0.y * v0.y;
    float d01 = v0.x * v1.x + v0.y * v1.y;
    float d11 = v1.x * v1.x + v1.y * v1.y;
    float d20 = v2.x * v0.x + v2.y * v0.y;
    float d21 = v2.x * v1.x + v2.y * v1.y;

    float denom = d00 * d11 - d01 * d01;
	//三角形变成了一条线
	if(Mathf::Abs(denom) <0.000001)
	{
		isInLine = true;
		return -1 * Vector3::one;
	}else
	{
		isInLine = false;
	}


    float v = (d11 * d20 - d01 * d21) / denom;
    float w = (d00 * d21 - d01 * d20) / denom;
    float u = 1.0f - v - w;

	return Vector3(u,v,w);
}

int Rasterizer::EnCode(Vector2& pos, Vector2& min, Vector2& max)
{
	int code;
	code = LINE_INSIDE;          // initialised as being inside of clip window

	if (pos.x < min.x)           // to the left of clip window
		code |= LINE_LEFT;
	else if (pos.x > max.x)      // to the right of clip window
		code |= LINE_RIGHT;
	if (pos.y < min.y)           // below the clip window
		code |= LINE_BOTTOM;
	else if (pos.y > max.y)      // above the clip window
		code |= LINE_TOP;
	return code;
}

void Rasterizer::DrawOneLine(Line2d *line, Color c)
{
	if (!CohenSutherlandLineClip(line, Vector2(0, 0), Vector2(mRenderContext->width, mRenderContext->height)))
	{
		return;
	}

	int x1 = line->start.x;
	int y1 = line->start.y;
	int x2 = line->end.x;
	int y2 = line->end.y;

	int x, y, rem = 0;

	//line is a pixel
	if (x1 == x2 && y1 == y2) 
	{
		DrawPixel(x1, y1, c);
	}
	//vertical line
	else if (x1 == x2) 
	{
		int inc = (y1 <= y2) ? 1 : -1;
		for (y = y1; y != y2; y += inc) DrawPixel(x1, y, c);
		DrawPixel(x2, y2, c);
	}
	//horizontal line
	else if (y1 == y2) 
	{
		int inc = (x1 <= x2) ? 1 : -1;
		for (x = x1; x != x2; x += inc) DrawPixel(x, y1, c);
		DrawPixel(x2, y2, c);
	}
	else {
		int dx = (x1 < x2) ? x2 - x1 : x1 - x2;
		int dy = (y1 < y2) ? y2 - y1 : y1 - y2;

		// slope < 1
		if (dx >= dy) 
		{
			if (x2 < x1) x = x1, y = y1, x1 = x2, y1 = y2, x2 = x, y2 = y;
			for (x = x1, y = y1; x <= x2; x++) 
			{
				DrawPixel(x, y, c);
				rem += dy;
				if (rem >= dx) 
				{
					rem -= dx;
					y += (y2 >= y1) ? 1 : -1;
					//DrawPixel(x, y, c);
				}
			}
			DrawPixel(x2, y2, c);
		}
		// slope > 1
		else {
			if (y2 < y1) x = x1, y = y1, x1 = x2, y1 = y2, x2 = x, y2 = y;
			for (x = x1, y = y1; y <= y2; y++) 
			{
				DrawPixel(x, y, c);
				rem += dx;
				if (rem >= dy) 
				{
					rem -= dy;
					x += (x2 >= x1) ? 1 : -1;
					//DrawPixel(x, y, c);
				}
			}
			DrawPixel(x2, y2, c);
		}
	}
}

void Rasterizer::DrawOneLineInvY(Line2d *line, Color c)
{
	if (!CohenSutherlandLineClip(line, Vector2(0, 0), Vector2(mRenderContext->width, mRenderContext->height)))
	{
		return;
	}

	int x1 = line->start.x;
	int y1 = line->start.y;
	int x2 = line->end.x;
	int y2 = line->end.y;

	int x, y, rem = 0;

	//line is a pixel
	if (x1 == x2 && y1 == y2) 
	{
		DrawPixelInvY(x1, y1, c);
	}
	//vertical line
	else if (x1 == x2) 
	{
		int inc = (y1 <= y2) ? 1 : -1;
		for (y = y1; y != y2; y += inc) DrawPixelInvY(x1, y, c);
		DrawPixelInvY(x2, y2, c);
	}
	//horizontal line
	else if (y1 == y2) 
	{
		int inc = (x1 <= x2) ? 1 : -1;
		for (x = x1; x != x2; x += inc) DrawPixelInvY(x, y1, c);
		DrawPixelInvY(x2, y2, c);
	}
	else {
		int dx = (x1 < x2) ? x2 - x1 : x1 - x2;
		int dy = (y1 < y2) ? y2 - y1 : y1 - y2;

		// slope < 1
		if (dx >= dy) 
		{
			if (x2 < x1) x = x1, y = y1, x1 = x2, y1 = y2, x2 = x, y2 = y;
			for (x = x1, y = y1; x <= x2; x++) 
			{
				DrawPixelInvY(x, y, c);
				rem += dy;
				if (rem >= dx) 
				{
					rem -= dx;
					y += (y2 >= y1) ? 1 : -1;
					//DrawPixel(x, y, c);
				}
			}
			DrawPixelInvY(x2, y2, c);
		}
		// slope > 1
		else {
			if (y2 < y1) x = x1, y = y1, x1 = x2, y1 = y2, x2 = x, y2 = y;
			for (x = x1, y = y1; y <= y2; y++) 
			{
				DrawPixelInvY(x, y, c);
				rem += dx;
				if (rem >= dy) 
				{
					rem -= dy;
					x += (x2 >= x1) ? 1 : -1;
					//DrawPixel(x, y, c);
				}
			}
			DrawPixelInvY(x2, y2, c);
		}
	}
}

void Rasterizer::DrawPixelInvY(int x, int y, Color& c)
{
	mRenderContext->backBuffer[x + (mRenderContext->height-y-1)* mRenderContext->width] = (c.GetUintA() << 24) | (c.GetUintR() << 16) | (c.GetUintG() << 8) | (c.GetUintB() << 0);
}

void Rasterizer::DrawPixel(int x, int y, Color& c)
{
	mRenderContext->backBuffer[x + y* mRenderContext->width] = (c.GetUintA() << 24) | (c.GetUintR() << 16) | (c.GetUintG() << 8) | (c.GetUintB() << 0);
}

void Rasterizer::DrawPixel(int x, int y, uint32& c)
{
	mRenderContext->backBuffer[x + y* mRenderContext->width] = c;
}

void Rasterizer::RasterizeFragment(Vertex2D* screenPoses,IShader* shader)
{

}

float edgeFunction( Vector3 &a,  Vector3 &b,  Vector3 &c)
{ return (c[0] - a[0]) * (b[1] - a[1]) - (c[1] - a[1]) * (b[0] - a[0]); }

float edgeFunction( Vector3 &a,  Vector3 &b,  Vector2i &c)
{ return (c[0] - a[0]) * (b[1] - a[1]) - (c[1] - a[1]) * (b[0] - a[0]); }



float Linear01Depth( float z, float nearClip, float farClip)
{
	//double zc0, zc1;
	// OpenGL would be this:
	// zc0 = (1.0 - m_FarClip / m_NearClip) / 2.0;
	// zc1 = (1.0 + m_FarClip / m_NearClip) / 2.0;
	// D3D is this:
	//zc0 = 1.0 - m_FarClip / m_NearClip;
	//zc1 = m_FarClip / m_NearClip;

	float eyeZ = 2 * nearClip * farClip /(nearClip + farClip - z * (farClip-nearClip));

	return (eyeZ - nearClip)/(farClip - nearClip);

	//return nearClip * (z + 1.0f) /(nearClip + farClip - z * (farClip - nearClip));
	//return 1.0 / (zc0 * z + zc1);
}

void Rasterizer::RasterizeTriangleLarabeeDepthMode(VSOutput *pVSOutput0, VSOutput *pVSOutput1, VSOutput *pVSOutput2, IShader* shader)
{
	Vector2 bboxmin( Mathf::Infinity,  Mathf::Infinity);
    Vector2 bboxmax(Mathf::NegativeInfinity, Mathf::NegativeInfinity);
    Vector2 clamp(mRenderContext->width-1, mRenderContext->height-1);
	
	//Screen space clip by bounding box
	bboxmin.x = Mathf::Max(0.f, Mathf::Min(bboxmin.x , pVSOutput0->position.x));
	bboxmin.y = Mathf::Max(0.f, Mathf::Min(bboxmin.y , pVSOutput0->position.y));

	bboxmax.x = Mathf::Min(clamp.x, Mathf::Max(bboxmax.x, pVSOutput0->position.x));
	bboxmax.y = Mathf::Min(clamp.y, Mathf::Max(bboxmax.y, pVSOutput0->position.y));

	bboxmin.x = Mathf::Max(0.f, Mathf::Min(bboxmin.x , pVSOutput1->position.x));
	bboxmin.y = Mathf::Max(0.f, Mathf::Min(bboxmin.y , pVSOutput1->position.y));

	bboxmax.x = Mathf::Min(clamp.x, Mathf::Max(bboxmax.x, pVSOutput1->position.x));
	bboxmax.y = Mathf::Min(clamp.y, Mathf::Max(bboxmax.y, pVSOutput1->position.y));

	bboxmin.x = Mathf::Max(0.f, Mathf::Min(bboxmin.x , pVSOutput2->position.x));
	bboxmin.y = Mathf::Max(0.f, Mathf::Min(bboxmin.y , pVSOutput2->position.y));

	bboxmax.x = Mathf::Min(clamp.x, Mathf::Max(bboxmax.x, pVSOutput2->position.x));
	bboxmax.y = Mathf::Min(clamp.y, Mathf::Max(bboxmax.y, pVSOutput2->position.y));

	Vector2i P;
	Color col;

	//float area = edgeFunction(vertexs[0].position, vertexs[1].position, vertexs[2].position); 
	bool isInline = false;
    for (P.x=bboxmin.x; P.x<=bboxmax.x; P.x++) {
        for (P.y=bboxmin.y; P.y<=bboxmax.y; P.y++) 
		{
			Vector4 currentPoint(P.x, P.y, 0, 0);

			Vector3 barycentricCoord = BarycentricFast2(pVSOutput0->position,pVSOutput1->position, pVSOutput2->position, currentPoint, isInline);
			float fInvW =1.0f / (barycentricCoord.x * pVSOutput0->position.w+ barycentricCoord.y*pVSOutput1->position.w +barycentricCoord.z * pVSOutput2->position.w);
			float depth = barycentricCoord.x * pVSOutput0->position.z + barycentricCoord.y*pVSOutput1->position.z + barycentricCoord.z * pVSOutput2->position.z;
			
			if(isInline)
			 	continue;
			
			float threshold = -0.000001;
            if (barycentricCoord.x<threshold || barycentricCoord.y<threshold || barycentricCoord.z<threshold ) continue;

			float w = (barycentricCoord.x * pVSOutput0->position.w, barycentricCoord.y*pVSOutput1->position.w, barycentricCoord.z * pVSOutput2->position.w) ;

			//float depth = (barycentricCoord.x * pVSOutput0->position.z, barycentricCoord.y*pVSOutput1->position.z, barycentricCoord.z * pVSOutput2->position.z);
            
			if(mRenderContext->depthBuffer[P.x + P.y * mRenderContext->width] < depth)
				continue;

			//Do Vertex attribute interpolation
			Vector2 interpUV(0,0);
			interpUV = fInvW * (barycentricCoord.x * pVSOutput0->uv + barycentricCoord.y * pVSOutput1->uv + barycentricCoord.z * pVSOutput2->uv);
			Color interpCol = fInvW * (barycentricCoord.x *pVSOutput0->color + barycentricCoord.y * pVSOutput1->color+ barycentricCoord.z * pVSOutput2->color);
			shader->FragmentInColor =  &interpCol;
			shader->FragmentInUV =  &interpUV;
			
			if(shader->VaryingsCountBitMask & FirstBitMask)
			{
				shader->FragmentVaringOuts[0] = barycentricCoord.x * pVSOutput0->varying[0] 
				+ barycentricCoord.y * pVSOutput1->varying[0] 
				+ barycentricCoord.z * pVSOutput2->varying[0];

				shader->FragmentVaringOuts[0] *= fInvW;
			}

			if(shader->VaryingsCountBitMask & SecondBitMask)
			{
				shader->FragmentVaringOuts[1] = barycentricCoord.x * pVSOutput0->varying[1] 
				+ barycentricCoord.y * pVSOutput1->varying[1] 
				+ barycentricCoord.z * pVSOutput2->varying[1];
				shader->FragmentVaringOuts[1] *= fInvW;
			}

			bool discard = shader->fragment(barycentricCoord, col);

			if (!discard) 
			{
				mRenderContext->depthBuffer[P.x + P.y * mRenderContext->width] = depth;
				
				float linearZ = Linear01Depth(depth, 0.3f, 30);
				col.r = linearZ;
				col.g = linearZ;
				col.b = linearZ;
				DrawPixel(P.x,  mRenderContext->height - P.y - 1, col);
			}
        }
    }
}

void Rasterizer::RasterizeTriangleLarabee3(VSOutput *pVSOutput0, VSOutput *pVSOutput1, VSOutput *pVSOutput2 , IShader* shader)
{
	Vector2 bboxmin( Mathf::Infinity,  Mathf::Infinity);
    Vector2 bboxmax(Mathf::NegativeInfinity, Mathf::NegativeInfinity);
    Vector2 clamp(mRenderContext->width-1, mRenderContext->height-1);
	
	//Screen space clip by bounding box
	bboxmin.x = Mathf::Max(0.f, Mathf::Min(bboxmin.x , pVSOutput0->position.x));
	bboxmin.y = Mathf::Max(0.f, Mathf::Min(bboxmin.y , pVSOutput0->position.y));

	bboxmax.x = Mathf::Min(clamp.x, Mathf::Max(bboxmax.x, pVSOutput0->position.x));
	bboxmax.y = Mathf::Min(clamp.y, Mathf::Max(bboxmax.y, pVSOutput0->position.y));

	bboxmin.x = Mathf::Max(0.f, Mathf::Min(bboxmin.x , pVSOutput1->position.x));
	bboxmin.y = Mathf::Max(0.f, Mathf::Min(bboxmin.y , pVSOutput1->position.y));

	bboxmax.x = Mathf::Min(clamp.x, Mathf::Max(bboxmax.x, pVSOutput1->position.x));
	bboxmax.y = Mathf::Min(clamp.y, Mathf::Max(bboxmax.y, pVSOutput1->position.y));

	bboxmin.x = Mathf::Max(0.f, Mathf::Min(bboxmin.x , pVSOutput2->position.x));
	bboxmin.y = Mathf::Max(0.f, Mathf::Min(bboxmin.y , pVSOutput2->position.y));

	bboxmax.x = Mathf::Min(clamp.x, Mathf::Max(bboxmax.x, pVSOutput2->position.x));
	bboxmax.y = Mathf::Min(clamp.y, Mathf::Max(bboxmax.y, pVSOutput2->position.y));

	Vector2i P;
	Color col;

	//float area = edgeFunction(vertexs[0].position, vertexs[1].position, vertexs[2].position); 
	bool isInline = false;
    for (P.x=bboxmin.x; P.x<=bboxmax.x; P.x++) {
        for (P.y=bboxmin.y; P.y<=bboxmax.y; P.y++) 
		{
			Vector4 currentPoint(P.x, P.y, 0, 0);

			Vector3 barycentricCoord = BarycentricFast2(pVSOutput0->position,pVSOutput1->position, pVSOutput2->position, currentPoint, isInline);
			float fInvW =1.0f / (barycentricCoord.x * pVSOutput0->position.w+ barycentricCoord.y*pVSOutput1->position.w +barycentricCoord.z * pVSOutput2->position.w);
			float depth = barycentricCoord.x * pVSOutput0->position.z + barycentricCoord.y*pVSOutput1->position.z + barycentricCoord.z * pVSOutput2->position.z;
			
			if(isInline)
			 	continue;
			
			float threshold = -0.000001;
            if (barycentricCoord.x<threshold || barycentricCoord.y<threshold || barycentricCoord.z<threshold ) continue;

			float w = (barycentricCoord.x * pVSOutput0->position.w, barycentricCoord.y*pVSOutput1->position.w, barycentricCoord.z * pVSOutput2->position.w) ;

			//float depth = (barycentricCoord.x * pVSOutput0->position.z, barycentricCoord.y*pVSOutput1->position.z, barycentricCoord.z * pVSOutput2->position.z);
            
			if(mRenderContext->depthBuffer[P.x + P.y * mRenderContext->width] < depth)
				continue;

			//Do Vertex attribute interpolation
			Vector2 interpUV(0,0);
			interpUV = fInvW * (barycentricCoord.x * pVSOutput0->uv + barycentricCoord.y * pVSOutput1->uv + barycentricCoord.z * pVSOutput2->uv);
			Color interpCol = fInvW * (barycentricCoord.x *pVSOutput0->color + barycentricCoord.y * pVSOutput1->color+ barycentricCoord.z * pVSOutput2->color);
			shader->FragmentInColor =  &interpCol;
			shader->FragmentInUV =  &interpUV;
			
			if(shader->VaryingsCountBitMask & FirstBitMask)
			{
				shader->FragmentVaringOuts[0] = barycentricCoord.x * pVSOutput0->varying[0] 
				+ barycentricCoord.y * pVSOutput1->varying[0] 
				+ barycentricCoord.z * pVSOutput2->varying[0];

				shader->FragmentVaringOuts[0] *= fInvW;
			}

			if(shader->VaryingsCountBitMask & SecondBitMask)
			{
				shader->FragmentVaringOuts[1] = barycentricCoord.x * pVSOutput0->varying[1] 
				+ barycentricCoord.y * pVSOutput1->varying[1] 
				+ barycentricCoord.z * pVSOutput2->varying[1];
				shader->FragmentVaringOuts[1] *= fInvW;
			}

			bool discard = shader->fragment(barycentricCoord, col);

			if (!discard) 
			{
				mRenderContext->depthBuffer[P.x + P.y * mRenderContext->width] = depth;
				DrawPixel(P.x,  mRenderContext->height - P.y - 1, col);
			}
        }
    }
}


void Rasterizer::RasterizeTriangleLarabee2(Vertex* vertexs,IShader* shader, bool depthmode)
{
	Vector2 bboxmin( Mathf::Infinity,  Mathf::Infinity);
    Vector2 bboxmax(Mathf::NegativeInfinity, Mathf::NegativeInfinity);
    Vector2 clamp(mRenderContext->width-1, mRenderContext->height-1);
	
	//Screen space clip by bounding box
	for (int i=0; i<3; i++) 
	{
		bboxmin.x = Mathf::Max(0.f, Mathf::Min(bboxmin.x , (vertexs + i)->position.x));
		bboxmin.y = Mathf::Max(0.f, Mathf::Min(bboxmin.y , (vertexs + i)->position.y));

		bboxmax.x = Mathf::Min(clamp.x, Mathf::Max(bboxmax.x, (vertexs + i)->position.x));
		bboxmax.y = Mathf::Min(clamp.y, Mathf::Max(bboxmax.y, (vertexs + i)->position.y));
	}

	Vector2i P;
	Color col;


	//float area = edgeFunction(vertexs[0].position, vertexs[1].position, vertexs[2].position); 
	bool isInline = false;
    for (P.x=bboxmin.x; P.x<=bboxmax.x; P.x++) {
        for (P.y=bboxmin.y; P.y<=bboxmax.y; P.y++) 
		{

			Vector3 currentPoint(P.x, P.y, 0);

			Vector3 barycentricCoord = BarycentricFast(vertexs[0].position,vertexs[1].position, vertexs[2].position, currentPoint, isInline);
			Vector3 barycentricCoordClip = Vector3(barycentricCoord.x/vertexs[0].w, barycentricCoord.y/vertexs[1].w, barycentricCoord.z/vertexs[2].w);
			
			if(isInline)
			 	continue;
			
			float threshold = -0.000001;
            if (barycentricCoord.x<threshold || barycentricCoord.y<threshold || barycentricCoord.z<threshold ) continue;

			float depth = barycentricCoordClip.x + barycentricCoordClip.y +barycentricCoordClip.z;
            
			if(mRenderContext->depthBuffer[P.x + P.y * mRenderContext->width] < depth)
				continue;

			//Do Vertex attribute interpolation
			Vector2 interpUV(0,0);
			interpUV.x = (barycentricCoordClip.x * vertexs[0].u + barycentricCoordClip.y * vertexs[1].u + barycentricCoordClip.z * vertexs[2].u)/depth;
			interpUV.y = (barycentricCoordClip.x * vertexs[0].v + barycentricCoordClip.y * vertexs[1].v + barycentricCoordClip.z * vertexs[2].v)/depth;
			
			shader->FragmentInColor =  &(Color::black);
			shader->FragmentInUV =  &interpUV;
			

			bool discard = shader->fragment(barycentricCoord, col);

			if (!discard) 
			{
				mRenderContext->depthBuffer[P.x + P.y * mRenderContext->width] = depth;

				if(depthmode)
				{
					float linearZ = Linear01Depth(depth, 0.3f, 10);
					//z = linearZ;
					col =  Color(linearZ,linearZ,linearZ,1);

					DrawPixel(P.x,  mRenderContext->height - P.y - 1, col);
				}
				else
				{
					//left-bottom as start corner
					DrawPixel(P.x,  mRenderContext->height - P.y - 1, col);
				}
			}
        }
    }
}


void Rasterizer::RasterizeTriangleLarabee(Vertex* vertexs,IShader* shader, bool depthmode)
{
	Vector2 bboxmin( Mathf::Infinity,  Mathf::Infinity);
    Vector2 bboxmax(Mathf::NegativeInfinity, Mathf::NegativeInfinity);
    Vector2 clamp(mRenderContext->width-1, mRenderContext->height-1);
	
	//Screen space clip by bounding box
	for (int i=0; i<3; i++) 
	{
		bboxmin.x = Mathf::Max(0.f, Mathf::Min(bboxmin.x , (vertexs + i)->position.x));
		bboxmin.y = Mathf::Max(0.f, Mathf::Min(bboxmin.y , (vertexs + i)->position.y));

		bboxmax.x = Mathf::Min(clamp.x, Mathf::Max(bboxmax.x, (vertexs + i)->position.x));
		bboxmax.y = Mathf::Min(clamp.y, Mathf::Max(bboxmax.y, (vertexs + i)->position.y));
	}

	Vector2i P;
	Color col;

	//Do perspect correction
	for(int i=0; i<3; i++)
	{
		float wInv =  1/vertexs[i].w;
		//vertexs[i].position.z = 1/vertexs[i].position.z;

		vertexs[i].color *= wInv;
		vertexs[i].u *= wInv;
		vertexs[i].v *= wInv;

		vertexs[i].position.z *= wInv;
		vertexs[i].w = wInv;

		/*if(shader->VaryingsCountBitMask & FirstBitMask)
		{
			shader->FragmentVaringIns[0][i] *= wInv;
		}

		if(shader->VaryingsCountBitMask & SecondBitMask)
		{
			shader->FragmentVaringIns[0][i] *= wInv;
		}*/
	
	}

	//float area = edgeFunction(vertexs[0].position, vertexs[1].position, vertexs[2].position); 
	bool isInline = false;
    for (P.x=bboxmin.x; P.x<=bboxmax.x; P.x++) {
        for (P.y=bboxmin.y; P.y<=bboxmax.y; P.y++) 
		{

			//float w0 = edgeFunction(vertexs[1].position, vertexs[2].position, P);
			//float w1 = edgeFunction(vertexs[2].position, vertexs[0].position, P);
			//float w2 = edgeFunction(vertexs[0].position, vertexs[1].position, P);

			//Vector3 barycentricCoord (w0,w1,w2);
			Vector3 currentPoint(P.x, P.y, 0);

			Vector3 barycentricCoord = BarycentricFast(vertexs[0].position,vertexs[1].position, vertexs[2].position, currentPoint, isInline);
			
			if(isInline)
			 	continue;
			
			float threshold = -0.000001;
            //Vector3 barycentricCoord  = CalcBarycentric(vertexs[0].position, vertexs[1].position, vertexs[2].position, P);
            if (barycentricCoord.x<threshold || barycentricCoord.y<threshold || barycentricCoord.z<threshold ) continue;
            
			//barycentricCoord.x /= area;
			//barycentricCoord.y /= area;
			//barycentricCoord.z /= area;

			//Do interpolation here
			//float z = 1 / (barycentricCoord.x * vertexs[0].position.z + barycentricCoord.y * vertexs[1].position.z + barycentricCoord.z * vertexs[2].position.z ); 
			float z = 1 / (barycentricCoord.x * vertexs[0].w + barycentricCoord.y * vertexs[1].w + barycentricCoord.z * vertexs[2].w ); 
			
			float interpolatedW = 1 / (barycentricCoord.x * vertexs[0].w + barycentricCoord.y * vertexs[1].w + barycentricCoord.z * vertexs[2].w ); 

			if(mRenderContext->depthBuffer[P.x + P.y * mRenderContext->width] < z)
				continue;

			//Do Vertex attribute interpolation
			Color interpCol = barycentricCoord.x * vertexs[0].color + barycentricCoord.y * vertexs[1].color+ barycentricCoord.z * vertexs[2].color;
			Vector2 interpUV(0,0);
			interpUV.x = barycentricCoord.x * vertexs[0].u + barycentricCoord.y * vertexs[1].u + barycentricCoord.z * vertexs[2].u;
			interpUV.y = barycentricCoord.x * vertexs[0].v + barycentricCoord.y * vertexs[1].v + barycentricCoord.z * vertexs[2].v;
			
/*
			if(shader->VaryingsCountBitMask & FirstBitMask)
			{
				
				shader->FragmentVaringOuts[0] = barycentricCoord.x * shader->FragmentVaringIns[0][0] 
				+ barycentricCoord.y * shader->FragmentVaringIns[0][1] 
				+ barycentricCoord.z * shader->FragmentVaringIns[0][2];

				shader->FragmentVaringOuts[0] *= z;
			}

			if(shader->VaryingsCountBitMask & SecondBitMask)
			{
				shader->FragmentVaringOuts[1] = barycentricCoord.x * shader->FragmentVaringIns[1][0] 
				+ barycentricCoord.y * shader->FragmentVaringIns[1][1] 
				+ barycentricCoord.z * shader->FragmentVaringIns[1][2];
				shader->FragmentVaringOuts[1] *=z;
				
			}
			*/
			//Do perspect correction
			interpCol *= interpolatedW;
			interpUV *= interpolatedW;
			shader->FragmentInColor =  &interpCol;
			shader->FragmentInUV =  &interpUV;
			

			bool discard = shader->fragment(barycentricCoord, col);

			if (!discard) 
			{
				mRenderContext->depthBuffer[P.x + P.y * mRenderContext->width] = z;

				if(depthmode)
				{
					float linearZ = Linear01Depth(z, 0.3f, 10);
					//z = linearZ;
					col =  Color(linearZ,linearZ,linearZ,1);

					DrawPixel(P.x,  mRenderContext->height - P.y - 1, col);
				}
				else
				{
					//left-bottom as start corner
					DrawPixel(P.x,  mRenderContext->height - P.y - 1, col);
				}

			}
        }
    }
}

void Rasterizer::RasterizeTriangle(const Vertex2D& v1, const Vertex2D& v2, const Vertex2D& v3,IShader* shader)
{
// sort a b c by y, top to bottom
	const Vertex2D *a = &v1;
	const Vertex2D *b = &v2;
	const Vertex2D *c = &v3;
	if (a->position.y > b->position.y) {
		std::swap(a, b);
	}
	if (b->position.y > c->position.y) {
		std::swap(b, c);
	}
	if (a->position.y > b->position.y) {
		std::swap(a, b);
	}

	float middleFactor = (float)(b->position.y - a->position.y) / (c->position.y - a->position.y);
	Vertex2D middle = a->interpolate(*c, middleFactor);

	int startY = a->position.y;
	int endY = b->position.y;
	for (int y = startY; y <= endY; ++y) {
		float factor = 0;
		if (endY != startY) {
			factor = (float)(y - startY) / (endY - startY);
		}
		Vertex2D va = a->interpolate(*b, factor);
		Vertex2D vb = a->interpolate(middle, factor);
		DrawScanLine(&va, &vb);
	}
	startY = b->position.y;
	endY = c->position.y;
	for (int y = startY; y <= endY; ++y) {
		float factor = 0;
		if (endY != startY) {
			factor = (float)(y - startY) / (endY - startY);
		}
		Vertex2D va = b->interpolate(*c, factor);
		Vertex2D vb = middle.interpolate(*c, factor);
		DrawScanLine(&va, &vb);
	}
}


void Rasterizer::DrawTriangle2D(const Vertex2D& v1, const Vertex2D& v2, const Vertex2D& v3)
{
	// sort a b c by y, top to bottom
	const Vertex2D *a = &v1;
	const Vertex2D *b = &v2;
	const Vertex2D *c = &v3;
	if (a->position.y > b->position.y) {
		std::swap(a, b);
	}
	if (b->position.y > c->position.y) {
		std::swap(b, c);
	}
	if (a->position.y > b->position.y) {
		std::swap(a, b);
	}

	float middleFactor = (float)(b->position.y - a->position.y) / (c->position.y - a->position.y);
	Vertex2D middle = a->interpolate(*c, middleFactor);

	int startY = a->position.y;
	int endY = b->position.y;
	for (int y = startY; y <= endY; ++y) {
		float factor = 0;
		if (endY != startY) {
			factor = (float)(y - startY) / (endY - startY);
		}
		Vertex2D va = a->interpolate(*b, factor);
		Vertex2D vb = a->interpolate(middle, factor);
		DrawScanLine(&va, &vb);
	}
	startY = b->position.y;
	endY = c->position.y;
	for (int y = startY; y <= endY; ++y) {
		float factor = 0;
		if (endY != startY) {
			factor = (float)(y - startY) / (endY - startY);
		}
		Vertex2D va = b->interpolate(*c, factor);
		Vertex2D vb = middle.interpolate(*c, factor);
		DrawScanLine(&va, &vb);
	}
}



void Rasterizer::DrawTriangle(const Vertex* v1, const Vertex* v2, const Vertex* v3)
{
	// sort a b c by y, top to bottom
	const Vertex *a = v1;
	const Vertex *b = v2;
	const Vertex *c = v3;
	if (a->position.y > b->position.y) {
		std::swap(a, b);
	}
	if (b->position.y > c->position.y) {
		std::swap(b, c);
	}
	if (a->position.y > b->position.y) {
		std::swap(a, b);
	}

	float middleFactor = (float)(b->position.y - a->position.y) / (c->position.y - a->position.y);
	Vertex middle = a->interpolate(*c, middleFactor);

	int startY = a->position.y;
	int endY = b->position.y;
	for (int y = startY; y <= endY; ++y) {
		float factor = 0;
		if (endY != startY) {
			factor = (float)(y - startY) / (endY - startY);
		}
		Vertex va = a->interpolate(*b, factor);
		Vertex vb = a->interpolate(middle, factor);
		DrawScanLine(&va, &vb);
		//drawScanline(va, vb, y, texture);
	}
	startY = b->position.y;
	endY = c->position.y;
	for (int y = startY; y <= endY; ++y) {
		float factor = 0;
		if (endY != startY) {
			factor = (float)(y - startY) / (endY - startY);
		}
		Vertex va = b->interpolate(*c, factor);
		Vertex vb = middle.interpolate(*c, factor);
		DrawScanLine(&va, &vb);
		//drawScanline(va, vb, y, texture);
	}
}


void Rasterizer::DrawScanLine(const Vertex2D* v1, const Vertex2D* v2)
{
	int x1 = v1->position.x;
	int x2 = v2->position.x;
	int flag = x2 > x1 ? 1 : -1;
	float factor = 0;

	for (int x = x1, i = 0; i < (x2 - x1) * flag + 1; i++, x += flag) {
		if (x2 != x1) {
			factor = (float)(x - x1) / (x2 - x1);
		}
		Color color = Color::Lerp(v1->color, v2->color, factor);
		Vertex2D v = v1->interpolate(*v2, factor);
		DrawPixel(x, v1->position.y, color);
	}
}


void Rasterizer::DrawScanLine(const Vertex* v1, const Vertex* v2)
{
	int x1 = v1->position.x;
	int x2 = v2->position.x;
	int flag = x2 > x1 ? 1 : -1;
	float factor = 0;

	for (int x = x1, i = 0; i < (x2 - x1) * flag + 1; i++, x += flag) {
		if (x2 != x1) {
			factor = (float)(x - x1) / (x2 - x1);
		}
		Color color = Color::Lerp(v1->color, v2->color, factor);
		Vertex v = v1->interpolate(*v2, factor);
		int z = Mathf::Lerp(v1->position.z, v2->position.z, factor);
		//auto color = texture->sample(v.u, v.v);
		//Vertex tmp = v1.interpolate(v2, factor);
		DrawPixel(x, v1->position.y, color);
		//DrawPoint(Vector3(x, v1.position.y, 1), color);
		//DrawPoint(x, v1.position.y, z, color);
		//DrawPoint(v.position, color);

	}
}