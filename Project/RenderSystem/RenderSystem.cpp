#include "RenderSystem.h"
#include "Base/Csdl2.h"
#include "Base/Camera.h"
#include "Base/Vertex.h"
#include "RenderSystem/Shader.h"
#include "RenderSystem/Material.h"
#include "Base/Mesh.h"
#include "Shape/Line2d.h"
#include <assert.h>
#include "Profiler/SimpleProfiler.h"

RenderSystem::RenderSystem(InitDesc &initDesc)
{
	mRenderContext = new RenderContext();
	mRenderContext->width = initDesc.width;
	mRenderContext->height = initDesc.height;
	mRenderContext->bpp = sizeof(uint32);
	mRenderContext->backBuffer = new uint32[initDesc.width * initDesc.height];
	mRenderContext->depthBuffer = new float[initDesc.width * initDesc.height];

	memset(mRenderContext->backBuffer, 0x0, mRenderContext->width * mRenderContext->height * 4);
	memset(mRenderContext->depthBuffer, 0x0, mRenderContext->width * mRenderContext->height * 4);

	mRasterizer = new Rasterizer(mRenderContext);
	mSdl = new CSDL2(initDesc.width, initDesc.height, initDesc.title.c_str());

	mRenderStates[RenderState_ZWrite] = ZWrite_On;
	mRenderStates[RenderState_Fillmode] = ShadedMode;
	//mRenderStates[RenderState_Fillmode] = DepthMode;
	//mRenderStates[RenderState_Fillmode] = WireframeMode;
	mRenderStates[RenderState_Cull] = Cull_Back;

	mClippingPlanes[cp_left] = Plane(1, 0, 0, 1);
	mClippingPlanes[cp_right] = Plane(-1, 0, 0, 1);
	mClippingPlanes[cp_top] = Plane(0, -1, 0, 1);
	mClippingPlanes[cp_bottom] = Plane(0, 1, 0, 1);
	mClippingPlanes[cp_near] = Plane(0, 0, 1, 0);
	mClippingPlanes[cp_far] = Plane(0, 0, -1, 1);

	memset(&mClipVertices, 0, sizeof(mClipVertices));
	memset(&mpClipVertices, 0, sizeof(mpClipVertices));
}

RenderSystem::~RenderSystem()
{
	delete mRenderContext;
	delete mRasterizer;
	delete mSdl;
	delete[] mRenderContext->backBuffer;
	delete[] mRenderContext->depthBuffer;
}

void RenderSystem::SetCurCamera(Camera *cam)
{
	mCurCamera = cam;
}

//void RenderSystem::SetRenderMode(RenderMode mode)
//{
//	mCurrentRenderMode = mode;
//}
// 裁剪的区域判断码
#define CLIP_CODE_MORE_Z 0x0001 // z > z_max
#define CLIP_CODE_LESS_Z 0x0002 // z < z_min
#define CLIP_CODE_IN_Z 0x0004   // z_min < z < z_max

#define CLIP_CODE_MORE_Y 0x0010
#define CLIP_CODE_LESS_Y 0x0020
#define CLIP_CODE_IN_Y 0x0040

#define CLIP_CODE_MORE_X 0x0100
#define CLIP_CODE_LESS_X 0x0200
#define CLIP_CODE_IN_X 0x0400

#define CLIP_CODE_NULL 0x0000

int RenderSystem::ClipEncode(Vector4 &clipPos)
{
	int code = 0;
	if (clipPos.w > mCurCamera->zFar)
	{
		code |= CLIP_CODE_MORE_Z;
	}
	else if (clipPos.w < mCurCamera->zNear)
	{
		code |= CLIP_CODE_LESS_Z;
	}
	else
	{
		code |= CLIP_CODE_IN_Z;
	}

	float nearYMargin = Mathf::Sin(Mathf::Deg2Rad * 0.5 * mCurCamera->Fov) * mCurCamera->zNear;
	float nearXMargin = Mathf::Sin(Mathf::Deg2Rad * 0.5 * mCurCamera->Fov) * mCurCamera->zNear * mCurCamera->Aspect;
	float margin;

	margin = nearXMargin * clipPos.w / mCurCamera->zNear;

	if (clipPos.x > margin)
	{
		code |= CLIP_CODE_MORE_X;
	}
	else if (clipPos.x < -margin)
	{
		code |= CLIP_CODE_LESS_X;
	}
	else
	{
		code |= CLIP_CODE_IN_X;
	}

	margin = nearYMargin * clipPos.w / mCurCamera->zNear;

	if (clipPos.y > margin)
	{
		code |= CLIP_CODE_MORE_Y;
	}
	else if (clipPos.y < -margin)
	{
		code |= CLIP_CODE_LESS_Y;
	}
	else
	{
		code |= CLIP_CODE_IN_Y;
	}

	return code;
}

