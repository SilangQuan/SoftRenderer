#pragma once
#include "Base/Color.h"
#include "TypeDef.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"

class Vertex;
class Vertex2D;
class Line2d;
class IShader;
class VSOutput;


class Rasterizer
{
public:
	Rasterizer(RenderContext* pcontext);
	~Rasterizer();

	bool CohenSutherlandLineClip(Line2d *line, Vector2 min, Vector2 max);
	int EnCode(Vector2& pos, Vector2& min, Vector2& max);
	void DrawOneLine(Line2d *line, Color c);
	void DrawOneLineInvY(Line2d *line, Color c);
	
	void DrawPixel(int x, int y, Color& c);
	void DrawPixelInvY(int x, int y, Color& c);
	
	void DrawPixel(int x, int y,uint32& c);

	void DrawTriangle2D(const Vertex2D& v1, const Vertex2D& v2, const Vertex2D& v3);
	void DrawTriangle(const Vertex* v1, const Vertex* v2, const Vertex* v3);
	void DrawScanLine(const Vertex* v1, const Vertex* v2);
	void DrawScanLine(const Vertex2D* v1, const Vertex2D* v2);

	void RasterizeFragment(Vertex2D* screenPoses,IShader* shader);
	void RasterizeTriangle(const Vertex2D& v1, const Vertex2D& v2, const Vertex2D& v3,IShader* shader);
	void  RasterizeTriangleLarabee(Vertex* vertexs, IShader* shader, bool depthMode = false);
	void  RasterizeTriangleLarabee2(Vertex* vertexs, IShader* shader, bool depthMode = false);
	void RasterizeTriangleLarabee3(VSOutput *pVSOutput0, VSOutput *pVSOutput1, VSOutput *pVSOutput2 , IShader* shader);
	void RasterizeTriangleLarabeeSSE(VSOutput *pVSOutput0, VSOutput *pVSOutput1, VSOutput *pVSOutput2 , IShader* shader);
	void RasterizeTriangleLarabeeDepthMode(VSOutput *pVSOutput0, VSOutput *pVSOutput1, VSOutput *pVSOutput2, IShader* shader);
	

	Vector3 CalcBarycentric(Vector2& A, Vector2& B, Vector2& C, Vector2i& P);
	Vector3 CalcBarycentric(Vector3& A, Vector3& B, Vector3& C, Vector2i& P);

	Vector3 BarycentricFastSSE(float d00, float d01, float d11, Vector2&  v0, Vector2&  v1, Vector2&  v2, bool& isInLine);
private:
	RenderContext *mRenderContext;
	RenderState renderState;
	__m128 mBarycentricCoord;
};