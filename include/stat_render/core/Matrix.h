#pragma once
#include "Vector.h"
#include <cstring> // for std::memset
class Mat4f {
public:
    // Row-major
    float m[4][4];

    // Identity matrix by default
    Mat4f() {
        std::memset(m, 0, sizeof(m));
        m[0][0] = 1.0f; m[1][1] = 1.0f;
        m[2][2] = 1.0f; m[3][3] = 1.0f;
    }

    Mat4f(float t00, float t01, float t02, float t03,
          float t10, float t11, float t12, float t13,
          float t20, float t21, float t22, float t23,
          float t30, float t31, float t32, float t33) {
        m[0][0] = t00; m[0][1] = t01; m[0][2] = t02; m[0][3] = t03;
        m[1][0] = t10; m[1][1] = t11; m[1][2] = t12; m[1][3] = t13;
        m[2][0] = t20; m[2][1] = t21; m[2][2] = t22; m[2][3] = t23;
        m[3][0] = t30; m[3][1] = t31; m[3][2] = t32; m[3][3] = t33;
    }

    Mat4f operator*(const Mat4f& rhs) const {
        Mat4f res;
        for (int i = 0; i < 4; ++i) {       // 行
            for (int j = 0; j < 4; ++j) {   // 列
                res.m[i][j] = m[i][0] * rhs.m[0][j] +
                              m[i][1] * rhs.m[1][j] +
                              m[i][2] * rhs.m[2][j] +
                              m[i][3] * rhs.m[3][j];
            }
        }
        return res;
    }

    Vec4f operator*(const Vec4f& v) const {
        return Vec4f(
            m[0][0]*v.x + m[0][1]*v.y + m[0][2]*v.z + m[0][3]*v.w,
            m[1][0]*v.x + m[1][1]*v.y + m[1][2]*v.z + m[1][3]*v.w,
            m[2][0]*v.x + m[2][1]*v.y + m[2][2]*v.z + m[2][3]*v.w,
            m[3][0]*v.x + m[3][1]*v.y + m[3][2]*v.z + m[3][3]*v.w
        );
    }

    Mat4f transpose() const {
        return Mat4f(
            m[0][0], m[1][0], m[2][0], m[3][0],
            m[0][1], m[1][1], m[2][1], m[3][1],
            m[0][2], m[1][2], m[2][2], m[3][2],
            m[0][3], m[1][3], m[2][3], m[3][3]
        );
    }
    
    // 矩阵求逆 (Inverse)：在光线追踪中极为重要！
    // 声明于此，通常为了避免头文件臃肿，会在 .cpp 中实现高斯-约当消元或伴随矩阵法
    Mat4f inverse() const; 
};