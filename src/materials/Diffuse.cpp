#include"stat_render/materials/Diffuse.h"
#include<random>

Diffuse::Diffuse(DiffuseColor dc, SamplingStrategy _strategy):
strategy(_strategy)
{
    switch (dc)
    {
    case DiffuseColor::BLUE :
    {
        albedo = Color3f(0.14, 0.14, 0.45);
        break;
    }
    case DiffuseColor::GREEN :
    {
        albedo = Color3f(0.14, 0.45, 0.091);
        break;
    }
    case DiffuseColor::WHITE :
    {
        albedo = Color3f(0.725, 0.71, 0.68);
        break;
    }
    case DiffuseColor::RED :
    {
        albedo = Color3f(0.63, 0.065, 0.05);
        break;
    }
    }
}

Vec3f Diffuse::sample(const Vec3f& wi, const Vec3f& n, SobolSampler& sampler)
{
    
    //Sampler sampler;
    float u1 = sampler.get1D();
    float u2 = sampler.get1D();
    // 计算局部坐标系中的方向
    float x,y,z;
    switch (strategy)
    {
    case SamplingStrategy::Uniform :
    {
        // 均匀采样
        float phi = 2.0f * Pi * u2;
        float sin_theta = std::sqrt(1- u1 * u1);

        x = sin_theta * std::cos(phi);
        y = sin_theta * std::sin(phi);
        z = u1;
        break;
    }
    case SamplingStrategy::CosineWeighted :
    {
        // 余弦加权采样
        float sin_theta = std::sqrt(u1);
        float phi = 2.0f * Pi * u2;
        x = sin_theta * std::cos(phi);
        y = sin_theta * std::sin(phi);
        z = std::sqrt(1.0f - u1);
        break;
    }
    }
    // 根据 n 转换到世界坐标系
    Vec3f t;
    if (std::abs(n.x) > std::abs(n.y)) {
        float invLen = 1.0f / std::sqrt(n.x * n.x + n.z * n.z);
        t = Vec3f(n.z * invLen, 0.0f, -n.x * invLen);
    } else {
        float invLen = 1.0f / std::sqrt(n.y * n.y + n.z * n.z);
        t = Vec3f(0.0f, n.z * invLen, -n.y * invLen);
    }
    Vec3f b = cross(n, t);
    auto wo = x * t + y * b + z * n;
    assert(wo.norm2() > 0 && "Diffuse Sample 0 vector");

    return x * t + y * b + z * n;
}


float Diffuse::pdf(const Vec3f & wi, const Vec3f & wo, const Vec3f& n)
{
    // 给定 wo, n, 采样到 wi 的概率.
    switch (strategy)
    {
    case SamplingStrategy::Uniform :
    {
        return 0.5f * inv_Pi;
        break;
    }
    case SamplingStrategy::CosineWeighted :
    {
        // 余弦加权采样
        // p(wi) = cos(theta_i) / Pi
        return wi.dot(n) * inv_Pi;
        break;
    }
    }
    return wi.dot(n) * inv_Pi;      // CosineWeighted by default;
}
    
Color3f Diffuse::eval(const Vec3f & wi, const Vec3f& wo, const Vec3f & n)
{
    // BRDF 常数.
    // 确保 wo, wi 方向朝外
    if (dot(n, wi) > 0.0f && dot(n, wo) > 0.0f)
    {
        return albedo * inv_Pi;
    }
    return Color3f(0.0f);
}