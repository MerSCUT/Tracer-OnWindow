#pragma once
#include"stat_render/core/Ray.h"
#include"stat_render/core/Hit.h"
#include"stat_render/materials/Material.h"
#include"stat_render/accelerators/Bound.h"
// 物体抽象基类

class Object{
public:
    // API
    
    // 1. Ojbect x Ray
    virtual Hit intersect(const Ray& ray) = 0;
    // 2. Bounding Box
    virtual Bound getBound() const = 0;
    virtual Material* getMaterial() const = 0;
    
    Object() {}
    
    virtual ~Object() = default;

    virtual float SurfaceArea() const = 0;
    virtual Point3f getCenter() const = 0;

    virtual void sample(float xi1, float xi2, Point3f& position, Vec3f& normal, float& pdf) const { return; }

    virtual void transform(const Mat4f& Matrix) { return; }
};