#pragma once

#include"stat_render/core/common.h"
#include"stat_render/scenes/Scene.h"
#include"stat_render/core/Camera.h"
#include"stat_render/core/Film.h"

enum class Mode { PathTracing, NormalMap };
class Renderer
{
private:
    
    // Camera camera;
    // float l,r,t,b;
    Mode mode;

public:
    Renderer() : mode(Mode::PathTracing) {}
    Renderer(Mode mode_):
    mode(mode_) {}
    
    ~Renderer() = default;

    // Pipeline
    void RenderPipeline(const Scene& scene, Film& film, const Camera& camera);
    void RenderMultiThreading(const Scene& scene, Film& film, const Camera& camera);
    Color3f CastRay(const Ray& ray, const Scene& scene, int depth, SobolSampler& sampler);
};