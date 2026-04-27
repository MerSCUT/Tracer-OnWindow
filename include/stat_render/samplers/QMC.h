#pragma once

#include"stat_render/core/common.h"
#include<random>
#include<stat_render/samplers/sobolMatrix.h>
#include<iostream>
class _SobolSampler
{
private:
    uint64_t sampleIndex = 0;
    uint32_t dimension = 1;
    uint32_t pixelSeed = 0;
public:
    _SobolSampler() = default;
    _SobolSampler(uint64_t index) : sampleIndex(index), dimension(0) {}
    _SobolSampler(uint64_t index, int pixelX, int pixelY) : sampleIndex(index), dimension(0) {
        pixelSeed = HashPixel2D(pixelX, pixelY);
    } 
    inline uint32_t HashPixel2D(int x, int y) {
        // 阶段 1：状态打包 (State Packing)
        // 将两个 16 位整数无损合并为一个 32 位整数
        uint32_t s = (x & 0xFFFF) | ((y & 0xFFFF) << 16);

        // 阶段 2：混淆与雪崩 (MurmurHash3 Finalizer)
        s ^= s >> 16;
        s *= 0x85ebca6b;
        s ^= s >> 13;
        s *= 0xc2b2ae35;
        s ^= s >> 16;
        
        return s;
    }
    // 新增一个混合 Hash 函数
    inline uint32_t HashDimension(uint32_t seed, uint32_t dim) {
        uint32_t s = seed ^ dim;
        // 经典的 MurmurHash3 混淆
        s ^= s >> 16;
        s *= 0x85ebca6b;
        s ^= s >> 13;
        s *= 0xc2b2ae35;
        s ^= s >> 16;
        return s;
    }

    float get1D(){
        return SobolSample(sampleIndex, dimension++);
    }
    Vec2f get2D(){
        float u = get1D();
        float v = get1D();
        return Vec2f(u, v);
    }

    
    inline uint32_t SobolSample(uint64_t sampleIndex, uint32_t dimension) {
        uint32_t result = 0;
        // 计算当前维度在方向数矩阵中的起始偏移量
        uint32_t matrixOffset = dimension * 52; 

        // 遍历 sampleIndex 的二进制位
        // 当 index 被右移到 0 时循环提前结束，这是 O(w) 的复杂度，w 是非零位的数量
        for (uint32_t c = 0; sampleIndex != 0; sampleIndex >>= 1, ++c) {
            if (c >= 52) break;
            // 如果当前最低位是 1，则异或对应的方向数
            if (sampleIndex & 1) {
                result ^= SobolMatrices32[matrixOffset + c];
            }
        }
        result ^= HashDimension(pixelSeed, dimension);
        float res = static_cast<float>(result) * 0x1p-32f;
        // 截断, 防止采样到 1.0f
        std::cout << result << std::endl;
        return std::min(res, 0.99999994f);
    }
};


class SobolSampler
{
private:
    uint64_t sampleIndex = 0;
    uint32_t dimension = 0; // 修复：初始化为0，因为 get1D 是后置递增
    uint32_t pixelSeedBase = 0; // 改名，表示基础种子

public:
    SobolSampler() = default;
    SobolSampler(uint64_t index) : sampleIndex(index), dimension(0) {}
    SobolSampler(uint64_t index, int pixelX, int pixelY) : sampleIndex(index), dimension(0) {
        // 将 X 和 Y 绑定为该像素的基础种子
        pixelSeedBase = (pixelX & 0xFFFF) | ((pixelY & 0xFFFF) << 16);
    } 

    // 新增：将基础种子与当前维度结合的 Hash 函数
    inline uint32_t HashDimension(uint32_t baseSeed, uint32_t dim) {
        uint32_t s = baseSeed;
        s ^= dim * 0x85ebca6b; // 混入维度信息

        // 阶段 2：混淆与雪崩 (MurmurHash3 Finalizer)
        s ^= s >> 16;
        s *= 0x85ebca6b;
        s ^= s >> 13;
        s *= 0xc2b2ae35;
        s ^= s >> 16;
        
        return s;
    }

    float get1D(){
        return SobolSample(sampleIndex, dimension++);
    }
    
    Vec2f get2D(){
        float u = get1D();
        float v = get1D();
        return Vec2f(u, v);
    }

    inline float SobolSample(uint64_t sampleIndex, uint32_t dim) {
        uint32_t result = 0;
        
        // 安全防护：防止路径过深导致维度超出矩阵大小 (1024)
        uint32_t safeDim = dim % 1024; 
        uint32_t matrixOffset = safeDim * 52; 

        // 核心：计算当前维度的专属扰乱种子
        uint32_t dimSeed = HashDimension(pixelSeedBase, safeDim);

        for (uint32_t c = 0; sampleIndex != 0; sampleIndex >>= 1, ++c) {
            if (c >= 52) break;
            if (sampleIndex & 1) {
                result ^= SobolMatrices32[matrixOffset + c];
            }
        }
        
        // 使用当前维度的专属种子进行扰乱
        result ^= dimSeed;
        
        float res = static_cast<float>(result) * 0x1p-32f;
        return std::min(res, 0.99999994f);
    }
};
