#pragma once
#include"stat_render/core/common.h"
#include"stat_render/core/transform.h"
#include"Film.h"
class Camera 
{
private:
    Point3f pos;
    Vec3f gaze;
    Vec3f top;
    float aspect;
    float fov;      // deg
public:

    Camera(Point3f p, Vec3f r, Vec3f u, float asp = 1., float _fov = 90.) :
    pos(p), gaze(r), top(u), aspect(asp), fov(_fov) {}

    Camera() : 
    pos(Point3f(0.,0.,0.)),
    gaze(Vec3f(0., 0., -1.)),
    top(Vec3f(0., 1., 0.)),
    aspect(1.),
    fov(90.) {}

    

    Camera(const Film& film) : Camera(Point3f(0.,0.,0.), Vec3f(1., 0., 0.), Vec3f(0., 1., 0.), film.getWidth()/film.getHeight(), 90)  {}

    Camera(Point3f p, Vec3f g, Vec3f u, const Film& film, float fov_ = 90) :
    Camera(p, g, u, film.getWidth()/film.getHeight(), fov_) {}


    ~Camera() = default;

    Point3f getPosition() const { return pos; }
    float getAspect() const { return aspect; }
    float getFov() const { return fov; }
    Vec3f getTop() const {return top; }
    Vec3f getGaze() const
    { return gaze; }

    void transform(const Mat4f& M)
    {
        pos = (M * toPoint4D(pos)).xyz();
        gaze = (M * toVec4D(gaze)).xyz().normalized();
        top = (M * toVec4D(top)).xyz().normalized();
        return;
    }
};