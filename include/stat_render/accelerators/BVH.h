#pragma once

#include"stat_render/core/common.h"
#include"stat_render/accelerators/Bound.h"
#include"stat_render/shapes/Object.h"
#include"stat_render/shapes/MeshDataStructure.h"
#include<vector>

struct BVHnode
{
    Bound bound;
    BVHnode* left, *right;
    // std::vector<Object*>
    //std::vector<Object*> objects;
    uint32_t firstPrimOffset = 0; // 在 orderedIndices 数组中的起始位置
    uint32_t nPrimitives = 0;     // 该叶子节点包含的三角形数量

    BVHnode() : left(nullptr),right(nullptr) {}

    bool isLeaf() const { return left == nullptr && right == nullptr; }

    /*BVHnode(const std::vector<Object*>& objects_, const Bound& bound_) :
    bound(bound_), 
    objects(objects_),
    left(nullptr), right(nullptr) {}*/
};

struct BVHPrimitiveInfo {
    uint32_t primitiveIndex; // 在 EBO (indices数组) 中的面索引
    Bound bounds;            // 该三角形的精确包围盒
    Vec3f centroid;          // 该包围盒的中心点 (用于 SAH 划分)

    BVHPrimitiveInfo(uint32_t idx, const Bound& b)
        : primitiveIndex(idx), bounds(b), centroid(b.Center()) {
    }
};

//class BVH_ 
//{
//public:
//    BVH() : head(nullptr) {}
//    BVH(const std::vector<Object*>& objects, int B = 12);  // 建树
//
//	BVH(const std::vector<Vertex>& vertices, const std::vector<TriangleIndices>& indices, int B = 12);  // 建树
//    ~BVH();                 // 销毁
//    void Clear(BVHnode* node);
//    Hit intersect(const Ray& ray);      // 加速求交 (递归)
//    void Dense();
//private:
//    BVHnode* head;
//    int B = 12;
//    std::vector<BVHnode> array;
//};

class BVH {
public:
//==========================================================
    std::vector<uint32_t> orderedIndices; // 重排后的三角形索引
    BVHnode* root = nullptr;

    // 保存 Mesh 的引用或指针，以便求交时获取顶点
    const std::vector<Vertex>* vertices;
    const std::vector<TriangleIndices>* indices;

    // TLAS 求交 : 
    const std::vector<std::unique_ptr<Object>>* scene_objects = nullptr;
//==========================================================
    // For BLAS
    BVH::BVH(const std::vector<Vertex>& v, const std::vector<TriangleIndices>& idx, int B = 12);
    
    Hit intersectBVH(const Ray& ray) const {
        return intersectBLAS(ray, root);
    }
    

    // 新增构造函数：专为 TLAS 设计
    BVH::BVH(std::vector<BVHPrimitiveInfo>& primitiveInfo, const Bound& sbound, int B)
    {
        // 复用之前写好的高性能 SAH Build 函数
        root = Build(primitiveInfo, 0, primitiveInfo.size(), sbound, B);
    }

    // 提供一个绑定接口，让 BVH 知道去哪里找真实的 Object
    void BVH::BindSceneObjects(const std::vector<std::unique_ptr<Object>>& objs) {
        scene_objects = &objs;
    }


    BVHnode* Build(std::vector<BVHPrimitiveInfo>& primitiveInfo,
        int start, int end, const Bound& bp, int B);
    Hit intersectBLAS(const Ray& ray, BVHnode* node) const;
    Hit intersectTLAS(const Ray& ray, BVHnode* node) const;

};