void RenderSystem::Rasterization(TrianglePrimitive &triangle, Material *mat)
{
	//Screen Mapiing here
	for (int j = 0; j < 3; j++)
	{
		triangle.ndcSpacePoses[j] = triangle.clipSpacePoses[j] / triangle.clipSpacePoses[j].w;
		Vector4 result = mCurCamera->viewportMaxtrix * triangle.ndcSpacePoses[j];
		triangle.screenTriangleVerts[j].position.x = result.x;
		triangle.screenTriangleVerts[j].position.y = result.y;
		triangle.screenTriangleVerts[j].position.z = result.z;
		//for projection correction
		//triangle.screenTriangleVerts[j].w = triangle.clipSpacePoses[j].w;

		//May not be right
		//triangle.screenTriangleVerts[j].position.z = triangle.ndcSpacePoses[j].z;
	}

	//Check if three vert in one line
	float area = triangle.screenTriangleVerts[0].position.x * (triangle.screenTriangleVerts[1].position.y - triangle.screenTriangleVerts[2].position.y) +
				 triangle.screenTriangleVerts[1].position.x * (triangle.screenTriangleVerts[2].position.y - triangle.screenTriangleVerts[0].position.y) +
				 triangle.screenTriangleVerts[2].position.x * (triangle.screenTriangleVerts[0].position.y - triangle.screenTriangleVerts[1].position.y);
	if (Mathf::Abs(area) < 0.0001)
	{
		//	return;
	}

	float slop = (triangle.screenTriangleVerts[0].position.y - triangle.screenTriangleVerts[1].position.y) *
					 (triangle.screenTriangleVerts[0].position.x - triangle.screenTriangleVerts[2].position.x) -
				 (triangle.screenTriangleVerts[0].position.y - triangle.screenTriangleVerts[2].position.y) *
					 (triangle.screenTriangleVerts[0].position.x - triangle.screenTriangleVerts[1].position.x);

	if (Mathf::Abs(slop) < 0.0001)
	{
		//	return;
	}

	//Back face culling
	if (mat->Shader->Cull != Culling::Off)
	{
		Vector4 u = triangle.ndcSpacePoses[1] - triangle.ndcSpacePoses[0];
		Vector4 v = triangle.ndcSpacePoses[2] - triangle.ndcSpacePoses[0];

		float z = (u.x * v.y) - (u.y * v.x);

		if (mat->Shader->Cull == Culling::Back)
		{
			if (z > 0)
				return;
		}
		else if (mat->Shader->Cull == Culling::Front)
		{
			if (z < 0)
				return;
		}
	}

	if (mRenderStates[RenderState_Fillmode] == WireframeMode)
	{
		Line2d line;
		line.start = Vector2(triangle.screenTriangleVerts[0].position.x, triangle.screenTriangleVerts[0].position.y);
		line.end = Vector2(triangle.screenTriangleVerts[1].position.x, triangle.screenTriangleVerts[1].position.y);
		mRasterizer->DrawOneLineInvY(&line, Color::white);

		line.start = Vector2(triangle.screenTriangleVerts[1].position.x, triangle.screenTriangleVerts[1].position.y);
		line.end = Vector2(triangle.screenTriangleVerts[2].position.x, triangle.screenTriangleVerts[2].position.y);
		mRasterizer->DrawOneLineInvY(&line, Color::white);

		line.start = Vector2(triangle.screenTriangleVerts[2].position.x, triangle.screenTriangleVerts[2].position.y);
		line.end = Vector2(triangle.screenTriangleVerts[0].position.x, triangle.screenTriangleVerts[0].position.y);
		mRasterizer->DrawOneLineInvY(&line, Color::white);
	}

	if (mRenderStates[RenderState_Fillmode] == ShadedMode)
	{
		mRasterizer->RasterizeTriangleLarabee2(triangle.screenTriangleVerts, mat->Shader);

		//Debug code
		{
			Line2d line;
			line.start = Vector2(triangle.screenTriangleVerts[0].position.x, triangle.screenTriangleVerts[0].position.y);
			line.end = Vector2(triangle.screenTriangleVerts[1].position.x, triangle.screenTriangleVerts[1].position.y);
			mRasterizer->DrawOneLineInvY(&line, Color::white);

			line.start = Vector2(triangle.screenTriangleVerts[1].position.x, triangle.screenTriangleVerts[1].position.y);
			line.end = Vector2(triangle.screenTriangleVerts[2].position.x, triangle.screenTriangleVerts[2].position.y);
			mRasterizer->DrawOneLineInvY(&line, Color::white);

			line.start = Vector2(triangle.screenTriangleVerts[2].position.x, triangle.screenTriangleVerts[2].position.y);
			line.end = Vector2(triangle.screenTriangleVerts[0].position.x, triangle.screenTriangleVerts[0].position.y);
			mRasterizer->DrawOneLineInvY(&line, Color::white);
		}
	}
	else if (mRenderStates[RenderState_Fillmode] == DepthMode)
	{
		//Covert depth buffer to colorbuff
		mRasterizer->RasterizeTriangleLarabee(triangle.screenTriangleVerts, mat->Shader, true);
	}
}

