#pragma once

#include <vector>
#include "Math/Vector3.h"
#include "Math/Vector2.h"
#include "Math/Vector4.h"

class Mesh
{
    public:
        Mesh();
        ~Mesh();
        std::vector<Vector3> Vertices;
        std::vector<Vector2> UVs;
        std::vector<Vector3> Normals;
        std::vector<Vector4> Colors;
        std::vector<int> Triangles;

    private:

    
};