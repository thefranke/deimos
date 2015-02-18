/* 
 * ray.h by Tobias Alexander Franke (tob@cyberhead.de) 2003 
 * For copyright and license see LICENSE
 */

#if !defined(DEIMOS_MATH_RAY__)
#define DEIMOS_MATH_RAY__

#include "Vector.h"

namespace deimos {
namespace math {
namespace geometry {

template <typename T, int S>
class Ray
{
public:
	typedef Vector<T, S> Vec;
	Vec	origin_;
	Vec direction_;

	bool is_on_ray(const Vec& op) const
	{
		// Works also, but only for 3 dimensions: 
		// CrossProduct(op - Point_, Direction_) == 0 ? return true : return false;
		return direction_.is_parallel_to(op - origin_);
	};

	Vec get_point_on_ray(T op) const
	{
		return (origin_ + (direction_ * op));
	};
	
};

} // namespace geometry
} // namespace math
} // namespace deimos

#endif // DEIMOS_MATH_RAY__


	
