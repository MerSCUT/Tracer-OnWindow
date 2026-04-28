#include"stat_render/shapes/Mesh.h"
#include"stat_render/accelerators/BVH.h"

void MeshGeometry::BuildBLAS() {
	BLAS = std::make_unique<BVH>(vertices, indices);
}


Mesh::Mesh(const std::vector<Vec3f>& vertices, 
         const std::vector<uint32_t>& indices, 
         Material* m) 
{
    // 1. 根据索引数据创建 Triangle 实例
    Bound b;
    for (size_t i = 0; i < indices.size(); i += 3) {
        auto v1 = vertices[indices[i]];
        auto v2 = vertices[indices[i + 1]];
        auto v3 = vertices[indices[i + 2]];
        v1 = (ObjectToWorld * Vec4f(v1, 1.0f)).xyz();
        v2 = (ObjectToWorld * Vec4f(v2, 1.0f)).xyz();
        v3 = (ObjectToWorld * Vec4f(v3, 1.0f)).xyz();
        
        
        
        //triangles.push_back(t);
        b.Union(v1);
        b.Union(v2);
        b.Union(v3);
    }
    
    this->world_bound;
    
}

Mesh::Mesh(MeshGeometry meshGeo, const std::shared_ptr<Material> material, const Transform& trans) {
    // one-submesh by default
    ObjectToWorld = trans;
    Bound b;
    for (auto& triInd: meshGeo.indices) {
        auto v1 = meshGeo.vertices[triInd.v1];
        auto v2 = meshGeo.vertices[triInd.v2];
        auto v3 = meshGeo.vertices[triInd.v3];
        v1.position = (trans * Vec4f(v1.position, 1.0f)).xyz();
        v2.position = (trans * Vec4f(v2.position, 1.0f)).xyz();
        v3.position = (trans * Vec4f(v3.position, 1.0f)).xyz();

        b.Union(v1.position);
        b.Union(v2.position);
        b.Union(v3.position);

    }
    meshGeo.mat = material;
    
    world_bound = b;
    meshGeometries.push_back(std::move(meshGeo));
    meshGeometries[0].BuildBLAS();
	assert(meshGeometries.size() == 1); // 当前实现仅支持单一 MeshGeometry，后续可扩展支持多个
}





// Without BVH acceleration
//Hit Mesh::intersectWithoutBVH(const Ray& ray)
//{
//    Hit payload;
//    /*for(auto& tri : triangles)
//    {
//        if (Hit temp = tri->intersect(ray); temp.intersected && temp.tmin < payload.tmin)
//        payload = temp;
//    }*/
//	auto indices = meshGeometries[0].indices;
//    auto vertices = meshGeometries[0].vertices;
//    for (auto& index : indices) {
//		Triangle tri(vertices[index.v1].position, vertices[index.v2].position, vertices[index.v3].position, nullptr);
//		if (Hit temp = tri.intersect(ray); temp.intersected && temp.tmin < payload.tmin)
//			payload = temp;
//    }
//
//    return payload;
//}

Hit Mesh::intersect(const Ray& ray)
{
    Hit payload;
	Ray localRay(Vec3f(ObjectToWorld.mInv * Vec4f(ray.origin, 1.0f)), Vec3f(ObjectToWorld.mInv * Vec4f(ray.direct, 0.0f)));
    payload = meshGeometries[0].BLAS->intersectBVH(localRay);
    payload.obj = this;
    payload.material = meshGeometries[0].mat.get();
    auto old_position = payload.position;
    if (payload.intersected) {
        payload.position = (ObjectToWorld.m * Vec4f(payload.position, 1.0f)).xyz();
        payload.normal = (ObjectToWorld.m * Vec4f(payload.normal, 0.0f)).xyz().normalized();
        payload.incident = ray.origin - payload.position;
        //assert(payload.incident.norm2() > 0.f && "Mesh::Intersect Failed");
    }
    
    return payload;
}

Mesh::~Mesh()
{
    
}

struct TriangleProxy {
    const Vec3f& v0;
    const Vec3f& v1;
    const Vec3f& v2;

    TriangleProxy(const Vec3f& p0, const Vec3f& p1, const Vec3f& p2)
        : v0(p0), v1(p1), v2(p2) {
    }

    // 计算局部空间下的表面积
    float SurfaceArea() const {
        return cross(v1 - v0, v2 - v0).norm() * 0.5f;
    }

    // 在局部空间下进行均匀采样
    void sample(float xi1, float xi2, Point3f& local_pos, Vec3f& local_normal) const {
        float sqrt_xi1 = std::sqrt(xi1);
        float b0 = 1.0f - sqrt_xi1;
        float b1 = xi2 * sqrt_xi1;
        float b2 = 1.0f - b0 - b1;

        local_pos = v0 * b0 + v1 * b1 + v2 * b2;
        local_normal = normalize(cross(v1 - v0, v2 - v0));
    }
};

void Mesh::sample(float xi1, float xi2, Point3f& position, Vec3f& normal, float& pdf) const
{
    float sum_S = 0.0f;

    // 第一次遍历：计算模型局部总面积
    for (const auto& geom : meshGeometries) {
        for (const auto& tri : geom.indices) {
            TriangleProxy proxy(
                geom.vertices[tri.v1].position,
                geom.vertices[tri.v2].position,
                geom.vertices[tri.v3].position
            );
            sum_S += proxy.SurfaceArea();
        }
    }

    Sampler sampler;
    float u = sampler.get1D();
    float sum = 0.0f;

    // 第二次遍历：寻找被采样的三角形
    for (const auto& geom : meshGeometries) {
        for (const auto& tri : geom.indices) {
            TriangleProxy proxy(
                geom.vertices[tri.v1].position,
                geom.vertices[tri.v2].position,
                geom.vertices[tri.v3].position
            );

            sum += proxy.SurfaceArea();
            if (sum / sum_S >= u) {
                Point3f local_pos;
                Vec3f local_normal;

                // 1. 获取局部坐标系的采样点和法线
                proxy.sample(xi1, xi2, local_pos, local_normal);

                // 2. 将局部坐标转换到世界坐标
                position = (ObjectToWorld * Vec4f(local_pos, 1.0f)).xyz();
                normal = normalize(ObjectToWorld.ApplyInverseTranspose(Vec4f(local_normal, 0.f)).xyz());


                // 3. 计算 PDF
                Vec3f local_x_axis(1.0f, 0.0f, 0.0f);

                // 注意这里是变换 Vector，不是 Point，忽略平移分量
                Vec3f world_x_axis = (ObjectToWorld * Vec4f(local_x_axis, 0.0f)).xyz();

                // 计算长度的平方即为 s^2 (假设你的向量库有 length_squared 或 dot 乘)
                float scale_squared = world_x_axis.norm2();

                // 世界空间的总面积 = 局部总面积 * s^2
                // PDF = 1.0 / 世界空间总面积
                pdf = 1.0f / (sum_S * scale_squared);
                return;
            }
        }
    }
}