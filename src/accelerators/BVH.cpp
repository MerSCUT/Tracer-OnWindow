#include"stat_render/accelerators/BVH.h"
#include<cassert>

// Single-File Tool Functions
namespace {
    struct Bucket{
        std::vector<Object*> list;
        int num = 0;
        Bound box = Bound();
    };
    BVHnode* Build(const std::vector<Object*>& objects,  const Bound bp, const int B = 12)
    {
        if (objects.empty()) { return nullptr; }
        if (objects.size() <= 2)  { return new BVHnode(objects, bp); }
        
        std::vector<Bucket> bucket(B);
        // bp 是 objects 的包围盒.
        int ax = bp.MaxAxis();
        float l = bp.getPmin()[ax], r = bp.getPmax()[ax];
        for(auto& obj : objects)
        {
            auto cen = obj->getCenter()[ax];
            int k = std::floor(B * ((cen - l) / (r-l + Epsilon)));
            k = std::clamp(k, 0, B-1);
            

            bucket[k].box.Union(obj->getBound());
            bucket[k].num++;
            bucket[k].list.push_back(obj);
        }
        float C = std::numeric_limits<float>::max();
        int split = 0;
        float temp = 1.0f / (bp.getSurfaceArea() + Epsilon);
        
        Bound Bl_res, Br_res;
        float Ck;
        for(int k = 1; k < B; k++)
        {
            Bound Bl, Br;
            int Nl = 0, Nr = 0;
            
            for(int j = 0; j < k; j++)
            {
                Nl += bucket[j].num ;
                Bl.Union(bucket[j].box);
            }
            for(int j = k; j < B; j++)
            {
                Nr += bucket[j].num;
                Br.Union(bucket[j].box);
            }
            
            // 1. Nl == 0 或 Nr == 0 则分类无意义
            if (Nl == 0 || Nr == 0) continue;
            // 2. 分类有意义, 但子类包围盒表面积为0 (退化为平面区域)
            float SL = Nl == 0 ? 0 : Bl.getSurfaceArea();
            float SR = Nr == 0 ? 0 : Br.getSurfaceArea();
            Ck = 0.125f + temp * (Nl * SL + Nr * SR );
            

            if (Ck < C) { 
                C = Ck; 
                split = k; 
                Bl_res = Bl; 
                Br_res = Br; 
                continue;
            }
            
        }
        if (split == 0)
        {
            return new BVHnode(objects, bp);
        }

        // 分割
        std::vector<Object*> left, right;
        for(int j = 0; j < split; j++)
        {
            left.insert(left.end(), bucket[j].list.begin(), bucket[j].list.end());
        }
        for(int j = split; j < B; j++)
        {
            right.insert(right.end(), bucket[j].list.begin(), bucket[j].list.end());
        }
        
        if (left.size() == 0 || right.size() == 0)
        {
            return new BVHnode(objects, bp);
        }

        BVHnode* node = new BVHnode;
        node->bound = bp;
        
        node->left = Build(left, Bl_res, B);
        node->right = Build(right, Br_res, B);
        return node;
    }
    
    Hit intersectBVH(const Ray& ray, BVHnode* node)
    {
        Hit payload;
        if (node == nullptr || !node->bound.intersect(ray)) return payload;
        
        if (!node->left && !node->right) 
        {
            for(auto& obj : node->objects)
            {
                
                assert(obj != nullptr);
                if (Hit t = obj->intersect(ray); t.intersected && t.tmin < payload.tmin)
                payload = t;
            }
            return payload;
        }
        Hit templ = intersectBVH(ray, node->left);
        Hit tempr = intersectBVH(ray, node->right);
        
        // 1. 两边都击中了，返回距离近的
        if (templ.intersected && tempr.intersected) {
            return (templ.tmin < tempr.tmin) ? templ : tempr;
        }
        // 2. 只有左边击中，直接返回左边
        else if (templ.intersected) {
            return templ;
        }
        // 3. 只有右边击中，直接返回右边
        else if (tempr.intersected) {
            return tempr;
        }
        return payload;
    }
}

BVH::BVH(const std::vector<Object*>& objects, const int B) : head(nullptr)
{
    Bound b;
    // 求总包围盒
    for(auto& obj : objects)
    {
        b.Union(obj->getBound());
    }
    head = Build(objects, b, B);
}

BVH::~BVH()
{
    Clear(head);
}
void BVH::Clear(BVHnode* node)
{
    if (node == nullptr) return;
    Clear(node->left); 
    node->left = nullptr;

    Clear(node->right);
    node->right = nullptr;

    delete node;
    node = nullptr;
    return;
}


Hit BVH::intersect(const Ray& ray)
{
    // 递归
    return intersectBVH(ray, head);
}

void BVH::Dense(){
    // 将 head 中的结构转储到 std::vector<> 中
}