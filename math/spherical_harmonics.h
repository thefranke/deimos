/* 
 * spherical_harmonics.h by Tobias Alexander Franke (tob@cyberhead.de) 2006 
 * For copyright and license see LICENSE
 */

#if !defined(DEIMOS_MATH_SPHERICAL_HARMONICS__)
#define DEIMOS_MATH_SPHERICAL_HARMONICS__

#include <cmath>
#include <list>
#include <vector>

#include <boost/random.hpp>
#include <boost/function.hpp>
#include <boost/numeric/ublas/matrix_sparse.hpp>
#include <boost/numeric/ublas/io.hpp>

#include "misc.h"

namespace deimos {
namespace math {
namespace sh {

/*
 * Definitions & Usage
 * -------------------
 *
 * Spherical Coordinates: 
 *   0 <= theta < PI   for latitude   (north/south)
 *   0 <= phi   < 2PI  for longtitude (equator)
 */

// adapted from Numerical Methods in C
template<typename T>
T P(int l, int m, T x)
{
	T p_mm = 1.0;

    // if m > 0, generate highest possible band P_m^m with rule 2
	if (m > 0)
	{
		T somx2 = static_cast<T>(std::sqrt((1.0 - x) * (1.0 + x)));
        T fact = 1.0;
		for (int i = 1; i <= m; i++)
		{
			p_mm *= (-fact) * somx2;
			fact += 2.0;
		}
	}

	// m == l -> P_l^m == P_m^m (Rule 2)
	if (l == m)
		return p_mm;

	// m != l, raise band with rule 3
	T p_mmp1 = static_cast<T>(x * (2.0 * m + 1.0) * p_mm);

	// (Rule 3)
	if (l == m+1)
		return p_mmp1;

	// still m != l, evaluate with rule 1
	T p_ll = 0.0;

	for (int ll = m+2; ll <= l; ++ll)
	{
		p_ll = static_cast<T>( ( (2.0 * ll - 1.0)* x * p_mmp1 - (ll + m - 1.0) * p_mm ) / (ll-m) );
		p_mm = p_mmp1;
		p_mmp1 = p_ll;
	}

	return p_ll;
}

// spherical harmonics
template<typename T>
struct spherical
{
	typedef T base_type;

	static inline T K(int l, int m)
	{
		return static_cast<T>(std::sqrt( ((2.0 * l + 1.0)/(4.0 * constants::PI)) * fact_frac<T>(l-m, l+m)));
	}

	static T evaluate(int l, int m, T theta, T phi)
	{
		const T sqrt2 = static_cast<T>(std::sqrt(2.0));
		if (m == 0)
			return				K(l, m)	*							P<T>(l, m, std::cos(theta));
		else if (m > 0)
			return sqrt2	*	K(l, m)	* std::cos(m * phi)		*	P<T>(l, m, std::cos(theta));
		else
			return sqrt2	*	K(l, -m) * std::sin(-m * phi)	*	P<T>(l, -m, std::cos(theta));
	}
};

// zonal harmonics
template<typename T>
struct zonal
{
	typedef T base_type;

	static inline T evaluate(int l, int m, T theta, T phi)
	{
		return spherical<T>::evaluate(l, 0, theta, phi);
	}
};

// hemispherical harmonics
template<typename T>
struct hemispherical
{
	typedef T base_type;

	static T K(int l, int m)
	{
		return static_cast<T>(std::sqrt( ((2.0 * l + 1.0)/(2.0 * constants::PI)) * fact_frac<T>(l-m, l+m)));
	}

	static T hsh_P(int l, int m, T x)
	{
		return P<T>(l, m, x) * static_cast<T>(2.0 * x - 1.0);
	}

