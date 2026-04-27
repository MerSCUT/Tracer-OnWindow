#pragma once

#include "stat_render/core/common.h"
#include "stat_render/shapes/Mesh.h"





class MeshLoader {
public:
	MeshLoader() = default;
	~MeshLoader() = default;

	MeshGeometry LoadMeshFromOBJ(const std::string& filepath);
};