#include"stat_render/shapes/Mesh.h"

Mesh::Mesh(const std::vector<Vec3f>& vertices, 
         const std::vector<uint32_t>& indices, 
         Material* m) 
{
    // 1. 根据索引数据创建 Triangle 实例
    Bound b;
    for (size_t i = 0; i < indices.size(); i += 3) {
        auto t = new Triangle(
            vertices[indices[i]], 
            vertices[indices[i+1]], 
            vertices[indices[i+2]], 
            m
        );
        triangles.push_back(t);
        b.Union(t->getBound());
    }
    
    this->bound = b;
    
}



// Without BVH acceleration
Hit Mesh::intersectWithoutBVH(const Ray& ray)
{
    Hit payload;
    for(auto& tri : triangles)
    {
        if (Hit temp = tri->intersect(ray); temp.intersected && temp.tmin < payload.tmin)
        payload = temp;
    }
    return payload;
}

Hit Mesh::intersect(const Ray& ray)
{
    //return intersectWithoutBVH(ray);
    return bvh->intersect(ray);
}

Mesh::~Mesh()
{
    // triangles 里的 std::vector<Object*> 要全部删除.
    for(auto& obj : triangles)
    {
        delete obj;
    }
}

void Mesh::sample(float xi1, float xi2, Point3f& position, Vec3f& normal, float& pdf) const
{
    float sum_S = 0.0f;
    for(auto& tri: triangles)
    {
        sum_S += tri->SurfaceArea();
    }
    Sampler sampler; 
    // 次选方案 : 不动架构, 用流式随机数生成.
    float u = sampler.get1D();
    float sum = 0.0f;
    for(auto& tri: triangles)
    {
        sum += tri->SurfaceArea();
        if (sum / sum_S > u) 
        {
            tri->sample(xi1, xi2, position, normal, pdf);
            pdf = 1.0f / sum_S;
            break;
        }
    }
    return;
}