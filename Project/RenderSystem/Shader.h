#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Math/Matrix.h"
#include "Base/Color.h"
#include "Base/Mesh.h"
#include "RenderSystem/Texture.h"
#include <math.h>
#include "Math/Mathf.h"
#include "Base/Vertex.h"
#include "Profiler/SimpleProfiler.h"

static uint FirstBitMask = 0x1;
static uint SecondBitMask = 0x10;
static uint ThirdBitMask = 0x100;
static uint ForthBitMask = 0x1000;
static const int UnitsCout = 4;

enum Culling{
    Back,
    Front,
    Off,
};

class IShader
{
    public:
    

    Vector3 *DirectionLightDir;
    float* DirectionLightIntensity;

    Vector3* WorldSpaceViewPos;

    Matrix4x4 *ViewMatrix;
    Matrix4x4 *ModelMatrix;
    Matrix4x4 *ProjectionMatrix;
    Matrix4x4 *PVMMatrix;
    Matrix4x4 *ViewportMatrix;
    Matrix4x4 *ModelViewMatrix;
    Matrix4x4* World2ObjectMatrix;

    Mesh* RenderMesh;

    Color* FragmentInColor;
    Vector2* FragmentInUV;

    //Vector3** FragmentVaringIns;
    Vector3* FragmentVaringOuts;
    uint8 VaryingsCountBitMask;


    Texture* TextureUnit0;
    Texture* TextureUnit1;
    Texture* TextureUnit2;

    Culling Cull = Culling::Back;

    IShader()
    {
        VaryingsCountBitMask = 0;
        FragmentVaringOuts = new Vector3[UnitsCout];

    }
    virtual ~IShader(){
        delete[] FragmentVaringOuts;
    };
    virtual Vector4 vertex(int bufferIndex,int triangleIndex, VSOutput* vsOutput)  = 0;
    virtual bool fragment(const Vector3& barycentric, Color &color) = 0;
};

class SimpleShader : public IShader 
{
   virtual Vector4 vertex(int bufferIndex,int triangleIndex, VSOutput* vsOutput) 
    {
        Vector4 tmp;
        tmp.x = RenderMesh->Vertices[bufferIndex].x;
        tmp.y = RenderMesh->Vertices[bufferIndex].y;
        tmp.z = RenderMesh->Vertices[bufferIndex].z;
        tmp.w = 1;

        return *PVMMatrix * tmp; // transform it to screen coordinates
    }

    virtual bool fragment(const Vector3& barycentric,Color &color) 
    {
        color = Color::blue;
        return false;                              // no, we do not discard this pixel
    }
};

class VertexColorShader : public IShader 
{
   virtual Vector4 vertex(int bufferIndex,int triangleIndex, VSOutput* vsOutput)  
    {
        Vector4 tmp;
        tmp.x = RenderMesh->Vertices[bufferIndex].x;
        tmp.y = RenderMesh->Vertices[bufferIndex].y;
        tmp.z = RenderMesh->Vertices[bufferIndex].z;
        tmp.w = 1;
        return *PVMMatrix * tmp; // transform it to screen coordinates
    }

    virtual bool fragment(const Vector3& barycentric,Color &color) 
    {
        color = *FragmentInColor;
         //color = Color::white;
         //color.r = FragmentInUV->x;
         //color.g = FragmentInUV->y;
        return false;                              // no, we do not discard this pixel
    }
};

class VertexColorTextureShader : public IShader 
{
    virtual Vector4 vertex(int bufferIndex,int triangleIndex, VSOutput* vsOutput) 
    {
        PROFILE_BEGIN(VertexColorTextureShader_vert);
        Vector4 tmp;
        tmp.x = RenderMesh->Vertices[bufferIndex].x;
        tmp.y = RenderMesh->Vertices[bufferIndex].y;
        tmp.z = RenderMesh->Vertices[bufferIndex].z;
        tmp.w = 1;
        PROFILE_END(VertexColorTextureShader_vert);
        return *PVMMatrix * tmp; // tOransform it to screen coordinates
    }

    virtual bool fragment(const Vector3& barycentric,Color &color) 
    {
        PROFILE_BEGIN(VertexColorTextureShader_fragment);
        
        color = (Color)(TextureUnit0->Sample(FragmentInUV->x,FragmentInUV->y));
        color *= *FragmentInColor;
        PROFILE_END(VertexColorTextureShader_fragment);
        return false;      
    }                        
};

class SimpleTextureShader : public IShader 
{
   virtual Vector4 vertex(int bufferIndex,int triangleIndex, VSOutput* vsOutput) 
    {
        Vector4 tmp;
        tmp.x = RenderMesh->Vertices[bufferIndex].x;
        tmp.y = RenderMesh->Vertices[bufferIndex].y;
        tmp.z = RenderMesh->Vertices[bufferIndex].z;
        tmp.w = 1;
        return *PVMMatrix * tmp; // tOransform it to screen coordinates
    }

