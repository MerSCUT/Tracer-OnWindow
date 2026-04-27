#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include "stat_render/shapes/Mesh.h"
#include <unordered_map>
#include "stat_render/scenes/MeshLoader.h"
struct TinyObjIndexHash {
    size_t operator()(const tinyobj::index_t& idx) const {
        size_t h1 = std::hash<int>()(idx.vertex_index);
        size_t h2 = std::hash<int>()(idx.normal_index);
        size_t h3 = std::hash<int>()(idx.texcoord_index);
        // 简单的哈希组合 (Hash Combine)
        return h1 ^ (h2 << 1) ^ (h3 << 2);
    }
};

struct TinyObjIndexEqual {
    bool operator()(const tinyobj::index_t& a, const tinyobj::index_t& b) const {
        return a.vertex_index == b.vertex_index &&
            a.normal_index == b.normal_index &&
            a.texcoord_index == b.texcoord_index;
    }
};

MeshGeometry MeshLoader::LoadMeshFromOBJ(const std::string& filepath) {
    // 配置 Reader
    MeshGeometry meshGeometry;
    tinyobj::ObjReaderConfig reader_config;
    reader_config.mtl_search_path = "./";
    reader_config.triangulate = true;     // 确保多边形被转换为三角形

    tinyobj::ObjReader reader;

    // 执行解析
    if (!reader.ParseFromFile(filepath, reader_config)) {
        if (!reader.Error().empty()) {
            std::cerr << "TinyObjReader 错误: " << reader.Error() << std::endl;
        }
        return meshGeometry; // 在引擎开发中，使用返回值而不是 exit(1) 更安全
    }

    if (!reader.Warning().empty()) {
        std::cout << "TinyObjReader 警告: " << reader.Warning() << std::endl;
    }

    auto& attrib = reader.GetAttrib();
    auto& shapes = reader.GetShapes();

    // 清空输出容器，防止重复追加
    meshGeometry.vertices.clear();
    meshGeometry.indices.clear();

    // 核心：使用哈希表记录已经处理过的唯一顶点组合
    // Key: obj 原始索引组合, Value: outMesh.vertices 中的新索引
    std::unordered_map<tinyobj::index_t, int, TinyObjIndexHash, TinyObjIndexEqual> uniqueVertices;

    // 遍历所有 Shape (即使约定只有一个 Mesh，遍历也是安全的)
    for (const auto& shape : shapes) {
        size_t index_offset = 0;

        for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
            size_t fv = size_t(shape.mesh.num_face_vertices[f]);

            // 防御性检查：确保确实是三角形
            if (fv != 3) {
                std::cout << "[Info] 导入" + filepath + "时检测到非三角形图元" << std::endl;
                index_offset += fv;
                continue;
            }

            TriangleIndices tri;
            int current_face_indices[3];

            for (size_t v = 0; v < 3; v++) {
                tinyobj::index_t idx = shape.mesh.indices[index_offset + v];

                // 如果该顶点组合是首次出现，则提取数据并压入顶点数组
                if (uniqueVertices.count(idx) == 0) {
                    Vertex vertex;

                    // 提取位置
                    vertex.position.x = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
                    vertex.position.y = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
                    vertex.position.z = attrib.vertices[3 * size_t(idx.vertex_index) + 2];

                    // 提取法线 (处理缺失情况)
                    if (idx.normal_index >= 0) {
                        vertex.normal.x = attrib.normals[3 * size_t(idx.normal_index) + 0];
                        vertex.normal.y = attrib.normals[3 * size_t(idx.normal_index) + 1];
                        vertex.normal.z = attrib.normals[3 * size_t(idx.normal_index) + 2];
                    }
                    else {
                        vertex.normal.x = 0.0f; vertex.normal.y = 0.0f; vertex.normal.z = 0.0f;
                    }

                    // 提取纹理坐标 (处理缺失情况)
                    if (idx.texcoord_index >= 0) {
                        vertex.texcoord.x = attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
                        vertex.texcoord.y = attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];
                    }
                    else {
                        vertex.texcoord.x = 0.0f; vertex.texcoord.y = 0.0f;
                    }

                    // 记录新索引并压入容器
                    uniqueVertices[idx] = static_cast<int>(meshGeometry.vertices.size());
                    meshGeometry.vertices.push_back(vertex);
                }

                // 从哈希表中获取该顶点的唯一索引
                current_face_indices[v] = uniqueVertices[idx];
            }

            // 构建三角形索引结构并压入容器
            tri.v1 = current_face_indices[0];
            tri.v2 = current_face_indices[1];
            tri.v3 = current_face_indices[2];
            meshGeometry.indices.push_back(tri);

            index_offset += 3;
        }
    }
    return meshGeometry;
}


