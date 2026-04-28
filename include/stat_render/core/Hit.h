#pragma once
#include<stat_render/core/common.h>
#include"stat_render/materials/Material.h"

// Intersection pakcages between Ray and Object / Bounding Box
class Object;

struct Hit
{
    // ==========================================
    // 1. 射线求交状态 (由 BVH 核心算法填充)
    // ==========================================
    bool intersected;
    float tmin;         // 射线原点到交点的距离 t
    float dist_square;

    // ==========================================
    // 2. 底层几何数据 (由 BVH 的叶子节点填充)
    // ==========================================
    uint32_t faceIndex; // 命中的三角形在 indices 数组中的索引！(核心)
    float u;            // 重心坐标 u
    float v;            // 重心坐标 v

    // ==========================================
    // 3. 顶层着色数据 (由 MeshInstance 在求交成功后进行插值和变换填充)
    // ==========================================
    Point3f position;   // 世界空间下的交点精确坐标
    Vec3f normal;       // 世界空间下的平滑法线 (插值并经过逆转置矩阵变换)
    Vec2f texcoord;     // 插值后的纹理坐标 UV (用于纹理映射)
    Vec3f incident;     // 出射方向 (通常是 -ray.direction，指向摄像机或上一条光线)

    // ==========================================
    // 4. 材质与所属实例 (由 MeshInstance 填充)
    // ==========================================
    Material* material; // 该交点绑定的材质
    const Object* obj;  // 指向被击中的顶层实例对象 (如具体的某一只 Bunny)

    // 构造函数：提供安全的默认值
    Hit() :
        intersected(false),
        tmin(std::numeric_limits<float>::max()),
        faceIndex(0), u(0.f), v(0.f),
        position(Point3f(0.f, 0.f, 0.f)),
        incident(Vec3f(0.f, 0.f, 0.f)),
        normal(Vec3f(0.f, 0.f, 0.f)),
        texcoord(Vec2f(0.f, 0.f)),
        material(nullptr),
        obj(nullptr),
        dist_square(0.0f)
    {
    }
};
