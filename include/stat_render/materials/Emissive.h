#pragma once
#include"stat_render/core/common.h"
#include"stat_render/materials/Material.h"
#include"stat_render/textures/Texture.h"
class Emissive : public Material
{
private:
    Color3f emission;          // 基础发光强度
    Texture* map;               // 纹理
public:
    Emissive() :  
    map(nullptr),
    emission(Color3f(1.,1.,1.)) {}
    
    Emissive(const Color3f& emission_) : 
    map(nullptr), 
    emission(emission_) {}


    bool isEmissive() const override { return true; }

    Color3f getEmission() const override { return emission; }

    Vec3f sample(const Vec3f& wi, const Vec3f& n, SobolSampler& sampler) override { return Vec3f(0.f, 0.f, 0.f); };

    float pdf(const Vec3f & wi, const Vec3f & wo, const Vec3f& n) override { return 0.0f; }
    
    Color3f eval(const Vec3f & wi, const Vec3f& wo, const Vec3f & n) override {return Color3f(0.0f, 0.f, 0.f); }
};