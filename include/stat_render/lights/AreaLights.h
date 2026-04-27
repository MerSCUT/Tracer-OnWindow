#pragma once

#include"stat_render/core/common.h"
#include"stat_render/shapes/Object.h"
#include"stat_render/lights/Light.h"
class AreaLight : public Light{
private:
    Color3f radiance;
    Object* obj;           // 发光的几何体
public:
    AreaLight(Object* objptr, Color3f radiance_) : obj(objptr), radiance(radiance_) 
    {}

    ~AreaLight() = default;
    
    Color3f getRadiance() const { return radiance; }

    Object* getObject() const override { return obj; }

    
    
    LightSample sampleLight(float u1, float u2) const override
    {
        LightSample ls;

        obj->sample(u1, u2, ls.position, ls.normal, ls.pdf);
        ls.radiance = radiance;
        ls.obj = obj;
        return ls;
    }

    
};