void RenderSystem::RenderMesh(Mesh *mesh, Matrix4x4 *modelMatrix, Material *mat, bool wireframeDebug)
{
	PROFILE_BEGIN(ShaderUniformSetting);

	Matrix4x4 MVP = mCurCamera->projectionMaxtrix * mCurCamera->viewMatrix * (*modelMatrix);
	mat->Shader->ViewMatrix = &(mCurCamera->viewMatrix);
	mat->Shader->ProjectionMatrix = &(mCurCamera->projectionMaxtrix);
	mat->Shader->ViewportMatrix = &(mCurCamera->viewportMaxtrix);

	mat->Shader->ModelMatrix = modelMatrix;
	Matrix4x4 World2Object = modelMatrix->inverted();
	mat->Shader->World2ObjectMatrix = &World2Object;
	mat->Shader->PVMMatrix = &MVP;
	mat->Shader->RenderMesh = mesh;

	Matrix4x4 MV = mCurCamera->viewMatrix * (*modelMatrix);
	mat->Shader->ModelViewMatrix = &MV;

	mat->Shader->WorldSpaceViewPos = &(mCurCamera->transform.position);

	mShader = mat->Shader;

	PROFILE_END(ShaderUniformSetting);
	
	VSOutput vsOutputs[3];

	//Vertex clipedTriangleVerts[9];
	for (int i = 0; i < mesh->Triangles.size() / 3; i++)
	{
		int vertexInZCount = 0;
		//mvp trasfomation and primitive assemble
		for (int j = 0; j < 3; j++)
		{
			int vertexIndex = mesh->Triangles[i * 3 + j];

			//tianglePrimitive.clipSpacePoses[j] = mat->Shader->vertex(mesh->Triangles[i*3 +j], j);
			vsOutputs[j].position = mat->Shader->vertex(mesh->Triangles[i * 3 + j], j, vsOutputs + j);
			vsOutputs[j].color.r = mesh->Colors[vertexIndex].x;
			vsOutputs[j].color.g = mesh->Colors[vertexIndex].y;
			vsOutputs[j].color.b = mesh->Colors[vertexIndex].z;
			vsOutputs[j].color.a = mesh->Colors[vertexIndex].w;
			vsOutputs[j].uv = mesh->UVs[vertexIndex];
		}

		uint32 iNumVertices = 3;
		mNextFreeClipVertex = 3;
		memcpy(&mClipVertices[0], &vsOutputs[0], sizeof(VSOutput));
		memcpy(&mClipVertices[1], &vsOutputs[1], sizeof(VSOutput));
		memcpy(&mClipVertices[2], &vsOutputs[2], sizeof(VSOutput));

		uint32 iStage = 0;
		mpClipVertices[iStage][0] = &mClipVertices[0];
		mpClipVertices[iStage][1] = &mClipVertices[1];
		mpClipVertices[iStage][2] = &mClipVertices[2];

		bool isTriangleCliped = false;
		// Perform clipping to the frustum planes ---------------------------------
		for (uint32 iPlane = 0; iPlane < 6; ++iPlane)
		{

			iNumVertices = ClipToPlane(iNumVertices, iStage, mClippingPlanes[iPlane]);
			//do not draw this triangle
			if (iNumVertices < 3)
			{
				isTriangleCliped = true;
				break;
			}
			iStage = (iStage + 1) & 1;
		}
		//Draw next triangle
		if (isTriangleCliped)
		{
			continue;
		}

		// Project and rasterize the clipped triangle -----------------------------
		uint32 iVertex;

		// Project the first three vertices for culling
		VSOutput **ppSrc = mpClipVertices[iStage];
		for (iVertex = 0; iVertex < 3; ++iVertex)
		{
			ViewportTransformVertex(ppSrc[iVertex]);
		}

		

		// We do not have to check for culling for each sub-polygon of the triangle, as they
		// are all in the same plane. If the first polygon is culled then all other polygons
		// would be culled, too.
		//if( CullTriangle( ppSrc[0], ppSrc[1], ppSrc[2] ) )
		//	return;

		if (mat->Shader->Cull != Culling::Off)
		{
			Vector4 u = ppSrc[1]->position - ppSrc[0]->position;
			Vector4 v = ppSrc[2]->position - ppSrc[0]->position;

			float z = (u.x * v.y) - (u.y * v.x);

			if (mat->Shader->Cull == Culling::Back)
			{
				if (z > 0)
					continue;
			}
			else if (mat->Shader->Cull == Culling::Front)
			{
				if (z < 0)
					continue;
			}
		}

		// Project the remaining vertices
		for (iVertex = 3; iVertex < iNumVertices; ++iVertex)
			ViewportTransformVertex(ppSrc[iVertex]);

		for (iVertex = 1; iVertex < iNumVertices - 1; ++iVertex)
		{
			RasterizeTriangle(ppSrc[0], ppSrc[iVertex], ppSrc[iVertex + 1], mat->Shader, wireframeDebug);
		}
	}
}

