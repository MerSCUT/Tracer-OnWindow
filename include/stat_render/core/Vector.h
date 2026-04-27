#pragma once
#include<cassert>
#include<cmath>
#include<iostream>
class Vec3f
{
public:
    union {
        struct { float x, y, z; };
        struct { float r, g, b; };
        float e[3];
    };

    inline Vec3f() : x(0.0f), y(0.0f), z(0.0f) {}
    inline Vec3f(float a) : x(a), y(a), z(a) {}
    inline Vec3f(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
    ~Vec3f() = default;

    inline Vec3f& operator+=(const Vec3f& other) { x += other.x; y += other.y; z += other.z; return *this; }
    inline Vec3f& operator-=(const Vec3f& other) { x -= other.x; y -= other.y; z -= other.z; return *this; }
    inline Vec3f& operator*=(const Vec3f& v) { x*=v.x; y*=v.y; z*=v.z; return *this; }
    inline Vec3f& operator*=(const float t)  { x*=t; y*=t; z*=t; return *this; }
    inline Vec3f& operator/=(const float t)  { 
        
        assert(t != 0.0f && "Division by zero in Vec3f"); 
        float inv = 1.0f / t;
        x*=inv; y*=inv; z*=inv; 
        return *this; 
    }
    inline Vec3f operator-() const { return Vec3f(-x, -y, -z); }
    inline float operator[](int i) const { return e[i]; }
    inline float& operator[](int i) { return e[i]; }
    inline float norm2() const  { return x*x + y*y + z*z; }
    inline float norm() const { return std::sqrt(norm2()); }
    inline Vec3f normalized() const { 
        float len = norm();
        if (len <= 0.0f){
            std::cout << "len : " << len << std::endl;
            assert(len > 0.0f); 
        }
        float inv = 1.0f / len; 
        return Vec3f(x * inv, y * inv, z * inv);
    }
    inline void normalize() { float len = norm(); *this/=len; }
    inline Vec3f cross(const Vec3f& v) const {
        return Vec3f(
                    y * v.z - z * v.y,
                    z * v.x - x * v.z,
                    x * v.y - y * v.x
                );
    } 
    inline float dot(const Vec3f& v) const { return x * v.x + y * v.y + z * v.z; }
    inline Vec3f cwiseMin(const Vec3f& v) const { return Vec3f(std::min(x, v.x), std::min(y, v.y), std::min(z, v.z)); }

    inline Vec3f cwiseMax(const Vec3f& v) const { return Vec3f(std::max(x, v.x), std::max(y, v.y), std::max(z, v.z)); }
};



inline Vec3f operator+(const Vec3f& u, const Vec3f& v) { return Vec3f(u.x + v.x, u.y + v.y, u.z + v.z); }
inline Vec3f operator-(const Vec3f& u, const Vec3f& v) { return Vec3f(u.x - v.x, u.y - v.y, u.z - v.z); }
inline Vec3f operator*(const Vec3f& u, const Vec3f& v) { return Vec3f(u.x * v.x, u.y * v.y, u.z * v.z); }
inline Vec3f operator*(float t, const Vec3f& v) { return Vec3f(t * v.x, t * v.y, t * v.z); }
inline Vec3f operator*(const Vec3f& v, float t) { return t * v; }
inline Vec3f operator/(const Vec3f& v, float t) { assert(t != 0.0f && "Division by zero in Vec3f"); return (1.0f / t) * v; }
inline bool operator==(const Vec3f& u, const Vec3f& v) { return u.x == v.x && u.y == v.y && u.z == v.z; }

inline float dot(const Vec3f& u, const Vec3f& v) {
    return u.x * v.x + u.y * v.y + u.z * v.z;
}
inline Vec3f cross(const Vec3f& u, const Vec3f& v) {
    return Vec3f(u.y * v.z - u.z * v.y,
                 u.z * v.x - u.x * v.z,
                 u.x * v.y - u.y * v.x);
}
using Point3f = Vec3f;  // 3D 空间中的点
using Color3f = Vec3f;

class Vec2f {
public:
    union {
        struct { float x, y; };
        struct { float u, v; }; // 针对 UV 映射的语义别名
        float e[2];
    };

    inline Vec2f() : x(0), y(0) {}
    inline Vec2f(float e0, float e1) : x(e0), y(e1) {}
    explicit inline Vec2f(float v) : x(v), y(v) {}

    inline Vec2f& operator+=(const Vec2f& v) { x+=v.x; y+=v.y; return *this; }
    inline Vec2f& operator-=(const Vec2f& v) { x-=v.x; y-=v.y; return *this; }
    inline Vec2f& operator*=(const Vec2f& v) { x*=v.x; y*=v.y; return *this; }
    inline Vec2f& operator*=(const float t)  { x*=t; y*=t; return *this; }
    inline Vec2f& operator/=(const float t)  { 
        assert(t != 0.0f && "Division by zero in Vec2f");
        float inv = 1.0f / t; 
        x*=inv; y*=inv; 
        return *this; 
    }

    inline float operator[](int i) const { return e[i]; }
    inline float& operator[](int i) { return e[i]; }

    inline float lengthSquared() const { return x*x + y*y; }
    inline float length() const { return std::sqrt(lengthSquared()); }
};

inline Vec2f operator+(const Vec2f& u, const Vec2f& v) { return Vec2f(u.x + v.x, u.y + v.y); }
inline Vec2f operator-(const Vec2f& u, const Vec2f& v) { return Vec2f(u.x - v.x, u.y - v.y); }
inline Vec2f operator*(const Vec2f& u, const Vec2f& v) { return Vec2f(u.x * v.x, u.y * v.y); }
inline Vec2f operator*(float t, const Vec2f& v) { return Vec2f(t * v.x, t * v.y); }
inline Vec2f operator*(const Vec2f& v, float t) { return t * v; }
inline Vec2f operator/(const Vec2f& v, float t) { return (1.0f / t) * v; }

using Point2f = Vec2f; 

class Vec4f {
public:
    union {
        struct { float x, y, z, w; };
        float e[4];
    };

    inline Vec4f() : x(0), y(0), z(0), w(0) {}
    inline Vec4f(float e0, float e1, float e2, float e3) : x(e0), y(e1), z(e2), w(e3) {}
    
    // Implicit 3D to 4D
    inline Vec4f(const Vec3f& v, float w_val) : x(v.x), y(v.y), z(v.z), w(w_val) {}

    inline float operator[](int i) const { return e[i]; }
    inline float& operator[](int i) { return e[i]; }

    // To 3D
    inline Vec3f xyz() const { return Vec3f(x, y, z); }
};

using Point4f = Vec4f;

inline Vec4f toPoint4D(const Vec3f& p) {
    return Vec4f(p, 1.0f);
}

inline Vec4f toVec4D(const Vec3f& v) {
    return Vec4f(v, 0.0f);
}