    virtual bool fragment(const Vector3& barycentric,Color &color) 
    {
        //color = (Color)(TextureUnit0->Sample(Mathf::Clamp(FragmentInUV->x,0,1),Mathf::Clamp(FragmentInUV->y, 0,1)));
        color = (Color)(TextureUnit0->Sample(FragmentInUV->x,FragmentInUV->y));
        //color = (Color)(TextureUnit0->SampleLinear(FragmentInUV->x,FragmentInUV->y));

        //color = Color(FragmentInUV->x,FragmentInUV->y,0,1);
        //color = *FragmentInColor;
        
       // color = Color::blue;
        return false;                              // no, we do not discard this pixel
    }
};


class GouraudShader : public IShader 
{

    public:
    GouraudShader()
    {
        VaryingsCountBitMask |= FirstBitMask;
    }

    virtual Vector4 vertex(int bufferIndex,int vertexIndex, VSOutput* vsOutput) 
    {

        //varying_intensity[index] = std::max(0.f,Vector3::Dot(Vector3::Normalize(RenderMesh->Normals[index]), *DirectionLightDir)); // get diffuse lighting intensity
        Vector3 normilzed = Vector3::Normalize(RenderMesh->Normals[bufferIndex]);
        Vector4 worldSpaceNormal = *ModelViewMatrix * Vector4(normilzed, 0.0f);
        //Vector4 worldSpaceNormal = Vector4(RenderMesh->Normals[bufferIndex], 0.0f);
        
        //diffuse term
        float ndotl = std::max(Vector3::Dot( Vector3::Normalize(worldSpaceNormal.xyz), Vector3::Normalize(*DirectionLightDir)), 0.0f);
        //varying_intensity[triangleIndex] = ndotl;
        //varyingVecs[triangleIndex] = Vector3::Normalize(worldSpaceNormal.xyz);
        
        
        vsOutput->varying[0] = Vector3(ndotl,ndotl,ndotl);
        //vsOutput->varying[0] = Vector3(worldSpaceNormal.x,worldSpaceNormal.y,worldSpaceNormal.z);
        //FragmentVaringIns[0][triangleIndex] = normilzed;

        Vector4 tmp;
        tmp.x = RenderMesh->Vertices[bufferIndex].x;
        tmp.y = RenderMesh->Vertices[bufferIndex].y;
        tmp.z = RenderMesh->Vertices[bufferIndex].z;
        tmp.w = 1;
        return *PVMMatrix * tmp; // transform it to screen coordinates
    }

    virtual bool fragment(const Vector3& barycentric, Color &color) {
        //float intensity = Vector3::Dot(varying_intensity, barycentric);   // interpolate intensity for the current pixel
       // Vector3 normalCol = barycentric.x * varyingVecs[0] +barycentric.y * varyingVecs[1] + barycentric.z * varyingVecs[2];
        Vector3 normalCol = FragmentVaringOuts[0];
        //Color diffuse = Color::blue;

        //float intensity = FragmentVaringOuts[0].x;
        //color = diffuse * intensity; // well duh
        //color.r = 0.5+ 0.5*normalCol.x;
        //color.g = 0.5+ 0.5*normalCol.y;
        //color.b = 0.5+ 0.5*normalCol.z;
        Color albedo = (Color)(TextureUnit0->Sample(FragmentInUV->x,FragmentInUV->y));

        color.r = (0.2 + 0.8*normalCol.x) * albedo.r;
        color.g = (0.2 + 0.8*normalCol.y) * albedo.g;
        color.b = (0.2 + 0.8*normalCol.z) * albedo.b;

        //color.r = albedo.r;
        //color.g = albedo.g;
        //color.b = albedo.b;
        color.a = 1;

        return false;                              // no, we do not discard this pixel
    }
};


class AlphaTestShader : public IShader {

    public:
    AlphaTestShader()
    {
        VaryingsCountBitMask |= FirstBitMask;
    }

    virtual Vector4 vertex(int bufferIndex,int vertexIndex, VSOutput* vsOutput) 
    {

        //varying_intensity[index] = std::max(0.f,Vector3::Dot(Vector3::Normalize(RenderMesh->Normals[index]), *DirectionLightDir)); // get diffuse lighting intensity
        Vector3 normilzed = Vector3::Normalize(RenderMesh->Normals[bufferIndex]);
        Vector4 worldSpaceNormal = *ModelViewMatrix * Vector4(normilzed, 0.0f);
        //Vector4 worldSpaceNormal = Vector4(RenderMesh->Normals[bufferIndex], 0.0f);
        
        //diffuse term
        float ndotl = std::max(Vector3::Dot( Vector3::Normalize(worldSpaceNormal.xyz), Vector3::Normalize(*DirectionLightDir)), 0.0f);
        //varying_intensity[triangleIndex] = ndotl;
        //varyingVecs[triangleIndex] = Vector3::Normalize(worldSpaceNormal.xyz);
        
        
        vsOutput->varying[0] = Vector3(ndotl,ndotl,ndotl);
        //vsOutput->varying[0] = Vector3(worldSpaceNormal.x,worldSpaceNormal.y,worldSpaceNormal.z);
        //FragmentVaringIns[0][triangleIndex] = normilzed;

        Vector4 tmp;
        tmp.x = RenderMesh->Vertices[bufferIndex].x;
        tmp.y = RenderMesh->Vertices[bufferIndex].y;
        tmp.z = RenderMesh->Vertices[bufferIndex].z;
        tmp.w = 1;
        return *PVMMatrix * tmp; // transform it to screen coordinates
    }

