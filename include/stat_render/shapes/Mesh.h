#pragma once

#include"stat_render/shapes/Triangle.h"
#include"stat_render/accelerators/BVH.h"
#include<vector>
#include<memory>
class Mesh : public Object{
private:
    std::vector<Object*> triangles;
    Material* m;
    Bound bound;
    BVH* bvh = nullptr;
public:
    Mesh(std::vector<Object*> triangles_) : triangles(triangles_) {}

    Mesh(const std::vector<Vec3f>& vertices, 
         const std::vector<uint32_t>& indices, 
         Material* m);

    

    Hit intersect(const Ray& ray) override;
    Hit intersectWithoutBVH(const Ray& ray) ;
    float SurfaceArea() const override { return 1.0f; };

    Bound getBound() const override {return bound; }
    Material* getMaterial() const override { return m; }
    
    inline Point3f getCenter() const override { return getBound().Center(); }
    ~Mesh();

    void sample(float xi1, float xi2, Point3f& position, Vec3f& normal, float& pdf) const override;
    
    void transform(const Mat4f & Matrix) override
    {
        Bound boundtemp;
        for(auto& tri: triangles)
        {
            tri->transform(Matrix);
            boundtemp.Union(tri->getBound());
        }
        bound = boundtemp;
        this->bvh = new BVH(triangles);
        return;
    }
};