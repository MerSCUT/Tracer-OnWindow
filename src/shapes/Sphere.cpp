#include"stat_render/shapes/Sphere.h"

bool Sphere::hit(const Ray& ray)
{
    // Test only
    auto a = dot(ray.direct, ray.direct);
    auto b = 2 * dot(ray.direct, ray.origin-center);
    auto c = dot(ray.origin-center, ray.origin-center) - radius *radius;
    // Discriminant
    auto Delta = b * b - 4 * a * c;
    if (Delta < 0) return false;

    // Time
    auto t1 = (-b + std::sqrt(Delta))/(2. * a);
    auto t2 = (-b - std::sqrt(Delta))/(2. * a);
    if (std::max(t1,t2) < 0) return false;
    return true;
}

Hit Sphere::intersect(const Ray& ray)
{
    // Test with Hit
    Hit payload;

    auto a = dot(ray.direct, ray.direct);
    auto b = 2 * dot(ray.direct, ray.origin-center);
    auto c = dot(ray.origin-center, ray.origin-center) - radius *radius;

    // Discriminant
    auto Delta = b * b - 4 * a * c;
    if (Delta < 0) return payload;

    // Time
    auto t1 = (-b + std::sqrt(Delta))/(2 * a);
    auto t2 = (-b - std::sqrt(Delta))/(2 * a);
    if (t1 >= t2) std::swap(t1,t2);
    if (t2 < 0) return payload;

    // Pass
    payload.intersected = true;
    payload.tmin = t1 < 0 ? t2 : t1;
    
    payload.position = ray(payload.tmin);
    payload.material= material;
    payload.normal = (payload.position - center).normalized();
    payload.incident = -ray.direct;
    return payload;
}