#include "Rasterizer.h"
#include "TypeDef.h"
#include "Base/Vertex.h"
#include "Math/Vector4.h"
#include "Shape/Plane.h"
class CSDL2;
class Rasterizer;
class IShader;
class Camera;
class Mesh;
class Matrix4x4;
class Material;
class VSOutput;

struct TrianglePrimitive
{
    Vector4 clipSpacePoses[3];
    Vector4 ndcSpacePoses[3];
    Vertex screenTriangleVerts[3];
};

enum clippingplanes
{
	cp_left = 0,			///< Left frustum clipping plane.
	cp_right,			///< Right frustum clipping plane.
	cp_top,				///< Top frustum clipping plane.
	cp_bottom,			///< Bottom frustum clipping plane.
	cp_near,				///< Near frustum clipping plane.
	cp_far,				///< Far frustum clipping plane.

	cp_numplanes
};


class RenderSystem
{
    public:
        RenderSystem(InitDesc& initDesc);
        ~RenderSystem();

        void RenderMesh(Mesh* mesh, Matrix4x4* modelMatrix, Material* mat, bool wireframeDebug = false);
        void ClipTriangle(Mesh* mesh,Matrix4x4* modelMatrix, Material* mat,int triangleIndex, float zNear, float zFar );
        int ClipEncode(Vector4& clipPos);
        void Rasterization(TrianglePrimitive& triangle, Material* mat);
        void DrawPoint(int x, int y, Color& c);
        void DrawPointLB(int x, int y, Color& c);
        void ViewFustrumCulling();
        int GetWindowWidth();
        int GetWindowHeight();
        void Clear(Color& color);
        void SwapBuffer();
        void SetCurCamera(Camera* cam);

       // void SetRenderState();

        int ClipToPlane( uint32 i_iNumVertices, uint32 i_iSrcIndex, const Plane &i_plane);
        void InterpolateVertex( VSOutput *o_pVSOutput, const VSOutput *i_pVSOutputA, const VSOutput *i_pVSOutputB, float i_fInterpolation );
        void ViewportTransformVertex( VSOutput *io_pVSOutput);
        bool CullTriangle( const VSOutput *i_pVSOutput0, const VSOutput *i_pVSOutput1, const VSOutput *i_pVSOutput2 );
        void RasterizeTriangle(VSOutput *pVSOutput0, VSOutput *pVSOutput1, VSOutput *pVSOutput2 , IShader* shader, bool wireframeDebug = false);
        
    private:
        RenderContext* mRenderContext;
        IShader* mShader;
        Rasterizer* mRasterizer;
        CSDL2 *mSdl;
        Camera*mCurCamera;

        uint32	mRenderStates[NumRenderstates];	///< The renderstates.


        Plane mClippingPlanes[6];
        VSOutput mClipVertices[20];
        VSOutput *mpClipVertices[2][20];
        int mNextFreeClipVertex;
};