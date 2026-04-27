#include"stat_render/scenes/parser.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm> // for std::replace, std::min, std::max
#include <limits>    // 引入 limits 用于初始化包围盒边界

// 辅助函数：从 "v/vt/vn" 格式的字符串中提取顶点索引
int get_vertex_index(std::string str) {
    size_t slash_pos = str.find('/');
    if (slash_pos != std::string::npos) {
        return std::stoi(str.substr(0, slash_pos));
    }
    return std::stoi(str);
}

std::shared_ptr<Object> Parser::loadOBJ(const std::string& filename, std::shared_ptr<Material> material) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return nullptr;
    }

    std::vector<Vec3f> vertices;
    std::vector<uint32_t> indices;
    std::string line;

    // 用于计算 Bounding Box 的极值
    float min_x = std::numeric_limits<float>::max();
    float min_y = std::numeric_limits<float>::max();
    float min_z = std::numeric_limits<float>::max();
    
    float max_x = std::numeric_limits<float>::lowest();
    float max_y = std::numeric_limits<float>::lowest();
    float max_z = std::numeric_limits<float>::lowest();

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string prefix;
        ss >> prefix;

        if (prefix == "v") {
            float x, y, z;
            ss >> x >> y >> z;
            vertices.emplace_back(x, y, z);

            // 如果需要归一化，在读取时顺便记录极值
            // if (normalize) {
            //     min_x = std::min(min_x, x);
            //     min_y = std::min(min_y, y);
            //     min_z = std::min(min_z, z);
                
            //     max_x = std::max(max_x, x);
            //     max_y = std::max(max_y, y);
            //     max_z = std::max(max_z, z);
            // }
        } 
        else if (prefix == "f") {
            std::string s1, s2, s3;
            ss >> s1 >> s2 >> s3;

            indices.push_back(get_vertex_index(s1) - 1);
            indices.push_back(get_vertex_index(s2) - 1);
            indices.push_back(get_vertex_index(s3) - 1);
        }
    }
    file.close();

    // --- 归一化处理模块 ---
    // if (normalize && !vertices.empty()) {
    //     // 计算中心点
    //     float center_x = (max_x + min_x) * 0.5f;
    //     float center_y = (max_y + min_y) * 0.5f;
    //     float center_z = (max_z + min_z) * 0.5f;

    //     // 计算最大跨度，用于等比例缩放 (Uniform Scaling)
    //     float extent_x = max_x - min_x;
    //     float extent_y = max_y - min_y;
    //     float extent_z = max_z - min_z;
    //     float max_extent = std::max({extent_x, extent_y, extent_z});

    //     if (max_extent > 0.0f) {
    //         // 缩放系数：将最大跨度映射到 2.0 (即 [-1, 1] 区间的长度)
    //         float scale = 2.0f / max_extent;

    //         for (auto& v : vertices) {
    //             // 假设 Vec3f 可以直接访问 .x, .y, .z
    //             v.x = (v.x - center_x) * scale;
    //             v.y = (v.y - center_y) * scale;
    //             v.z = (v.z - center_z) * scale;
    //         }
    //         std::cout << "Normalized mesh to [-1, 1]^3 with scale factor: " << scale << std::endl;
    //     }
    // }
    // ----------------------

    std::cout << "Loaded Mesh: " << filename 
              << " | Vertices: " << vertices.size() 
              << " | Triangles: " << indices.size() / 3 << std::endl;
    auto shared = std::make_shared<Mesh>(vertices, indices, material.get());
    return shared;
}
