/*
 * Deimos tool library - Tobias Alexander Franke 2005
 * For copyright and license see LICENSE
 * http://www.tobias-franke.eu
 */

#if !defined(DEIMOS_MATH_PLANE__)
#define DEIMOS_MATH_PLANE__

#include <cassert>

namespace deimos {
namespace math {
namespace geometry {

template<typename T, int S>
class Plane
{
public:
	typedef Vector<T, S> Vec;
	Vec normal_;
	T distance_;

	bool is_on_plane(const Vec& op) const
	{
		//std::assert(T > 2 && "2 or less dimensional plains contain all points!");

		return (op*normal_ == distance_);
	};
};

} // namespace geometry
} // namespace math
} // namespace deimos

#endif // DEIMOS_MATH_PLANE__
