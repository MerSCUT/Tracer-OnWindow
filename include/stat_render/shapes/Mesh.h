#pragma once

#include"stat_render/shapes/Triangle.h"
#include"stat_render/accelerators/BVH.h"
#include"stat_render/core/transform.h"
#include<vector>
#include<memory>

struct Vertex {
    Vec3f position;
    Vec3f normal;
    Vec2f texcoord;
};

struct TriangleIndices {
    int v1, v2, v3;
};

struct MeshGeometry {
    // Datas of Mesh
    std::vector<Vertex> vertices;
    std::vector<TriangleIndices> indices;
};

class Mesh : public Object{
public:
    // new member
    std::vector<Vertex> vertices;
    std::vector<TriangleIndices> indices;
    std::shared_ptr<Material> mat;
    Transform ObjectToWorld; // Model Transform
private:
    // old member
    std::vector<Object*> triangles;
    Material* m;
   
    Bound bound;
    BVH* bvh = nullptr;
public:
    Mesh(std::vector<Object*> triangles_) : triangles(triangles_) {}

    Mesh(const std::vector<Vec3f>& vertices, 
         const std::vector<uint32_t>& indices, 
         Material* m);

    Mesh(const MeshGeometry& meshGeo, const std::shared_ptr<Material> material, const Transform& trans) {
        vertices = meshGeo.vertices;
        indices = meshGeo.indices;
        mat = material;
        ObjectToWorld = trans;
    }
    

    Hit intersect(const Ray& ray) override;
    Hit intersectWithoutBVH(const Ray& ray) ;
    float SurfaceArea() const override { return 1.0f; };

    Bound getBound() const override { return bound; }
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