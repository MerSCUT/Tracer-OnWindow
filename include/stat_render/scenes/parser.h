#pragma once

#include"stat_render/core/common.h"
#include"stat_render/shapes/Mesh.h"
#include"stat_render/core/Camera.h"
class Parser
{
private:
public:
    static bool loadOBJ(const std::string& path, Mesh& mesh);
    static std::shared_ptr<Object> loadOBJ(const std::string& filename, std::shared_ptr<Material> material);
    //static Mesh* loadOBJ(const std::string& filename, std::shared_ptr<Material> material);m
};