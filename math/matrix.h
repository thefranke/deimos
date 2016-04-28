/*
 * Deimos tool library - Tobias Alexander Franke 2003
 * For copyright and license see LICENSE
 * http://www.tobias-franke.eu
 *
 * Matrices are handled row-major!
 */

#if !defined(DEIMOS_MATH_MATRIX__)
#define DEIMOS_MATH_MATRIX__

#include <cassert>
#include "vector.h"

namespace deimos {
namespace math {

template<typename T, int S>
class Matrix
{
public:
	typedef Matrix<T, S> Mat;
	typedef Vector<T, S> Vec;
	Vec row_[S];

	void operator*=(const Mat& op)
	{
		*this = *this * op;
	};

	void operator*=(const float op)
	{
		for (int i=0; i < S; ++i)
			row_[i]*=op;
	};

	void operator+=(const Mat& op)
	{
		for(int i=0; i < S; ++i)
			row_[i]+=op[i];
	};

	void operator/=(const float op)
	{
		operator*=(1/op);
	};

	Vec operator*(const Vec& op) const
	{
		Vec res;

		for (int i=0; i < S; ++i)
			res[i] = row_[i] * op;

		return res;
	};

	Mat operator*(const Mat& op) const
	{
		Mat res;

		for (int i=0; i < S; ++i)
			for (int j=0; j < S; ++j)
				res.row_[i][j] = row_[i] * op.col(j);

		return res;
	};

	const Vec& operator[](int i) const
	{
		assert (i >= 0 && i < S);
		return row_[i];
	};

	Vec& operator[](int i)
	{
		assert(i >= 0 && i < S);
		return row_[i];
	};

	Vec col(int n) const
	{
		assert(n>=0 && n<S);

		Vec res;

		for (int i=0; i < S; ++i)
			res[i]=row_[i][n];
		return res;
	};

	void sqr()
	{
		*this*=*this;
	};

	void transpose()
	{
		Mat res;

		for (int i=0; i < S; ++i)
			res.row_[i]=col(i);

		*this = res;
	};

	T trace() const
	{
		T res = T();

		for(int i=0; i < S; ++i)
			res+=row_[i][i];

		return res;
	};

	void identity()
	{
		for (int i=0; i < S; ++i)
		{
			row_[i].clear();
			row_[i][i]=static_cast<T>(1);
		}
	};

	T* get_addr()
	{
		return &(row_[0][0]);
	};

    const T* get_addr() const
	{
		return &(row_[0][0]);
	};

	void clear()
	{
		for(int i=0; i < S; ++i)
			row_[i].clear();
	};
};

//-------------------------------------//

template<typename T>
T determinant(const Matrix<T, 3>& op)
{
	return	op[0][0]*op[1][1]*op[2][2] +
			op[0][1]*op[1][2]*op[2][0] +
			op[0][2]*op[1][0]*op[2][1] -
			op[0][2]*op[1][1]*op[2][0] -
			op[0][1]*op[1][0]*op[2][2] -
			op[0][0]*op[1][2]*op[2][1];
}



template<typename T>
T determinant(const Matrix<T, 2>& op)
{
	return	op[0][0]*op[1][1] -
			op[0][1]*op[1][0];
}



template<typename T>
Matrix<T, 2> invert(const Matrix<T, 2>& op)
{
	Matrix<T, 2> ret;

	ret[0][0]=op[1][1];
	ret[1][1]=op[0][0];
	ret[0][1]=-op[1][0];
	ret[1][0]=-op[0][1];

	T det(determinant(op));

	assert(det != 0);

	ret*=1/det;

	return ret;
}

} // namespace math
} // namespace deimos

#endif // DEIMOS_MATH_MATRIX__
