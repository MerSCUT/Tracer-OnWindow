#pragma once
#include"stat_render/core/common.h"

struct Ray{
    Point3f origin;
    Vec3f direct;
    Vec3f inv_direct;
    
    Ray(Point3f ori, Vec3f dir) : 
    origin(ori), 
    direct(dir) {
        float inv_x = std::fabs(dir.x) > 1e-9f ? 1.0f / dir.x : Inf;
        float inv_y = std::fabs(dir.y) > 1e-9f ? 1.0f / dir.y : Inf;
        float inv_z = std::fabs(dir.z) > 1e-9f ? 1.0f / dir.z : Inf;
        inv_direct = Vec3f(inv_x, inv_y, inv_z);
    }

    Point3f operator()(float t) const { return  t * direct + origin ; }
};
