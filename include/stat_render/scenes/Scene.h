#pragma once
#include<vector>
#include"stat_render/shapes/Object.h"
#include"stat_render/lights/AreaLights.h"
#include"stat_render/materials/Diffuse.h"
#include"stat_render/materials/Emissive.h"
#include"stat_render/materials/Microfacet.h"
#include"stat_render/scenes/parser.h"
#include<iostream>
#include<memory>
#include"stat_render/samplers/QMC.h"
class Scene
{
private:
    // std::vector<Object*> objects;
    std::vector<std::shared_ptr<Object>> objects;
    // std::vector<Light*> lights;
    std::vector<std::shared_ptr<Light>> lights;
    std::vector<std::shared_ptr<Material>> material_pool;
    Mat4f M_normalize;
public:
    Scene() {}
    
    ~Scene() = default;
    void AddObject(std::shared_ptr<Object> object) { objects.push_back(std::move(object)); }
    void AddLight(std::shared_ptr<Light> light) { lights.push_back(std::move(light)); }
    Mat4f getNormalizeMatrix() { return M_normalize; }

    void loadOBJlist(const std::vector<std::string>& paths, const std::vector<Color3f>& emissions, const std::vector<DiffuseColor>& dcs);
    

    void loadOBJ(const std::string& path,  Bound& boundbox, const Color3f& emission = Color3f(0.0f, 0.0f, 0.0f), const DiffuseColor dc = DiffuseColor::WHITE);
    void loadBunny(const std::string& path,  Bound& boundbox, const Color3f& emission, const DiffuseColor dc);
    
    inline std::vector<std::shared_ptr<Object>> getObjects() const { return objects; }
    inline std::vector<std::shared_ptr<Light>> getLights() const { return lights; }
    
    Hit intersect(const Ray& ray) const;
    // 在 light 列表里采样
    LightSample sampleLight(SobolSampler& sampler) const;
    float getLightpdf(Object* obj) const {
        return 1.0f / obj->SurfaceArea();
    }
};