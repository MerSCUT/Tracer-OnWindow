#pragma once
#include"stat_render/shapes/Triangle.h"
#include"stat_render/core/transform.h"
#include"stat_render/shapes/MeshDataStructure.h"
#include"stat_render/accelerators/BVH.h"
#include<vector>
#include<memory>


struct MeshGeometry {
    std::vector<Vertex> vertices;
    std::vector<TriangleIndices> indices;
    std::unique_ptr<BVH> BLAS;
    std::shared_ptr<Material> mat;
    Bound localBound;

    // 显式声明默认的构造和析构
    MeshGeometry() = default;
    ~MeshGeometry() = default;

    
    MeshGeometry(MeshGeometry&&) noexcept = default;
    MeshGeometry& operator=(MeshGeometry&&) noexcept = default;

    MeshGeometry(const MeshGeometry&) = delete;
    MeshGeometry& operator=(const MeshGeometry&) = delete;

    void BuildBLAS();
};

// Instance
class Mesh : public Object{
public:
    // new member
	std::vector<MeshGeometry> meshGeometries; // 支持多个 MeshGeometry 实例，便于处理复杂模型或多材质模型
    
    Transform ObjectToWorld; // Model Transform
    Bound world_bound;
private:
    // old member
    /*std::vector<Object*> triangles;*/
    //Material* m;
public:
    //Mesh(std::vector<Object*> triangles_) : triangles(triangles_) {}

    Mesh(const std::vector<Vec3f>& vertices, 
         const std::vector<uint32_t>& indices, 
         Material* m);

    Mesh(MeshGeometry meshGeo, const std::shared_ptr<Material> material, const Transform& trans = Transform());
    
    //Mesh(std::vector<std::unique_ptr<Mesh>> meshes);

    Hit intersect(const Ray& ray) override;

    Hit intersectWithoutBVH(const Ray& ray) ;

    float SurfaceArea() const override { return 1.0f; };

    Bound getBound() const override { return world_bound; }

    Bound getWorldBound() const override { return world_bound; }

    Material* getMaterial() const override { return meshGeometries[0].mat.get(); }
    
    inline Point3f getCenter() const override { return getBound().Center(); }

    ~Mesh();

    void sample(float xi1, float xi2, Point3f& position, Vec3f& normal, float& pdf) const override;
    
    
};