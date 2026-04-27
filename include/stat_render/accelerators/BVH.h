#pragma once

#include"stat_render/core/common.h"
#include"stat_render/accelerators/Bound.h"
#include"stat_render/shapes/Object.h"
#include<vector>
struct BVHnode
{
    Bound bound;
    BVHnode* left, *right;
    // std::vector<Object*>
    std::vector<Object*> objects;

    BVHnode() : left(nullptr),right(nullptr) {}

    BVHnode(const std::vector<Object*>& objects_, const Bound& bound_) :
    bound(bound_), 
    objects(objects_),
    left(nullptr), right(nullptr) {}
};

class BVH 
{
public:
    BVH() : head(nullptr) {}
    BVH(const std::vector<Object*>& objects, int B = 12);  // 建树
    ~BVH();                 // 销毁
    void Clear(BVHnode* node);
    Hit intersect(const Ray& ray);      // 加速求交 (递归)
    void Dense();
private:
    BVHnode* head;
    int B = 12;
    std::vector<BVHnode> array;
};