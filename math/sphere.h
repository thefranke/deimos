/*
 * Deimos tool library - Tobias Alexander Franke 2004
 * For copyright and license see LICENSE
 * http://www.tobias-franke.eu
 */

#if !defined (DEIMOS_MATH_SPHERE__)
#define DEIMOS_MATH_SPHERE__

#include <cmath>
#include "Vector.h"

namespace deimos {
namespace math {
namespace geometry {

template <typename T, int S>
class Sphere
{
public:
	typedef Vector<T, S> Vec;
	Vec center_;
	T radius_;

	T distance_sqr(const Vec& op) const
	{
		return (center_ - op).size_sqr() - std::pow(radius_, 2);
	};

	T distance(const Vec& op) const
	{
		std::sqrt(distanceSqr(op));
	};

	bool is_on_sphere(const Vec& op) const
	{
		return distance_sqr(op) == 0;
	};

};

} // namespace geometry
} // namespace math
} // namespace deimos

#endif // DEIMOS_MATH_SPHERE__
