#pragma once
#include"stat_render/core/common.h"
#include"stat_render/core/Matrix.h"

class Transform {
public:
	Mat4f m;		// Object to World
	Mat4f mInv;		// World to Object

	Transform() { m = Mat4f::Identity(); mInv = Mat4f::Identity(); }
	Transform(const Mat4f& mat, const Mat4f& minv) : m(mat), mInv(minv) {}

	static Transform Translate(const Vec3f& delta);
	static Transform Scale(float x, float y, float z);
	static Transform Rotate(float angle, const Vec3f& axis);

	Transform operator*(const Transform& t2) const {
		return Transform(m * t2.m, t2.mInv * mInv);
	}

	Vec4f operator*(const Vec4f& v) const {
		return m * v;
	}
	
	Vec4f ApplyInverseTranspose(const Vec4f& vec) const {
		return mInv.transpose() * vec;
	}
};


Mat4f ViewTransform(const Point3f p, Vec3f g, Vec3f t);

Mat4f CameraToWorldTransform(const Point3f p, Vec3f g, Vec3f t);

Mat4f PerspectiveTransform();
