#pragma once
#include"stat_render/core/common.h"
#include"stat_render/materials/Material.h"

enum class DiffuseColor { BLUE, GREEN, RED, WHITE };
enum class SamplingStrategy { Uniform, CosineWeighted };

class Diffuse : public Material{
private:
    Color3f albedo = Color3f(0.f, 0.f, 0.f);     // 漫反射系数
    SamplingStrategy strategy = SamplingStrategy::CosineWeighted;
public:
    Diffuse() = default;
    Diffuse(Color3f k, SamplingStrategy _strategy = SamplingStrategy::CosineWeighted) : albedo(k), strategy(_strategy) {}
    Diffuse(DiffuseColor dc, SamplingStrategy _strategy = SamplingStrategy::CosineWeighted);
    

    Vec3f sample(const Vec3f& wi, const Vec3f& n, SobolSampler& smapler) override;
    
    float pdf(const Vec3f & wi, const Vec3f & wo, const Vec3f& n) override;
    
    Color3f eval(const Vec3f & wi, const Vec3f& wo, const Vec3f & n) override;
};