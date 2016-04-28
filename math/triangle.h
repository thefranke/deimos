/*
 * Deimos tool library - Tobias Alexander Franke 2004
 * For copyright and license see LICENSE
 * http://www.tobias-franke.eu
 */

#if !defined(DEIMOS_MATH_TRIANGLE__)
#define DEIMOS_MATH_TRIANGLE__

#include <cmath>
#include "vector.h"

namespace deimos {
namespace math {
namespace geometry {

template<typename T, int S>
class Triangle
{
public:
	typedef Vector<T, S> Vec;
	Vec vertex_[3], normal_[3];

	void calc_normal()
	{
		Vec t1, t2;

		t1=vertex_[1]-vertex_[0];
		t2=vertex_[2]-vertex_[0];

		// standard case: flat shading
		normal_[0] = normal_[1] = normal_[2] = cross_product(t1, t2);

		normal_[0].normalize();
		normal_[1].normalize();
		normal_[2].normalize();
	};
};

} // namespace geometry
} // namespace math
} // namespace deimos

#endif // DEIMOS_MATH_TRIANGLE__
