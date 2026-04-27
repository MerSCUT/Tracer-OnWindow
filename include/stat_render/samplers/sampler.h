#pragma once

#include"stat_render/core/common.h"
#include<random>
// 随机数序列生成
class Sampler{
private:
    std::random_device rd;
    unsigned int seed = rd();
    std::mt19937 rng;
    std::uniform_real_distribution<> dist;


public:
    Sampler() : seed(rd()), rng(seed), dist(0.0f, 1.0f) {}

    ~Sampler() = default;
    
    // 均匀采样
    float get1D() { return dist(rng); }
    Vec2f get2D() { return Vec2f(dist(rng), dist(rng)); }
};