void RenderSystem::RasterizeTriangle(VSOutput *pVSOutput0, VSOutput *pVSOutput1, VSOutput *pVSOutput2, IShader *shader, bool wireframeDebug)
{
	if (mRenderStates[RenderState_Fillmode] == ShadedMode)
	{

		#if USE_SSE
			mRasterizer->RasterizeTriangleLarabeeSSE(pVSOutput0, pVSOutput1, pVSOutput2, shader);
		#else
			mRasterizer->RasterizeTriangleLarabee3(pVSOutput0, pVSOutput1, pVSOutput2, shader);
		#endif

		//Debug code
		if(wireframeDebug)
		{
			Line2d line;
			line.start = Vector2(pVSOutput0->position.x, pVSOutput0->position.y );
			line.end = Vector2(pVSOutput1->position.x, pVSOutput1->position.y);
			mRasterizer->DrawOneLineInvY(&line, Color::white);

			line.start = Vector2(pVSOutput1->position.x,  pVSOutput1->position.y);
			line.end = Vector2(pVSOutput2->position.x, pVSOutput2->position.y);
			mRasterizer->DrawOneLineInvY(&line, Color::white);

			line.start = Vector2(pVSOutput2->position.x, pVSOutput2->position.y);
			line.end = Vector2(pVSOutput0->position.x, pVSOutput0->position.y);
			mRasterizer->DrawOneLineInvY(&line, Color::white);
		}
	}
	else if (mRenderStates[RenderState_Fillmode] == DepthMode)
	{
		//Covert depth buffer to colorbuff
		mRasterizer->RasterizeTriangleLarabeeDepthMode(pVSOutput0, pVSOutput1, pVSOutput2, shader);
	}
}

inline bool RenderSystem::CullTriangle(const VSOutput *i_pVSOutput0, const VSOutput *i_pVSOutput1, const VSOutput *i_pVSOutput2)
{
	// Do backface-culling ----------------------------------------------------
	if (mRenderStates[RenderState_Cull] == Cull_None)
		return false;

	/* vector3 vAB = vScreenSpacePos[1] - vScreenSpacePos[0];
	vector3 vAC = vScreenSpacePos[2] - vScreenSpacePos[0];
	vector3 vFaceNormal; vVector3Cross( vFaceNormal, vAB, vAC );
	float fDirTest = fVector3Dot( vFaceNormal, vector3( 0, 0, 1 ) ); */
	const float fDirTest = (i_pVSOutput1->position.x - i_pVSOutput0->position.x) * (i_pVSOutput2->position.y - i_pVSOutput0->position.y) - (i_pVSOutput1->position.y - i_pVSOutput0->position.y) * (i_pVSOutput2->position.x - i_pVSOutput0->position.x);
	if (mRenderStates[RenderState_Cull] == Cull_Back)
	{
		if (fDirTest <= 0.0f)
			return true;
	}
	else // m3dcull_cw
	{
		if (fDirTest >= 0.0f)
			return true;
	}

	return false;
}

