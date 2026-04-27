#pragma once
#include"stat_render/core/common.h"
#include"stat_render/core/Matrix.h"


Mat4f ViewTransform(const Point3f p, Vec3f g, Vec3f t);

Mat4f CameraToWorldTransform(const Point3f p, Vec3f g, Vec3f t);

Mat4f PerspectiveTransform();
