#pragma once

#include"stat_render/core/common.h"
#include"stat_render/core/Ray.h"
#include"stat_render/core/Matrix.h"
#include<limits>
#include<iostream>
class Bound{
private:
    Point3f Pmin, Pmax;
public:
    Bound() : 
    Pmin(std::numeric_limits<float>::max(),std::numeric_limits<float>::max(),std::numeric_limits<float>::max() ), 
    Pmax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max()) {}

    Bound(const Point3f& pmin_, const Point3f & pmax_) : Pmin(pmin_), Pmax(pmax_) {
        Vec3f epvec(Epsilon, Epsilon, Epsilon);
        Pmin -= epvec;
        Pmax += epvec;

    } 


    ~Bound() = default;
    // 包围盒合并函数
    void Union(const Bound& other);

    Point3f Center() const { return (Pmin + Pmax) * 0.5f; }

    Point3f getPmin() const {return Pmin; }
    Point3f getPmax() const {return Pmax; }
    
    inline int MaxAxis() const {
        auto d = Pmax-Pmin;
        if (d.x > d.y && d.x > d.z) return 0;
        if (d.y > d.x && d.y > d.z) return 1;
        if (d.z > d.x && d.z > d.y) return 2;
        return 0;
    }

    float MaxExtent() const { 
        auto d = Pmax-Pmin;
        
        return std::max(d.x, std::max(d.y, d.z)); 
    }

    float getSurfaceArea() const { 
        auto d = Pmax-Pmin;
        
        return 2 * (d.x * d.y + d.y * d.z + d.x * d.z );
    }

    bool intersect(const Ray& ray);

    Mat4f getNormalizeMatrix()
    {
        // 不变型
        auto center = Center();
        auto max_extent = MaxExtent();
        float scale = (max_extent > 1e-8f) ? (2.0f / max_extent) : 1.0f;
        // 先平移
        Mat4f M(
            scale, 0.0f,  0.0f,  -scale * center.x,
            0.0f,  scale, 0.0f,  -scale * center.y,
            0.0f,  0.0f,  scale, -scale * center.z,
            0.0f,  0.0f,  0.0f,  1.0f
        );
        return M;
    }
};