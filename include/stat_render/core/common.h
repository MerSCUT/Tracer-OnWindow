#pragma once
#include<algorithm>
#include<string>
#include"stat_render/core/Vector.h"
#include<vector>


inline constexpr float Inf = std::numeric_limits<float>::infinity();
inline constexpr float Pi = 3.1415926535897932384626;
inline constexpr float inv_Pi = 1./Pi;
inline constexpr float Epsilon = 1e-5f;

inline constexpr int resolution = 1024;

inline constexpr float deg2rad(float deg) { return deg * Pi / 180.0f; }
// 球面坐标映射到3D方向向量
inline Vec3f SphTo3D(float theta, float phi)
{
    return Vec3f(
        std::sin(theta)*std::cos(phi),
        std::sin(theta)*std::sin(phi),
        std::cos(theta)
    );
}
inline std::string output_path = "../images/output.ppm";