    virtual bool fragment(const Vector3& barycentric, Color &color) {

        Color mask = (Color)(TextureUnit1->Sample(FragmentInUV->x,FragmentInUV->y));
        if(mask.r < 0.1)
        {
            return true;
        }

        Vector3 diffuse = FragmentVaringOuts[0];
        Color albedo = (Color)(TextureUnit0->Sample(FragmentInUV->x,FragmentInUV->y));

        color.r = (0.2 + 0.8*diffuse.x) * albedo.r;
        color.g = (0.2 + 0.8*diffuse.y) * albedo.g;
        color.b = (0.2 + 0.8*diffuse.z) * albedo.b;


        color.a = 1;
        return false;                              // no, we do not discard this pixel
    }
};




class BlinnPhongShader : public IShader {

    public:
    BlinnPhongShader()
    {
        VaryingsCountBitMask |= FirstBitMask;
        VaryingsCountBitMask |= SecondBitMask;
    }

    Vector3 ObjectToWorldNormal(Matrix4x4& world2Object, Vector3& norm)
    {
        //return Vector3::Normalize(world2Object[0].xyz * norm.x + world2Object[1].xyz * norm.y + world2Object[2].xyz * norm.z);
        return Vector3::Normalize(Vector3(world2Object[0],world2Object[1],world2Object[2]) * norm.x 
                                + Vector3(world2Object[4],world2Object[5],world2Object[6])  * norm.y 
                                + Vector3(world2Object[8],world2Object[9],world2Object[10])  * norm.z);
    }


    virtual Vector4 vertex(int bufferIndex,int triangleIndex, VSOutput* vsOutput)  
    {

        //varying_intensity[index] = std::max(0.f,Vector3::Dot(Vector3::Normalize(RenderMesh->Normals[index]), *DirectionLightDir)); // get diffuse lighting intensity
        Vector3 normilzed = Vector3::Normalize(RenderMesh->Normals[bufferIndex]);
       // Vector4 worldNormal = *ModelViewMatrix * Vector4(normilzed, 0.0f);
        Vector3 worldNormal = ObjectToWorldNormal(*World2ObjectMatrix, normilzed);
        Vector4 worldPos = *ModelViewMatrix * Vector4(RenderMesh->Vertices[bufferIndex], 0.0f);
        Vector3 worldView = *WorldSpaceViewPos - worldPos.xyz;

        vsOutput->varying[0] = worldNormal;
      // FragmentVaringIns[0][triangleIndex] = normilzed;
        vsOutput->varying[1] = worldView;
        
        return *PVMMatrix * Vector4(RenderMesh->Vertices[bufferIndex], 1.0f); // transform it to screen coordinates
    }

    virtual bool fragment(const Vector3& barycentric, Color &color) {
        //float intensity = Vector3::Dot(varying_intensity, barycentric);   // interpolate intensity for the current pixel
        //Vector3 normalCol = barycentric.x * varyingVecs[0] +barycentric.y * varyingVecs[1] + barycentric.z * varyingVecs[2];

        Vector3 normalizedLightDir  = Vector3::Normalize(*DirectionLightDir);
        Vector3 normalizedNormal = Vector3::Normalize(FragmentVaringOuts[0]);
        Vector3 normalizedViewDir = FragmentVaringOuts[1].normalized();
        Vector3 halfDir = Vector3::Normalize(normalizedViewDir + normalizedLightDir);

        Color diffuse = Color::white;

        float ndotl = Mathf::Max(Vector3::Dot( Vector3::Normalize(FragmentVaringOuts[0]), Vector3::Normalize(*DirectionLightDir)), 0.0f);

        //计算高光
        Vector3 specular = std::pow(Mathf::Max(Vector3::Dot(halfDir,normalizedNormal), 0),8);

        Vector3 col = 0.5 * Vector3(1,1,1) * ndotl + specular; // well duh
        //Vector3 col =  specular; // well duh

        col.x = Mathf::Clamp(col.x, 0, 1);
        col.y = Mathf::Clamp(col.y, 0, 1);
        col.z = Mathf::Clamp(col.z, 0, 1);
       //Vector3 col = specular; // well duh

       // color.r = 0.5+ 0.5 * col.x;
        //color.g = 0.5+ 0.5 * col.y;
        //color.b = 0.5+ 0.5 * col.z;


        color.r = col.x;
        color.g = col.y;
        color.b = col.z;
        color.a = 1;

        return false;                              // no, we do not discard this pixel
    }
};