	static T evaluate(int l, int m, T theta, T phi)
	{
		const T sqrt2 = static_cast<T>(std::sqrt(2.0));
		if (m == 0)
			return				K(l, m)	*							hsh_P(l, m, std::cos(theta));
		else if (m > 0)
			return sqrt2	*	K(l, m)	* std::cos(m * phi)		*	hsh_P(l, m, std::cos(theta));
		else
			return sqrt2	*	K(l, -m) * std::sin(-m * phi)	*	hsh_P(l, -m, std::cos(theta));
	}
};

template<typename T>
struct sample
{
	T sph[2];
	T vec[3];
	std::vector<T> coefficents;
};

template<class B>
void create_spherical_samples(std::list< sample<typename B::base_type> >& samples, int num_samples, int num_bands)
{
	typedef typename B::base_type T;

	boost::mt19937 rng;

	boost::uniform_real<T> distribution;
	boost::variate_generator<boost::mt19937&, boost::uniform_real<T> > rand(rng, distribution);

	for (int s = 0; s < num_samples; ++s)
	{
		T x = rand();
		T y = rand();

		T theta = static_cast<T>(2.0 * std::acos(std::sqrt(1.0 - y)));
		T phi	= static_cast<T>(2.0 * constants::PI * x);

		sample<T> sample;

		sample.sph[0] = theta;
		sample.sph[1] = phi;

		sample.vec[0] = std::sin(theta) * std::cos(phi);
		sample.vec[1] = std::sin(theta) * std::sin(phi);
		sample.vec[2] = std::cos(theta);

		for (int l = 0; l < num_bands; ++l)
			for(int m = -l; m <= l; ++m)
				sample.coefficents.push_back(B::evaluate(l, m, theta, phi));

		samples.push_back(sample);
	}
}

// project polar function with a pregenerated list of samples using spherical parameterization
template<typename RetT, typename T>
void project_polar_function_sph(boost::function<RetT(T, T)> func, std::list< sample<T> >& samples, std::vector<RetT>& result)
{
	typedef std::list< sample<T> > sample_list;

	// at least one sample must be there, otherwise the projection makes no sense
	const int num_coeff = static_cast<int>((*samples.begin()).coefficents.size());

	const T weight = T(4.0 * constants::PI)/T(samples.size());
	sample_list::const_iterator end = samples.end();

	result.resize(num_coeff);

	// integrate sample values
	for (sample_list::iterator sample = samples.begin(); sample != end; ++sample)
	{
		const RetT func_val = func((*sample).sph[0], (*sample).sph[1]);
		for (int c = 0; c < num_coeff; ++c)
			result[c] += func_val * (*sample).coefficents[c];
	}

	for (int i = 0; i < num_coeff; ++i)
		result[i] = result[i] * weight;
}

// project polar function with a pregenerated list of samples using vector parameterization
template<typename RetT, typename T>
void project_polar_function_vec(boost::function<RetT(T, T, T)> func, std::list< sample<T> >& samples, std::vector<RetT>& result)
{
	typedef std::list< sample<T> > sample_list;

	// at least one sample must be there, otherwise the projection makes no sense
	const int num_coeff = static_cast<int>((*samples.begin()).coefficents.size());

	const T weight = T(4.0 * constants::PI)/T(samples.size());
	sample_list::const_iterator end = samples.end();

	result.resize(num_coeff);

	// integrate sample values
	for (sample_list::iterator sample = samples.begin(); sample != end; ++sample)
	{
		const RetT func_val = func((*sample).vec[0], (*sample).vec[1], (*sample).vec[2]);
		for (int c = 0; c < num_coeff; ++c)
			result[c] += func_val * (*sample).coefficents[c];
	}

	for (int i = 0; i < num_coeff; ++i)
		result[i] = result[i] * weight;
}

// project polar function using vector parameterization 
template<typename RetT, class B>
void project_polar_function_vec(boost::function<RetT(typename B::base_type, typename B::base_type, typename B::base_type)> func,
								int num_samples, int num_bands, std::vector<RetT>& result)
{
	typedef typename B::base_type T;

	boost::mt19937 rng;

	boost::uniform_real<T> distribution;
	boost::variate_generator<boost::mt19937&, boost::uniform_real<T> > rand(rng, distribution);

	const T weight = T(4.0 * constants::PI)/T(num_samples);
	const int num_coeff = num_bands * num_bands;

	result.resize(num_coeff);

	// integrate sample values
	for (int s = 0; s < num_samples; ++s)
	{
		T x = rand();
		T y = rand();

		T theta = static_cast<T>(2.0 * std::acos(std::sqrt(1.0 - y)));
		T phi	= static_cast<T>(2.0 * constants::PI * x);

		T vec[3];
		vec[0] = std::sin(theta) * std::cos(phi);
		vec[1] = std::sin(theta) * std::sin(phi);
		vec[2] = std::cos(theta);

		const RetT func_val = func(vec[0], vec[1], vec[2]);

		for (int l = 0; l < num_bands; ++l)
			for(int m = -l; m <= l; ++m)
			{
				int index = l*(l+1)+m;
				result[index] += func_val * B::evaluate(l, m, theta, phi);
			}
	}

	for (int i = 0; i < num_coeff; ++i)
		result[i] = result[i] * weight;
}

// project polar function using spherical parameterization 
template<typename RetT, class B>
void project_polar_function_sph(boost::function<RetT(typename B::base_type, typename B::base_type)> func, 
								int num_samples, int num_bands, std::vector<RetT>& result)
{
	typedef typename B::base_type T;

	boost::mt19937 rng;

	boost::uniform_real<T> distribution;
	boost::variate_generator<boost::mt19937&, boost::uniform_real<T> > rand(rng, distribution);

	const T weight = T(4.0 * constants::PI)/T(num_samples);
	const int num_coeff = num_bands * num_bands;
	
	result.resize(num_coeff);

	// integrate sample values
	for (int s = 0; s < num_samples; ++s)
	{
		T x = rand();
		T y = rand();

		T theta = static_cast<T>(2.0 * std::acos(std::sqrt(1.0 - y)));
		T phi	= static_cast<T>(2.0 * constants::PI * x);

		const RetT func_val = func(theta, phi);

		for (int l = 0; l < num_bands; ++l)
			for(int m = -l; m <= l; ++m)
			{
				int index = l*(l+1)+m;
				result[index] += func_val * B::evaluate(l, m, theta, phi);
			}
	}

	for (int i = 0; i < num_coeff; ++i)
		result[i] = result[i] * weight;
}

template<typename RetT, class B>
RetT reconstruct_function(typename B::base_type theta, typename B::base_type phi, std::vector<RetT>& result)
{
	// result coefficients must be at least 1 long
	const int num_bands = static_cast<int>(std::sqrt(static_cast<float>(result.size())));

	RetT ret = RetT();
	
	// multiply all base functions with coefficients
	for (int l = 0; l < num_bands; ++l)
		for(int m = -l; m <= l; ++m)
		{
			int index = l*(l+1)+m;
			ret += result[index] * B::evaluate(l, m, theta, phi);
		}

	return ret;
}

/**
 * for original definition see:
 * - Ivanic et al 1996, Rotation Matrices with Real SH
 * - Green 2003, SH: The Gritty Details
 * - Krivanik et al 2006, Fast Approximation to SH Rotation
 */
template<typename RetT, typename T, typename MatrixT>
class rotation_matrix
{
protected:
	T u_, v_, w_;

