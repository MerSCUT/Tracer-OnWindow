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

    // new member ==============
    std::vector<std::unique_ptr<Object>> unique_objects;
    std::vector<std::unique_ptr<AreaLight>> unique_lights;
    Bound sbound;
	std::unique_ptr<BVH> TLAS;
    // new member ==============

    Mat4f M_normalize;
public:
    Scene() {}
    
    ~Scene() = default;
    Mat4f getNormalizeMatrix() { return M_normalize; }

	void BuildTLAS();

    
    void pushObject(std::unique_ptr<Mesh> object) {
        sbound.Union(object->world_bound);
        unique_objects.push_back(std::move(object));
    }

    void pushLight(std::unique_ptr<AreaLight> light) {
        unique_lights.push_back(std::move(light)) ;
    }    

    Hit intersect(const Ray& ray) const;
    
    LightSample sampleLight(SobolSampler& sampler) const;

    float getLightpdf(Object* obj) const {
        return 1.0f / obj->SurfaceArea();
    }
};