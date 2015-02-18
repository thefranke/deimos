/* 
 * vector.h by Tobias Alexander Franke (tob@cyberhead.de) 2003 
 * For copyright and license see LICENSE
 */

#if !defined(DEIMOS_MATH_VECTOR__)
#define DEIMOS_MATH_VECTOR__

#include <cassert>
#include <cmath>

namespace deimos {
namespace math {

template<typename T, int S>
class Vector
{
public:
	typedef Vector<T, S> Vec;
	T element_[S];

	bool operator==(const Vec& op) const
	{
		for (int i=0; i < S; ++i)
			if (element_[i] != op.element_[i])
				return false;

		return true;
	};

	const Vec operator+(const Vec& op) const
	{
		Vec res;

		for (int i=0; i<S; ++i) 
			res.element_[i] = element_[i] + op.element_[i];

		return res;
	};

	const Vec operator-(const Vec& op) const
	{
		Vec res;

		for (int i=0; i<S; ++i) 
			res.element_[i] = element_[i] - op.element_[i]; 

		return res;
	};

	// Inner product	
	T operator*(const Vec& op) const
	{ 
		T res = T();

		for (int i=0; i<S; ++i) 
			res+=element_[i]*op.element_[i]; 

		return res; 
	};
	
	const Vec operator*(T op) const
	{
		Vec res;

		for (int i=0; i<S; ++i)
			res.element_[i] = element_[i] * op;

		return res;
	};
	
	const Vec operator/(T op) const
	{
		return operator*(1/op);
	};

	void operator*=(T op)
	{
		for (int i=0; i<S; ++i)
			element_[i]*=op;
	};

	void operator/=(T op)
	{
		operator*=(1/op);
	};

	void operator+=(const Vec& op)
	{
		for (int i=0; i<S; ++i)
			element_[i]+=op.element_[i];
	};

	void operator-=(const Vec& op)
	{
		for (int i=0; i<S; ++i)
			element_[i]-=op.element_[i];
	};

	inline const T& operator[](int n) const
	{ 
		assert(n >=0 && n < S); 

		return element_[n]; 
	};

	inline T& operator[](int n)
	{
		assert(n >=0 && n < S);

		return element_[n];
	};

	T size_sqr() const
	{
		return (*this * *this);
	};

	T size() const
	{
		return static_cast<T>(sqrt(size_sqr()));
	};

	void clear(T op=0)
	{
		for (int i=0; i < S; ++i)
			element_[i]=op;
	};

	void normalize()
	{
		assert(size());
		*this *= T(1/size());
	};

	Vec project_to(const Vec& op) const
	{
		return op * ((*this * op)/op.size_sqr());
	};

	bool is_parallel_to(const Vec& op) const
	{
		Vec temp(op.project_to(*this));

		return temp == op;
	};

	T* get_addr() 
	{
		return &(element_[0]);
	};

    const T* get_addr() const 
	{
		return &(element_[0]);
	};
};

template<typename T>
Vector<T, 3> cross_product(const Vector<T, 3>& op1, const Vector<T, 3>& op2)
{
	Vector<T, 3> res;

	res[0]=op1[1]*op2[2] - op1[2]*op2[1];
	res[1]=op1[2]*op2[0] - op1[0]*op2[2];
	res[2]=op1[0]*op2[1] - op1[1]*op2[0];

	return res;
};

template<typename T>
Vector<T, 4> cross_product(const Vector<T, 4>& op1, const Vector<T, 4>& op2)
{
	Vector<T, 4> res;

	res[0]=op1[1]*op2[2] - op1[2]*op2[1];
	res[1]=op1[2]*op2[0] - op1[0]*op2[2];
	res[2]=op1[0]*op2[1] - op1[1]*op2[0];
	res[3]=0;

	return res;
};

template<typename T, int S>
Vector<T, S> get_normal_angle(const Vector<T, S>& op1, const Vector<T, S>& op2)
{
	return std::acos(op1 * op2);
};

} // namespace math
} // namespace deimos

#endif // DEIMOS_MATH_VECTOR__

