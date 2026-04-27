#pragma once
#include<stat_render/core/common.h>
#include"stat_render/materials/Material.h"

// Intersection pakcages between Ray and Object / Bounding Box
class Object;

struct Hit
{
    bool intersected;
    Point3f position;
    Vec3f incident;      // outwards
    Vec3f normal;
    Material* material;
    float tmin;
    float dist_square;
    Object* obj;

    Hit() : 
    intersected(false), 
    position(Point3f(0.,0.,0.)), 
    incident(Vec3f(0.,0.,0.)), 
    normal(Vec3f(0.,0.,0.)), 
    material(nullptr), 
    tmin(std::numeric_limits<float>::max()) {}

};
