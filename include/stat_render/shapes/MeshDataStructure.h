#pragma once
#include"stat_render/core/Vector.h"

struct Vertex {
    Vec3f position;
    Vec3f normal;
    Vec2f texcoord;
};

struct TriangleIndices {
    int v1, v2, v3;
};
