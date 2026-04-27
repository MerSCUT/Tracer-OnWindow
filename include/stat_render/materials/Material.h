#pragma once

#include"stat_render/core/common.h"
#include"stat_render/samplers/sampler.h" 
#include"stat_render/samplers/QMC.h"
enum class MaterialType { Diffuse, Specular, Microfacet, Emissive };

class Material{
private:
    //MaterialType type;
public:
    
    // Material() : type(MaterialType::Diffuse) {}
    // Material(MaterialType type_) : type(type_) {}

    //MaterialType getType() const { return type; }
    
    virtual ~Material() = default;

    virtual bool isEmissive() const { return false; }

    virtual Color3f getEmission() const { return Color3f(1.0f,1.0f, 1.0f); }


    // 根据出射方向 w_o 采样入射方向 w_i
    virtual Vec3f sample(const Vec3f& wi, const Vec3f& n, SobolSampler& sampler) = 0;      
    // 计算采样到 wi 的概率 
    virtual float pdf(const Vec3f & wi, const Vec3f & wo, const Vec3f& n) = 0;
    // 计算 BSDF(p, wi, wo)
    virtual Color3f eval(const Vec3f & wi, const Vec3f& wo, const Vec3f & n) = 0;

    
};