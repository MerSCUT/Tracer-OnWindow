#include"stat_render/accelerators/Bound.h"
#include<iostream>
void Bound::Union(const Bound& other)
{
    Pmin = Pmin.cwiseMin(other.Pmin);
    Pmax = Pmax.cwiseMax(other.Pmax);
}

bool Bound::intersect(const Ray& ray) {
    float tin = -std::numeric_limits<float>::max();
    float tmax = std::numeric_limits<float>::max();

    for(int i = 0; i < 3; i++) {
        float invD = ray.inv_direct[i];
        float t1 = (Pmin[i] - ray.origin[i]) * invD;
        float t2 = (Pmax[i] - ray.origin[i]) * invD;

        if (ray.direct[i] < 0.0f) {
            float tmp = t1; t1 = t2; t2 = tmp;
        }

        tin = t1 > tin ? t1 : tin;
        tmax = t2 < tmax ? t2 : tmax;

        if (tin > tmax) return false;
    }
    
    return tmax > 1e-5f; 
}