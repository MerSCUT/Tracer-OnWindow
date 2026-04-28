#include"stat_render/accelerators/BVH.h"
#include"stat_render/shapes/Triangle.h"
#include<cassert>


inline bool RayTriangleIntersect(const Ray& ray, const Vec3f& v0, const Vec3f& v1, const Vec3f& v2,
    float& t, float& u, float& v)
{
    Vec3f edge1 = v1 - v0;
    Vec3f edge2 = v2 - v0;
    Vec3f pvec = cross(ray.direct, edge2); // 注意：替换为你的叉乘函数
    float det = dot(edge1, pvec);             // 注意：替换为你的点乘函数

    // 如果 det 接近 0，说明射线与三角形平行
    if (std::abs(det) < 1e-8f) return false;

    float invDet = 1.0f / det;

    Vec3f tvec = ray.origin - v0;
    u = dot(tvec, pvec) * invDet;
    if (u < 0.0f || u > 1.0f) return false;

    Vec3f qvec = cross(tvec, edge1);
    v = dot(ray.direct, qvec) * invDet;
    if (v < 0.0f || u + v > 1.0f) return false;

    t = dot(edge2, qvec) * invDet;

    // t 必须大于 0 (交点在射线原点前方) 才算有效击中
    return t > 0.0f;
}

BVH::BVH(const std::vector<Vertex>& v, const std::vector<TriangleIndices>& idx, int B)
    : vertices(&v), indices(&idx)
{
    std::vector<BVHPrimitiveInfo> primitiveInfo;
    primitiveInfo.reserve(idx.size());

    Bound rootBound;
    for (uint32_t i = 0; i < idx.size(); ++i) {
        // 根据索引获取三个顶点
        const auto& tri = idx[i];
        const Vec3f& p1 = v[tri.v1].position;
        const Vec3f& p2 = v[tri.v2].position;
        const Vec3f& p3 = v[tri.v3].position;

        // 计算该三角形的包围盒
        Bound b;
        b.Union(p1); b.Union(p2); b.Union(p3);
        rootBound.Union(b);

        primitiveInfo.emplace_back(i, b);
    }

    root = Build(primitiveInfo, 0, primitiveInfo.size(), rootBound, B);
}

BVHnode* BVH::Build(std::vector<BVHPrimitiveInfo>& primitiveInfo,
    int start, int end, const Bound& bp, int B)
{
    BVHnode* node = new BVHnode();
    node->bound = bp;
    int nPrimitives = end - start;

    // 1. 叶子节点强制返回条件
    if (nPrimitives <= 2) {
        node->firstPrimOffset = orderedIndices.size();
        node->nPrimitives = nPrimitives;
        for (int i = start; i < end; ++i) {
            orderedIndices.push_back(primitiveInfo[i].primitiveIndex);
        }
        return node;
    }

    // 2. 计算该范围内所有图元【中心点】的包围盒
    // 这对于确定划分轴和桶的跨度至关重要
    Bound centroidBounds;
    for (int i = start; i < end; ++i) {
        centroidBounds.Union(primitiveInfo[i].centroid);
    }

    int ax = centroidBounds.MaxAxis(); // 沿着中心点分布最广的轴划分
    float l = centroidBounds.getPmin()[ax];
    float r = centroidBounds.getPmax()[ax];

    // 如果所有中心点都在同一点上，说明图元完全重合，无法进行空间划分
    if (l == r) {
        node->firstPrimOffset = orderedIndices.size();
        node->nPrimitives = nPrimitives;
        for (int i = start; i < end; ++i) {
            orderedIndices.push_back(primitiveInfo[i].primitiveIndex);
        }
        return node;
    }

    // 3. 初始化极其轻量的 SAH 桶 (抛弃 vector<Object*>)
    struct BucketInfo {
        int count = 0;
        Bound bounds;
    };
    std::vector<BucketInfo> buckets(B);

    // 4. 遍历当前范围内的图元，放入对应的桶中
    for (int i = start; i < end; ++i) {
        float cen = primitiveInfo[i].centroid[ax];

        // 计算桶索引
        int b = std::floor(B * ((cen - l) / (r - l)));
        b = std::clamp(b, 0, B - 1);

        buckets[b].count++;
        buckets[b].bounds.Union(primitiveInfo[i].bounds); // 注意：这里合并的是图元真实的包围盒
    }

    // 5. 扫描所有的划分平面 (1 到 B-1)，找出 SAH 代价最小的划分
    float minCost = std::numeric_limits<float>::max();
    int minSplitBucket = -1;
    Bound minLeftBound, minRightBound;

    // 预计算总表面积的倒数，避免在循环中重复除法
    float invTotalArea = 1.0f / (bp.getSurfaceArea() + 1e-6f);

    for (int k = 1; k < B; k++) {
        Bound leftBound, rightBound;
        int leftCount = 0, rightCount = 0;

        // 统计左侧 [0, k)
        for (int j = 0; j < k; j++) {
            leftCount += buckets[j].count;
            leftBound.Union(buckets[j].bounds);
        }
        // 统计右侧 [k, B)
        for (int j = k; j < B; j++) {
            rightCount += buckets[j].count;
            rightBound.Union(buckets[j].bounds);
        }

        if (leftCount == 0 || rightCount == 0) continue;

        // 计算当前划分的 SAH 代价
        // 公式: C = 0.125 + (Area_L * N_L + Area_R * N_R) / Area_Total
        float cost = 0.125f + (leftCount * leftBound.getSurfaceArea() + rightCount * rightBound.getSurfaceArea()) * invTotalArea;

        if (cost < minCost) {
            minCost = cost;
            minSplitBucket = k;
            minLeftBound = leftBound;
            minRightBound = rightBound;
        }
    }

    // 6. SAH 代价评估判断
    // 如果算出的最优划分成本比直接相交所有图元还要大，则放弃划分，作为叶子节点
    float leafCost = (float)nPrimitives;
    if (minSplitBucket == -1 || minCost > leafCost) {
        node->firstPrimOffset = orderedIndices.size();
        node->nPrimitives = nPrimitives;
        for (int i = start; i < end; ++i) {
            orderedIndices.push_back(primitiveInfo[i].primitiveIndex);
        }
        return node;
    }

    // 7. 使用 std::partition 进行高性能的数组原地划分
    auto midIter = std::partition(primitiveInfo.begin() + start, primitiveInfo.begin() + end,
        [=](const BVHPrimitiveInfo& pi) {
            float cen = pi.centroid[ax];
            int b = std::floor(B * ((cen - l) / (r - l)));
            b = std::clamp(b, 0, B - 1);
            return b < minSplitBucket; // 小于最优划分桶索引的，全部扔到左边
        });

    int mid = std::distance(primitiveInfo.begin(), midIter);

    // 防御性检查：以防浮点数精度等极端情况导致 partition 没能真正划分数据
    if (mid == start || mid == end) {
        node->firstPrimOffset = orderedIndices.size();
        node->nPrimitives = nPrimitives;
        for (int i = start; i < end; ++i) {
            orderedIndices.push_back(primitiveInfo[i].primitiveIndex);
        }
        return node;
    }

    // 8. 递归构建左右子树 (传入精细计算出的子包围盒 minLeftBound 和 minRightBound)
    node->left = Build(primitiveInfo, start, mid, minLeftBound, B);
    node->right = Build(primitiveInfo, mid, end, minRightBound, B);
    //======
    
    return node;
}

