/* 
 * intersect.h by Tobias Alexander Franke (tob@cyberhead.de) 2003 
 * For copyright and license see LICENSE
 */

#if !defined(DEIMOS_MATH_INTERSECT__)
#define DEIMOS_MATH_INTERSECT__

#include <cmath>
#include <cassert>

#include "triangle.h"
#include "sphere.h"
#include "plane.h"
#include "misc.h"

namespace deimos {
namespace math {
namespace geometry {
	
	namespace {
		static const float delta = .001f;
	}

/*
 * All intersect routines use normalized half-rays and return a positive direction
 */

template<typename T> 
struct intersection_point
{
    Vector<T, 4>	pos_;
    Vector<T, 4>	normal_;
	T				distance_;
    bool			valid_;
};

template<typename T>
intersection_point<T> intersect(const Triangle<T,4>& triangle, const Ray<T,4>& ray)
{
	static T t, u, v, det, invdet;
	static Vector<T,4> v31, v21, r0v1, cross1, cross2;
	static intersection_point<T> result;
	result.valid_ = false;

	v31 = triangle.vertex_[2] - triangle.vertex_[0];
	v21 = triangle.vertex_[1] - triangle.vertex_[0];
	
	cross2 = cross_product(ray.direction_, v31);
	det = cross2*v21;

	if (is_in_range(det, -delta, delta)) 
        return result; 

	r0v1 = ray.origin_ - triangle.vertex_[0];
	cross1 = cross_product(r0v1, v21);

	assert(det!=0);
	invdet = 1/det;

	u = invdet*(cross2*r0v1);
	if (!is_in_range(u, 0.0f, 1.0f))
		return result;

	v = invdet*(cross1*ray.direction_);
	if (u+v > 1 || v < 0)
		return result;

	t = invdet*(cross1*v31);
	if (t < 0)
		return result;

	v21 = interpolate_linear(triangle.normal_[0], triangle.normal_[1], u);

	v31 = interpolate_linear(triangle.normal_[0], triangle.normal_[2], v);

	result.normal_ = (v31+v21);
		
	result.normal_.normalize(); 
	result.pos_ = ray.origin_ + ray.direction_*t; 
	result.distance_ = t;
	result.valid_ = true; 

	return result;
}

template<typename T>
intersection_point<T> intersect(const Sphere<T,4>& sphere, const Ray<T,4>& ray)
{
	static intersection_point<T> result;
	result.valid_ = false;

	Vector<T, 4> l = sphere.center_ - ray.origin_;
	T lsqr(l.size_sqr());
	T rsqr(std::pow(sphere.radius_, 2));
	T d = l*ray.direction_;

	// sphere behind ray origin and ray direction points into opposite
	if (d < 0 && lsqr > rsqr)
		return result;

	// ray misses sphere (ray is not covered by radius)
	T msqr(lsqr - std::pow(d,2));
	if (msqr > rsqr)
		return result;

	T t = T();
	T q(sqrt(rsqr - msqr));
	if (lsqr > rsqr)
		t = d - q;
	else
		t = d + q;

	result.pos_ = ray.get_point_on_ray(t);
	result.distance_ = t;
	result.normal_ = result.pos_ - sphere.center_;
	result.normal_.normalize();
	result.valid_ = true;  
	return result;
}

} // namespace geometry
} // namespace math
} // namespace deimos

#endif // DEIMOS_MATH_INTERSECT__
