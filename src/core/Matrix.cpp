#include"stat_render/core/Matrix.h"

Mat4f Mat4f::inverse() const {
    Mat4f inv;

    // 前两行的 2x2 子矩阵的行列式
    float s0 = m[0][0] * m[1][1] - m[1][0] * m[0][1];
    float s1 = m[0][0] * m[1][2] - m[1][0] * m[0][2];
    float s2 = m[0][0] * m[1][3] - m[1][0] * m[0][3];
    float s3 = m[0][1] * m[1][2] - m[1][1] * m[0][2];
    float s4 = m[0][1] * m[1][3] - m[1][1] * m[0][3];
    float s5 = m[0][2] * m[1][3] - m[1][2] * m[0][3];

    // 后两行的 2x2 子矩阵的行列式
    float c5 = m[2][2] * m[3][3] - m[3][2] * m[2][3];
    float c4 = m[2][1] * m[3][3] - m[3][1] * m[2][3];
    float c3 = m[2][1] * m[3][2] - m[3][1] * m[2][2];
    float c2 = m[2][0] * m[3][3] - m[3][0] * m[2][3];
    float c1 = m[2][0] * m[3][2] - m[3][0] * m[2][2];
    float c0 = m[2][0] * m[3][1] - m[3][0] * m[2][1];

    // 计算整个 4x4 矩阵的行列式
    float det = (s0 * c5 - s1 * c4 + s2 * c3 + s3 * c2 - s4 * c1 + s5 * c0);

    // 奇异矩阵 (不可逆) 检查
    if (std::abs(det) <= std::numeric_limits<float>::epsilon()) {
        // 根据工程需求，这里可以抛出异常、打印 warning，或者直接返回单位矩阵
        return Mat4f::Identity();
    }

    float invDet = 1.0f / det;

    // 计算伴随矩阵并乘以 1/det
    inv.m[0][0] = (m[1][1] * c5 - m[1][2] * c4 + m[1][3] * c3) * invDet;
    inv.m[0][1] = (-m[0][1] * c5 + m[0][2] * c4 - m[0][3] * c3) * invDet;
    inv.m[0][2] = (m[3][1] * s5 - m[3][2] * s4 + m[3][3] * s3) * invDet;
    inv.m[0][3] = (-m[2][1] * s5 + m[2][2] * s4 - m[2][3] * s3) * invDet;

    inv.m[1][0] = (-m[1][0] * c5 + m[1][2] * c2 - m[1][3] * c1) * invDet;
    inv.m[1][1] = (m[0][0] * c5 - m[0][2] * c2 + m[0][3] * c1) * invDet;
    inv.m[1][2] = (-m[3][0] * s5 + m[3][2] * s2 - m[3][3] * s1) * invDet;
    inv.m[1][3] = (m[2][0] * s5 - m[2][2] * s2 + m[2][3] * s1) * invDet;

    inv.m[2][0] = (m[1][0] * c4 - m[1][1] * c2 + m[1][3] * c0) * invDet;
    inv.m[2][1] = (-m[0][0] * c4 + m[0][1] * c2 - m[0][3] * c0) * invDet;
    inv.m[2][2] = (m[3][0] * s4 - m[3][1] * s2 + m[3][3] * s0) * invDet;
    inv.m[2][3] = (-m[2][0] * s4 + m[2][1] * s2 - m[2][3] * s0) * invDet;

    inv.m[3][0] = (-m[1][0] * c3 + m[1][1] * c1 - m[1][2] * c0) * invDet;
    inv.m[3][1] = (m[0][0] * c3 - m[0][1] * c1 + m[0][2] * c0) * invDet;
    inv.m[3][2] = (-m[3][0] * s3 + m[3][1] * s1 - m[3][2] * s0) * invDet;
    inv.m[3][3] = (m[2][0] * s3 - m[2][1] * s1 + m[2][2] * s0) * invDet;

    return inv;
}