Hit BVH::intersectBLAS(const Ray& ray, BVHnode* node) const
{
    // Local Space
    Hit payload;
    // 1. 包围盒求交：如果射线没有击中包围盒，直接返回空 payload
    if (node == nullptr || !node->bound.intersect(ray)) return payload;

    // 2. 叶子节点判断 (依靠 nPrimitives > 0 比判断左右子树是否为空更严谨且快)
    if (node->nPrimitives > 0)
    {
        // 遍历该叶子节点包含的所有三角形
        for (int i = 0; i < node->nPrimitives; ++i)
        {
            // 通过 orderedIndices 找到真实的三角形在 EBO (indices) 中的索引
            uint32_t faceIndex = orderedIndices[node->firstPrimOffset + i];
            const auto& triIndices = (*indices)[faceIndex];

            // 通过 EBO 获取 VBO (vertices) 中的三个顶点坐标
            const Vec3f& v1 = (*vertices)[triIndices.v1].position;
            const Vec3f& v2 = (*vertices)[triIndices.v2].position;
            const Vec3f& v3 = (*vertices)[triIndices.v3].position;

            float t, u, v;
            // 执行底层 Möller-Trumbore 射线-三角形求交算法
            if (RayTriangleIntersect(ray, v1, v2, v3, t, u, v))
            {
                // 确保交点在合法范围内，且比当前记录的最近交点更近
                if (t > 0.0f && t < payload.tmin)
                {
                    payload.tmin = t;
                    assert(payload.tmin > 1e-7 && "Moller-Trumbore Algorithm");
                    payload.u = u;                 // 记录重心坐标 u
                    payload.v = v;                 // 记录重心坐标 v
                    
                    payload.position = (1.0f - u - v) * v1 + u * v2 + v * v3 ;
                    payload.normal = normalize(cross(v2 - v1, v3 - v2));
                    if (dot(payload.normal, ray.direct) > 0) {
                        payload.normal = -payload.normal;
                    }
                    payload.incident = ray.origin - payload.position;
                    payload.faceIndex = faceIndex; // 【关键核心】记录命中了具体哪个三角形！
                    payload.intersected = true;
                }
            }
        }
        return payload;
    }

    // 3. 内部节点递归
    Hit templ = intersectBLAS(ray, node->left);
    Hit tempr = intersectBLAS(ray, node->right);

    // 4. 结果合并 (你的原始逻辑非常清晰，保持不变)
    if (templ.intersected && tempr.intersected) {
        return (templ.tmin < tempr.tmin) ? templ : tempr;
    }
    else if (templ.intersected) {
        return templ;
    }
    else if (tempr.intersected) {
        return tempr;
    }

    return payload;
}

Hit BVH::intersectTLAS(const Ray& ray, BVHnode* node) const
{
    Hit payload;
    if (node == nullptr || !node->bound.intersect(ray)) return payload;

    // 叶子节点：遍历其包含的 Object 实例
    if (node->nPrimitives > 0)
    {
        for (int i = 0; i < node->nPrimitives; ++i)
        {
            // 获取该 Object 在 unique_objects 中的真实索引
            uint32_t objIndex = orderedIndices[node->firstPrimOffset + i];
            const auto& obj = (*scene_objects)[objIndex];

            // 多态调用：Mesh 内部会将射线转到局部空间，并调用自身的 BLAS
            Hit t = obj->intersect(ray);

            if (t.intersected && t.tmin < payload.tmin) {
                payload = t;
            }
        }
        return payload;
    }

    // 内部节点递归
    Hit templ = intersectTLAS(ray, node->left);
    Hit tempr = intersectTLAS(ray, node->right);

    if (templ.intersected && tempr.intersected) {
        return (templ.tmin < tempr.tmin) ? templ : tempr;
    }
    return templ.intersected ? templ : (tempr.intersected ? tempr : payload);
}