inline void RenderSystem::ViewportTransformVertex(VSOutput *pVSOutput)
{
	if (pVSOutput->position.w < FLT_EPSILON)
		return;

	const float fInvW = 1.0f / pVSOutput->position.w;
	pVSOutput->position.x *= fInvW;
	pVSOutput->position.y *= fInvW;
	pVSOutput->position.z *= fInvW;
	pVSOutput->position.w = 1.0f;

	pVSOutput->position = mCurCamera->viewportMaxtrix * pVSOutput->position;

	// divide shader output registers by w; this way we can interpolate them linearly while rasterizing ...
	pVSOutput->position.w = fInvW;

	pVSOutput->color *= fInvW;
	pVSOutput->uv *= fInvW;
	pVSOutput->normal *= fInvW;

	if (mShader->VaryingsCountBitMask & FirstBitMask)
	{
		pVSOutput->varying[0] *= fInvW;
	}

	if (mShader->VaryingsCountBitMask & SecondBitMask)
	{
		pVSOutput->varying[1] *= fInvW;
	}
}

void RenderSystem::ViewFustrumCulling()
{
}

void RenderSystem::DrawPoint(int x, int y, Color &c)
{
	mRasterizer->DrawPixel(x, y, c);
}

//Left-Bottom as start corner
void RenderSystem::DrawPointLB(int x, int y, Color &c)
{
	mRasterizer->DrawPixel(x, mRenderContext->height - y - 1, c);
}

int RenderSystem::GetWindowWidth()
{
	return mRenderContext->width;
}

int RenderSystem::GetWindowHeight()
{
	return mRenderContext->height;
}

void RenderSystem::Clear(Color &color)
{
	mSdl->Clean(mRenderContext, &color);
}
void RenderSystem::SwapBuffer()
{
	mSdl->SwapBuffer(mRenderContext);
}

int RenderSystem::ClipToPlane(uint32 i_iNumVertices, uint32 i_iSrcIndex, const Plane &i_plane)
{
	VSOutput **ppSrcVertices = mpClipVertices[i_iSrcIndex];
	VSOutput **ppDestVertices = mpClipVertices[(i_iSrcIndex + 1) & 1];

	uint32 iNumClippedVertices = 0;
	for (uint32 i = 0, j = 1; i < i_iNumVertices; ++i, ++j)
	{
		if (j == i_iNumVertices) // wrap over
			j = 0;

		float di, dj; // Distances of current and next vertex to clipping plane.
		di = i_plane * ppSrcVertices[i]->position;
		dj = i_plane * ppSrcVertices[j]->position;

		if (di >= 0.0f)
		{
			ppDestVertices[iNumClippedVertices++] = ppSrcVertices[i];
			if (dj < 0.0f)
			{
				InterpolateVertex(&mClipVertices[mNextFreeClipVertex], ppSrcVertices[i], ppSrcVertices[j], di / (di - dj));
				ppDestVertices[iNumClippedVertices++] = &mClipVertices[mNextFreeClipVertex];
				mNextFreeClipVertex = (mNextFreeClipVertex + 1) % 20; // TODO: 20 enough?
			}
		}
		else
		{
			if (dj >= 0.0f)
			{
				InterpolateVertex(&mClipVertices[mNextFreeClipVertex], ppSrcVertices[j], ppSrcVertices[i], dj / (dj - di));
				ppDestVertices[iNumClippedVertices++] = &mClipVertices[mNextFreeClipVertex];
				mNextFreeClipVertex = (mNextFreeClipVertex + 1) % 20; // TODO: 20 enough?
			}
		}
	}

	return iNumClippedVertices;
}

void RenderSystem::InterpolateVertex(VSOutput *o_pVSOutput, const VSOutput *i_pVSOutputA, const VSOutput *i_pVSOutputB, float factor)
{
	o_pVSOutput->position = Vector4::Lerp(i_pVSOutputA->position, i_pVSOutputB->position, factor);

	o_pVSOutput->normal = Vector3::Lerp(i_pVSOutputA->normal, i_pVSOutputB->normal, factor);

	o_pVSOutput->uv = Vector2::Lerp(i_pVSOutputA->uv, i_pVSOutputB->uv, factor);

	o_pVSOutput->color = Color::Lerp(i_pVSOutputA->color, i_pVSOutputB->color, factor);

	if (mShader->VaryingsCountBitMask & FirstBitMask)
	{
		o_pVSOutput->varying[0] = Vector3::Lerp(i_pVSOutputA->varying[0], i_pVSOutputB->varying[0], factor);
	}

	if (mShader->VaryingsCountBitMask & SecondBitMask)
	{
		o_pVSOutput->varying[1] = Vector3::Lerp(i_pVSOutputA->varying[1], i_pVSOutputB->varying[1], factor);
	}
}
