#pragma once

#include"stat_render/shapes/Object.h"
#include"stat_render/core/transform.h"
#include<iostream>

class Triangle : public Object
{
private:
    Point3f v0, v1, v2;
    Vec3f n0, n1, n2;   //vertices normal
    Point2f uv0, uv1, uv2;
    Bound bound;
    Material* material;
public:
    // All zero by default
    Triangle() : 
    v0(Point3f(0.,0.,0.)),
    v1(v0),
    v2(v0),
    uv0(Point2f(0.,0.)),
    uv1(uv0),
    uv2(uv0),
    bound(Bound()), 
    material(nullptr) {
        Vec3f pmin = v0.cwiseMin(v1).cwiseMin(v2);
        Vec3f pmax = v0.cwiseMax(v1).cwiseMax(v2);

        bound = Bound(pmin, pmax); 
    }

    Triangle(Point3f v0_, Point3f v1_, Point3f v2_, Material* m) 
    : v0(v0_), v1(v1_), v2(v2_), material(m) {
        
        Vec3f pmin = v0.cwiseMin(v1).cwiseMin(v2);
        Vec3f pmax = v0.cwiseMax(v1).cwiseMax(v2);

        bound = Bound(pmin, pmax); 
    }

    

    Triangle(Point3f v0_, Point3f v1_, Point3f v2_,
    Vec3f n0_, Vec3f n1_, Vec3f n2_,
    Point2f uv0_, Point2f uv1_, Point2f uv2_, Material* m) :
    v0(v0_), v1(v1_), v2(v2_), 
    n0(n0_), n1(n1_), n2(n2_),
    uv0(uv0_), uv1(uv1_), uv2(uv2_),
    material(m) {
        Vec3f pmin = v0.cwiseMin(v1).cwiseMin(v2);
        Vec3f pmax = v0.cwiseMax(v1).cwiseMax(v2);

        bound = Bound(pmin, pmax); 
    }

    ~Triangle() = default;

    Bound getBound() const override {return bound;}
    Material* getMaterial() const override { return material; }
    

    inline void setVertex(const Point3f& _v0, const Point3f& _v1, const Point3f& _v2)
    {
        v0 = _v0 ; v1  = _v1; v2 = _v2;
    }
    inline void setMaterial(Material* m) { this->material = m; }
    
    Hit intersect(const Ray& ray) override;
    bool hit(const Ray& ray);

    inline float SurfaceArea() const override {  
        Vec3f edge1 = v1 - v0;
        Vec3f edge2 = v2 - v0;
        return 0.5f * cross(edge1, edge2).norm(); }

    inline Point3f getCenter() const override {  
        return (v0 + v1 + v2) * 1./3.; 
    }

    Hit getHit(const Ray& ray);
    
    void sample(float xi1, float xi2, Point3f& position, Vec3f& normal, float& pdf) const override;

    float getArea();
    void transform(const Mat4f& M) override;

};