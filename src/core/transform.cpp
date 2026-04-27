#include"stat_render/core/transform.h"

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