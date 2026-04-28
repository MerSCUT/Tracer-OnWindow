#pragma once

#include"stat_render/core/common.h"
#include"stat_render/shapes/Object.h"
#include"stat_render/shapes/Mesh.h"
#include<memory>

struct LightSample
{
    Point3f position;
    Vec3f normal;
    Color3f radiance;
    float pdf;
    Object* obj;
};

class AreaLight {
public:
    Color3f radiance;
    Mesh* obj;           // 发光的几何体

    AreaLight(Mesh* objptr, Color3f radiance_) : 
        obj(objptr), radiance(radiance_) 
    {}

    ~AreaLight() = default;
    
    Color3f getRadiance() const { return radiance; }

    Object* getObject() const  { return obj; }

    
    
    LightSample sampleLight(float u1, float u2) const 
    {
        LightSample ls;

        obj->sample(u1, u2, ls.position, ls.normal, ls.pdf);
        ls.radiance = radiance;
        ls.obj = obj;
        return ls;
    }
};