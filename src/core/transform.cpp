#include"stat_render/core/transform.h"

Transform Transform::Translate(const Vec3f& delta) {
    Mat4f m(1.0f, 0.0f, 0.0f, delta.x,
        0.0f, 1.0f, 0.0f, delta.y,
        0.0f, 0.0f, 1.0f, delta.z,
        0.0f, 0.0f, 0.0f, 1.0f);

    Mat4f minv(1.0f, 0.0f, 0.0f, -delta.x,
        0.0f, 1.0f, 0.0f, -delta.y,
        0.0f, 0.0f, 1.0f, -delta.z,
        0.0f, 0.0f, 0.0f, 1.0f);

    return Transform(m, minv);
}

Transform Transform::Scale(float x, float y, float z) {
    Mat4f m(x, 0.0f, 0.0f, 0.0f,
        0.0f, y, 0.0f, 0.0f,
        0.0f, 0.0f, z, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f);

    Mat4f minv(1.0f / x, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f / y, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f / z, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f);

    return Transform(m, minv);
}

Transform Transform::Rotate(float angle, const Vec3f& axis) {
    // 假设 angle 的输入为弧度制。如果是角度制，需先转换为弧度：
    // float rad = angle * std::numbers::pi_v<float> / 180.0f;

    // 确保旋转轴被归一化
    float length = std::sqrt(axis.x * axis.x + axis.y * axis.y + axis.z * axis.z);
    float ax = axis.x / length;
    float ay = axis.y / length;
    float az = axis.z / length;

    float s = std::sin(angle);
    float c = std::cos(angle);
    float t = 1.0f - c;

    Mat4f m(
        t * ax * ax + c, t * ax * ay - s * az, t * ax * az + s * ay, 0.0f,
        t * ax * ay + s * az, t * ay * ay + c, t * ay * az - s * ax, 0.0f,
        t * ax * az - s * ay, t * ay * az + s * ax, t * az * az + c, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );

    // 利用纯旋转矩阵的正交性，直接转置得到逆矩阵
    return Transform(m, m.transpose());
}

Mat4f ViewTransform(const Point3f p, Vec3f g, Vec3f t)
{
    g.normalize();
    t.normalize();
    Mat4f translation(
                    1., 0., 0., -p.x,
                    0., 1., 0., -p.y,
                    0., 0., 1., -p.z,
                    0., 0., 0., 1.
                );
                   
    auto gCrosst = cross(g, t); 
    gCrosst.normalize();
    Mat4f rotation(
                gCrosst.x, gCrosst.y, gCrosst.z, 0.,
                t.x,       t.y,       t.z,       0.,
               -g.x,      -g.y,      -g.z,       0.,
                0.,        0.,        0.,        1.
    );
    Mat4f View;
    View = rotation * translation;
    return View;
}

Mat4f CameraToWorldTransform(const Point3f p, Vec3f g, Vec3f t)
{
    g.normalize();
    t.normalize();
    
    auto u = cross(g, t).normalized(); 
    Mat4f camToWorld(u.x, t.x, -g.x, p.x,
                  u.y, t.y, -g.y, p.y,
                  u.z, t.z, -g.z, p.z,
                  0.f,   0.f,   0.f,   1.f);
                  
    return camToWorld;
}

Mat4f PerspectiveTransform()
{
    return Mat4f();
}