	// the sh matrix
	boost::numeric::ublas::compressed_matrix<T, boost::numeric::ublas::column_major> M_;

protected:
	void uvw(int l, int m, int n)
	{
		const unsigned int abs_m = std::abs(m);
		const int dm0 = delta(m, 0);
		const T div = (std::abs(n) < l) ? (l+n)*(l-n) : (2*l)*(2*l - 1);

		u_ = std::sqrt(((l+m)*(l-m))/div);
		v_ = std::sqrt(((1+dm0)*(l+abs_m-1)*(l+abs_m))/div) * 0.5 * (1 - 2*dm0);
		w_ = std::sqrt(((l-abs_m-1)*(l-abs_m))/div) * -0.5 * (1 - dm0);
	}

	T P(int i, int l, int a, int b) const
	{
		const int lp = l-1;
		const int ln = -l+1;

		if (b == -l)		return (R(i, 1) * M(lp, a, ln)) + (R(i, -1) * M(lp, a, lp));
		else if (b == l)	return (R(i, 1) * M(lp, a, lp)) - (R(i, -1) * M(lp, a, ln));
		else				return  R(i, 0) * M(lp, a, b);
	}

	T U(int l, int m, int n)
	{
		return P(0, l, m, n);
	}

	T V(int l, int m, int n)
	{
		const unsigned int dm1 = std::abs(m) == 1 ? 1 : 0;
		if (m == 0)		return P(1, l, 1, n) + P(-1, l, -1, n);
		else if (m > 0) return P(1, l, m-1, n) * std::sqrt(1.0 + dm1) - P(-1, l, -m+1, n) * (1 - dm1);
		else			return P(1, l, m+1, n) * (1 - dm1) + P(-1, l, -m-1, n) * std::sqrt(1.0 + dm1);
	}