bool LoadMeshFromOBJ(const std::string& filepath, Mesh& outMesh) {
    // 配置 Reader
    tinyobj::ObjReaderConfig reader_config;
    reader_config.mtl_search_path = "./";
    reader_config.triangulate = true;     // 确保多边形被转换为三角形

    tinyobj::ObjReader reader;

    // 执行解析
    if (!reader.ParseFromFile(filepath, reader_config)) {
        if (!reader.Error().empty()) {
            std::cerr << "TinyObjReader 错误: " << reader.Error() << std::endl;
        }
        return false; // 在引擎开发中，使用返回值而不是 exit(1) 更安全
    }

    if (!reader.Warning().empty()) {
        std::cout << "TinyObjReader 警告: " << reader.Warning() << std::endl;
    }

    auto& attrib = reader.GetAttrib();
    auto& shapes = reader.GetShapes();

    // 清空输出容器，防止重复追加
    outMesh.vertices.clear();
    outMesh.indices.clear();

    // 核心：使用哈希表记录已经处理过的唯一顶点组合
    // Key: obj 原始索引组合, Value: outMesh.vertices 中的新索引
    std::unordered_map<tinyobj::index_t, int, TinyObjIndexHash, TinyObjIndexEqual> uniqueVertices;

    // 遍历所有 Shape (即使约定只有一个 Mesh，遍历也是安全的)
    for (const auto& shape : shapes) {
        size_t index_offset = 0;

        for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
            size_t fv = size_t(shape.mesh.num_face_vertices[f]);

            // 防御性检查：确保确实是三角形
            if (fv != 3) {
                std::cout << "[Info] 导入" + filepath + "时检测到非三角形图元" << std::endl;
                index_offset += fv;
                continue;
            }

            TriangleIndices tri;
            int current_face_indices[3];

            for (size_t v = 0; v < 3; v++) {
                tinyobj::index_t idx = shape.mesh.indices[index_offset + v];

                // 如果该顶点组合是首次出现，则提取数据并压入顶点数组
                if (uniqueVertices.count(idx) == 0) {
                    Vertex vertex;

                    // 提取位置
                    vertex.position.x = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
                    vertex.position.y = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
                    vertex.position.z = attrib.vertices[3 * size_t(idx.vertex_index) + 2];

                    // 提取法线 (处理缺失情况)
                    if (idx.normal_index >= 0) {
                        vertex.normal.x = attrib.normals[3 * size_t(idx.normal_index) + 0];
                        vertex.normal.y = attrib.normals[3 * size_t(idx.normal_index) + 1];
                        vertex.normal.z = attrib.normals[3 * size_t(idx.normal_index) + 2];
                    }
                    else {
                        vertex.normal.x = 0.0f; vertex.normal.y = 0.0f; vertex.normal.z = 0.0f;
                    }

                    // 提取纹理坐标 (处理缺失情况)
                    if (idx.texcoord_index >= 0) {
                        vertex.texcoord.x = attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
                        vertex.texcoord.y = attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];
                    }
                    else {
                        vertex.texcoord.x = 0.0f; vertex.texcoord.y = 0.0f;
                    }

                    // 记录新索引并压入容器
                    uniqueVertices[idx] = static_cast<int>(outMesh.vertices.size());
                    outMesh.vertices.push_back(vertex);
                }

                // 从哈希表中获取该顶点的唯一索引
                current_face_indices[v] = uniqueVertices[idx];
            }

            // 构建三角形索引结构并压入容器
            tri.v1 = current_face_indices[0];
            tri.v2 = current_face_indices[1];
            tri.v3 = current_face_indices[2];
            outMesh.indices.push_back(tri);

            index_offset += 3;
        }
    }

    return true;
}