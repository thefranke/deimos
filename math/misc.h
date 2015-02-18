/* 
 * misc.h by Tobias Alexander Franke (tob@cyberhead.de) 2003 
 * For copyright and license see LICENSE
 */

#if !defined(DEIMOS_MATH_MISC__)
#define DEIMOS_MATH_MISC__

#include <iostream>

#include "matrix.h"
#include "vector.h"

namespace deimos {
namespace math {

	// constants
	namespace constants
	{
		const double PI			= 3.1415926535897932384626433832795;
		const double HALF_PI	= 1.5707963267948966192313216916395;
		const double DOUBLE_PI	= 6.283185307179586476925286766558;
		const double DEG_TO_RAD = 0.0174532925199432957692369076848861;
	};

	// converters
	template<typename T>
	T deg_to_rad(T x) { return static_cast<T>(x*constants::DEG_TO_RAD); };
	
	template<typename T>
	T rad_to_deg(T x) { return static_cast<T>(x/constants::DEG_TO_RAD); };

	// specialized functions for templates
	template<typename T, int S>
	void display(const Vector<T, S>& op)
	{
		std::cout << "(";
		for (int i=0; i < S; ++i)
		{
			std::cout << op[i];
			if (i != S-1)
				std::cout << ", ";
		}
		std::cout << ")" << std::endl;
	};

	template<typename T, int S>
	void display(const Matrix<T, S>& op)
	{
		for (int i=0; i < S; ++i)
			display<T, S>(op[i]);
	};

	// generic distance functions
	template<typename T, int S>
	T distance(const Vector<T, S>& op1, const Vector<T, S>& op2)
	{
		return (op2 - op1).Size();
	};
	
	// generic 3dim distance function
	template<typename T, int S>
	T distance_sqr_3(const Vector<T, S>& op1, const Vector<T, S>& op2)
	{
		assert(S >= 3);
		
		const T dist[3] = { op2[0]-op1[0], op2[1]-op1[1], op2[2]-op1[2] };
		
		return dist[0]*dist[0] + dist[1]*dist[1] + dist[2]*dist[2];
	};
	
	template<typename T, int S>
	T distance_3(const Vector<T, S>& op1, const Vector<T, S>& op2)
	{
		assert(S >= 3);
		
		return std::sqrt(distance_sqr_3(op1, op2));
	};

	// interpolation
	template<typename T>
	T interpolate_linear(const T& a, const T& b, float x)
	{
		assert(x >= 0.f && x <= 1.f);
		return a*(1-x) + b*x;
	};

	template<typename T>
	T interpolate_cosine(const T& a, const T& b, float x)
	{
		assert(x >= 0.f && x <= 1.f);
		double f = (1.f - cos(x * constants::PI)) * 0.5f;
		return a*(1-f) + b*f;
	};

	template<typename T>
	T interpolate_cubic(const T& v0, const T& v1, const T& v2, const T& v3, float x)
	{
		double P = (v3 - v2) - (v0 - v1);
		double Q = (v0 - v1) - P;
		double R = v2 - v0;
		double S = v1;

		return std::pow(P*x, 3) + std::pow(Q*x, 2) + R*x + S;
	};

	// boolean functions
	template<typename T>
	bool is_in_range(const T& val, const T& min, const T& max)
	{
		return (val >= min && val <= max);
	};

	// other functions
	template<typename T>
	int round(T val)
	{
		static int r = static_cast<int>(val);

		if (val - static_cast<int>(val) >= 0.5)
			return r+1;
		else
			return r;
	};

    template<typename T>
	T clamp(T value, T min, T max)
	{	
		if (value < min)
			return min;
		if (value > max)
			return max;
		return value;
	};

	static const double factorial_cache[99] = {
	1, 			  1, 			2, 
	6, 			  24, 			120, 
	720, 		  5040, 		40320, 
	362880, 	  3.6288e+006, 	3.99168e+007, 
	4.79002e+008, 6.22702e+009, 8.71783e+010, 
	1.30767e+012, 2.09228e+013, 3.55687e+014, 
	6.40237e+015, 1.21645e+017, 2.4329e+018, 
	5.10909e+019, 1.124e+021, 	2.5852e+022, 
	6.20448e+023, 1.55112e+025, 4.03291e+026, 
	1.08889e+028, 3.04888e+029, 8.84176e+030, 
	2.65253e+032, 8.22284e+033, 2.63131e+035, 
	8.68332e+036, 2.95233e+038, 1.03331e+040, 
	3.71993e+041, 1.37638e+043, 5.23023e+044, 
	2.03979e+046, 8.15915e+047, 3.34525e+049, 
	1.40501e+051, 6.04153e+052, 2.65827e+054, 
	1.19622e+056, 5.50262e+057, 2.58623e+059, 
	1.24139e+061, 6.08282e+062, 3.04141e+064, 
	1.55112e+066, 8.06582e+067, 4.27488e+069, 
	2.30844e+071, 1.26964e+073, 7.10999e+074, 
	4.05269e+076, 2.35056e+078, 1.38683e+080, 
	8.32099e+081, 5.0758e+083, 	3.147e+085, 
	1.98261e+087, 1.26887e+089, 8.24765e+090, 
	5.44345e+092, 3.64711e+094, 2.48004e+096, 
	1.71122e+098, 1.19786e+100, 8.50479e+101, 
	6.12345e+103, 4.47012e+105, 3.30789e+107, 
	2.48091e+109, 1.88549e+111, 1.45183e+113, 
	1.13243e+115, 8.94618e+116, 7.15695e+118, 
	5.79713e+120, 4.75364e+122, 3.94552e+124, 
	3.31424e+126, 2.8171e+128, 	2.42271e+130, 
	2.10776e+132, 1.85483e+134, 1.6508e+136, 
	1.48572e+138, 1.352e+140, 	1.24384e+142, 
	1.15677e+144, 1.08737e+146, 1.033e+148, 
	9.91678e+149, 9.61928e+151, 9.42689e+153 
	};

	template<typename T>
	T factorial(int number) 
	{
		if (number < 99)
			return static_cast<T>(factorial_cache[number]);

		T ret = static_cast<T>(factorial_cache[99]);
		const int stop = number - 99;
		for (int i = number; i > stop; --i)
			ret *= i;

		return ret;
	}

	template<typename T>
	T fact_frac(int x, int n)
	{
		// x!
		//---
		// n!

		if (x == n) 
			return 1;
		
		return static_cast<T>(factorial<T>(x)/factorial<T>(n));
	}

	// kronecker delta
	inline unsigned int delta(int a, int b)
	{
		return (a == b) ? 1 : 0;
	}

	// static kronecker delta
	template<int a, int b>
	struct delta_static
	{
		enum{ value = (a == b) ? 1 : 0 };
	};

} // namespace math
} // namespace deimos

#endif // DEIMOS_MATH_MISC__
