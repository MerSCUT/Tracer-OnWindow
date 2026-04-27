#pragma once

#include"stat_render/core/common.h"
#include"stat_render/shapes/Object.h"

struct LightSample
{
    Point3f position;
    Vec3f normal;
    Color3f radiance;
    float pdf;
    Object* obj;
};

class Light{
private:
     
public:
    virtual ~Light() = default;

    // u1, u2 是随机数
    virtual LightSample sampleLight(float u1, float u2) const = 0;
    virtual Object* getObject() const { return nullptr; }
};