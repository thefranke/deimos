/*
 * Deimos tool library - Tobias Alexander Franke 2003
 * For copyright and license see LICENSE
 * http://www.tobias-franke.eu
 */

#if !defined(DEIMOS_MATH_QUAT__)
#define DEIMOS_MATH_QUAT__

#include <cassert>
#include <cmath>
#include "vector.h"
#include "matrix.h"

namespace deimos {
namespace math {

template<typename T>
class Quaternion
{
	typedef Quaternion<T> Quat;
	typedef Matrix<T, 3> Mat3;
	typedef Matrix<T, 4> Mat4;
	typedef Vector<T, 3> Vec3;

protected:
	T x, y, z, w;

public:
	virtual ~Quaternion() {};

	Quat operator*(const Quat& op) const
	{
		Quat res;

		res.x = y*op.z - z*op.y + w*op.x + x*op.w;
		res.y = z*op.x - x*op.z + w*op.y + y*op.w;
		res.z = x*op.y - y*op.x + w*op.z + z*op.w;
		res.w = w*op.w - x*op.x - y*op.y - z*op.z;

		return res;
	};

	Mat3 unit_to_matrix() const
	{
		Mat3 res;

		res(0,0)=1-2*(y*y + z*z);	res(0,1)=2*(x*y - w*z);		res(0,2)=2*(w*y + x*z);
		res(1,0)=2*(x*y + w*z);		res(1,1)=1-2*(x*x + z*z);	res(1,2)=2*(y*z - w*x);
		res(2,0)=2*(x*z - w*y);		res(2,1)=2*(y*z + w*x);		res(2,2)=1-2*(x*x+y*y);

		return res;
	};

	Mat3 to_matrix() const
	{
		Mat3 res;

		res(0,0)=w*w+x*x-y*y-z*z;	res(0,1)=2*(x*y - w*z);		res(0,2)=2*(w*y + x*z);
		res(1,0)=2*(x*y + w*z);		res(1,1)=w*w-x*x+y*y-z*z;	res(1,2)=2*(y*z - w*x);
		res(2,0)=2*(x*z - w*y);		res(2,1)=2*(y*z + w*x);		res(2,2)=w*w-x*x-y*y+z*z;

		return res;
	};

	void from_matrix(Mat3 op)
	{
		assert(op.Trace() > 0);

		// TODO
	};

	void set(Vec3 axis, T angle)
	{
		T sinus = std::sin(angle/2);
		T cosinus = std::cos(angle/2);

		x = axis[0] * sinus;
		y = axis[1] * sinus;
		z = axis[2] * sinus;

		w = cosinus;
	};

	T length() const
	{
		return w*w+x*x+y*y+z*z;
	};

	void unit_invert()
	{
		x=-x;
		y=-y;
		z=-z;
	};

	void invert()
	{
		const T l = length();

		unit_invert();

		x/=l;
		y/=l;
		z/=l;
	};

	Quat inverse() const
	{
		Quat res = *this;
		res.invert();

		return res;
	};

};

} // namespace math
} // namespace deimos

#endif // !defined(DEIMOS_QUAT__)
