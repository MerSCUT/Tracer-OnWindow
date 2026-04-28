#pragma once
#include<algorithm>
#include<string>
#include"stat_render/core/Vector.h"
#include<vector>

#ifdef __INTELLISENSE__
// 告诉 IntelliSense 看到这些符号时不要报错
#define KERNEL_ARGS2(grid, block) 
#define KERNEL_ARGS3(grid, block, sh_mem) 
#define KERNEL_ARGS4(grid, block, sh_mem, stream) 
#else
#define KERNEL_ARGS2(grid, block) <<< grid, block >>>
#define KERNEL_ARGS3(grid, block, sh_mem) <<< grid, block, sh_mem >>>
#define KERNEL_ARGS4(grid, block, sh_mem, stream) <<< grid, block, sh_mem, stream >>>
#endif

inline constexpr float Inf = std::numeric_limits<float>::infinity();
inline constexpr float Pi = 3.1415926535897932384626;
inline constexpr float inv_Pi = 1./Pi;
inline constexpr float Epsilon = 1e-5f;

//inline constexpr int resolution = 1024;

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

inline Vec3f normalize(const Vec3f& v)
{
	float len = v.norm();
	if (len <= 0.0f) {
		std::cout << "len : " << len << std::endl;
		assert(len > 0.0f);
	}
	float inv = 1.0f / len;
	return Vec3f(v.x * inv, v.y * inv, v.z * inv);
}