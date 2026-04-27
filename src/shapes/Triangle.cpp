#include"stat_render/shapes/Triangle.h"
#include"stat_render/samplers/sampler.h"

Hit Triangle::intersect(const Ray& ray)
{
    Hit payload;
    payload.intersected = false; 

    Vec3f e1 = v1 - v0;
    Vec3f e2 = v2 - v0;

    
    Vec3f pvec = cross(ray.direct, e2); // S1
    
    
    float det = dot(e1, pvec);

    // det 接近 0，射线与三角形所在平面平行
    if (std::fabs(det) < 1e-12) {
        return payload;
    }

    float invDet = 1.0f / det;
    Vec3f tvec = ray.origin - v0;       // S

    
    
    float u = dot(tvec, pvec) * invDet;
    if (u < -Epsilon || u > 1.0f + Epsilon) {
        return payload;
    }

    
    Vec3f qvec = cross(tvec, e1);       // S2
    float v = dot(ray.direct, qvec) * invDet;
    if (v < -Epsilon || u + v > 1.0f + Epsilon) {
        return payload;
    }

    
    float t = dot(e2, qvec) * invDet;
    
    // 避免背面相交
    if (t < 0.0) {
        return payload;
    }

    // 求交成功
    payload.intersected = true;
    payload.tmin = t;
    payload.position = ray(t);
    payload.dist_square = (ray.origin - payload.position).norm2();
    payload.incident = -ray.direct;
    payload.material = material;
    payload.obj = this;
    
    Vec3f normal = cross(e1, e2).normalized();
    
    
    if (det < 0.0f) {
        normal = -normal;
    }
    payload.normal = normal;

    return payload;
}

bool Triangle::hit(const Ray& ray)
{
    return true;
}

void Triangle::sample(float xi1, float xi2, Point3f& position, Vec3f& normal, float& pdf) const
{

    float sqrt_xi1 = std::sqrt(xi1);
    float b0 = 1.0f - sqrt_xi1;
    float b1 = xi2 * sqrt_xi1;
    float b2 = 1.0f - b0 - b1;
    position = b0 * v0 + b1 * v1 + b2 * v2;

    // 无平滑法线, 直接返回面法线
    normal = cross(v1 - v0, v2 - v0).normalized();
    pdf = 1.0f / SurfaceArea();
    return;
}

void Triangle::transform(const Mat4f& M)
{
    auto v0h = toPoint4D(v0);
    auto v1h = toPoint4D(v1);
    auto v2h = toPoint4D(v2);
    v0 = (M * v0h).xyz();
    v1 = (M * v1h).xyz();
    v2 = (M * v2h).xyz();
    Vec3f pmin = v0.cwiseMin(v1).cwiseMin(v2);
    Vec3f pmax = v0.cwiseMax(v1).cwiseMax(v2);
    bound = Bound(pmin, pmax); 
}