	T W(int l, int m, int n)
	{
		if (m == 0)		{ assert(false && "spherical_harmonics.h: Undefined value"); return 0; }
		else if (m > 0)	return P(1, l, m+1, n) + P(-1, l, -m-1, n);
		else			return P(1, l, m-1, n) - P(-1, l, -m+1, n);
	}

	inline void set(int l, int m, int n, T value)
	{
		assert (std::abs(m) <= l && std::abs(n) <= l && "spherical_harmonics.h: set() submatrix indices are too big");
		const int index = l*(l+1);
		M_(n+index, m+index) = value;
	}

	// Green notation!
	inline T M(int l, int m, int n) const 
	{
		assert (std::abs(m) <= l && std::abs(n) <= l && "spherical_harmonics.h: M() submatrix indices are too big");
		const int index = l*(l+1);
		return M_(n+index, m+index);
	}

	inline T R(int m, int n) const
	{
		assert (std::abs(m) <= 1 && std::abs(n) <= 1 && "spherical_harmonics.h: R() submatrix indices are too big");
		return M_(n+2, m+2);
	}

	void build_more_bands(int num_bands)
	{
		for (int l = 2; l < num_bands; ++l)
		for (int m = -l; m <= l; ++m)
		for (int n = -l; n <= l; ++n)
		{
			uvw(l, m, n);
			
			u_ *= (u_ != 0) ? U(l, m, n) : 0;
			v_ *= (v_ != 0) ? V(l, m, n) : 0;
			w_ *= (w_ != 0) ? W(l, m, n) : 0;
		
			set(l, m, n, u_ + v_ + w_);
		}
	}

	rotation_matrix(const rotation_matrix&);
	rotation_matrix& operator=(const rotation_matrix&);

public:
	rotation_matrix(const MatrixT& rot_matrix, int num_bands, bool row_major = true)
	{
		const int s = num_bands * num_bands;
		M_.resize(s, s, false);

		// band 0
		set(0, 0, 0, 1);

		// band 1
		const size_t x = 0, y = 1, z = 2;

		if (row_major)
		{
			set(1, -1, -1, rot_matrix[y][y]);
			set(1, -1,	0, rot_matrix[y][z]);
			set(1, -1,	1, rot_matrix[y][x]);

			set(1,  0, -1, rot_matrix[z][y]);
			set(1,  0,  0, rot_matrix[z][z]);
			set(1,  0,  1, rot_matrix[z][x]);

			set(1,  1, -1, rot_matrix[x][y]);			
			set(1,  1,  0, rot_matrix[x][z]);			
			set(1,  1,  1, rot_matrix[x][x]);
		}
		else
		{
			set(1, -1, -1, rot_matrix[y][y]);
			set(1, -1,	0, rot_matrix[z][y]);
			set(1, -1,	1, rot_matrix[x][y]);

			set(1,  0, -1, rot_matrix[y][z]);
			set(1,  0,  0, rot_matrix[z][z]);
			set(1,  0,  1, rot_matrix[x][z]);

			set(1,  1, -1, rot_matrix[y][x]);			
			set(1,  1,  0, rot_matrix[z][x]);			
			set(1,  1,  1, rot_matrix[x][x]);
		}

		if (num_bands > 2)
			build_more_bands(num_bands);
	}

	boost::numeric::ublas::vector<RetT> operator*(const boost::numeric::ublas::vector<RetT>& coefficients) const
	{
		return boost::numeric::ublas::prod(M_, coefficients);
	}

	std::vector<RetT> operator*(const std::vector<RetT>& coefficients) const
	{
		std::vector<RetT> ret;

		const size_t num_coeff = coefficients.size();
		
		ret.resize(num_coeff);

		for (size_t i = 0; i < num_coeff; ++i)
		for (size_t j = 0; j < num_coeff; ++j)
			ret[i] += coefficients[j] * M_(i, j);

		return ret;
	}

	void rotate(std::vector<RetT>& coefficients) const
	{

	}
};

} // namespace sh
} // namespace math
} // namespace deimos

#endif // DEIMOS_MATH_SPHERICAL_HARMONICS__