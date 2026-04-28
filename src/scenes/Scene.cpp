#include"stat_render/scenes/Scene.h"
#include"stat_render/accelerators/BVH.h"
#include<iostream>


//void Scene::BuildTLAS() {
//	// 构建 TLAS
//	std::vector<Object*> obj_ptrs;
//	for (auto& obj : unique_objects) {
//		obj_ptrs.push_back(obj.get());
//	}
//	TLAS = std::make_unique<BVH>(obj_ptrs, 12);
//}

void Scene::BuildTLAS() {
    if (unique_objects.empty()) return;

    // 1. 收集所有顶层实例（Object）的包围盒信息，构建代理数组
    std::vector<BVHPrimitiveInfo> primitiveInfo;
    primitiveInfo.reserve(unique_objects.size());

    sbound = Bound(); // 重置并计算场景全局包围盒

    for (uint32_t i = 0; i < unique_objects.size(); ++i) {
        // 【关键】必须获取物体变换到世界空间 (World Space) 后的包围盒！
        // 如果物体有 ObjectToWorld 矩阵，需要计算变换后的 AABB
        Bound objWorldBound = unique_objects[i]->getWorldBound();

        primitiveInfo.emplace_back(i, objWorldBound);
        sbound.Union(objWorldBound);
    }

    // 2. 调用专门针对 Object 实例重载的 BVH 构造函数
    TLAS = std::make_unique<BVH>(primitiveInfo, sbound, 12);
    TLAS->BindSceneObjects(unique_objects); // 绑定指针，用于遍历求交
}


Hit Scene::intersect(const Ray& world_ray) const
{
    if (!TLAS) return Hit();
    return TLAS->intersectTLAS(world_ray, TLAS->root);
}



LightSample Scene::sampleLight(SobolSampler& sampler) const
{
    LightSample ls;
    // 假设场景中只有一个光源
    assert(unique_lights.size() == 1);
    Vec2f u = sampler.get2D();
    auto l = unique_lights[0].get();
    ls = l->sampleLight(u.x, u.y);
    ls.pdf = std::max(ls.pdf, 1e-5f);
    return ls;
    
    // ... 多个光源 :
}

