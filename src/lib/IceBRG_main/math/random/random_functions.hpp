/**********************************************************************\
  @file random_functions.hpp

 **********************************************************************

 Copyright (C) 2014  Bryan R. Gillis

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.

\**********************************************************************/

// body file: random_functions.cpp

#ifndef _BRG_RANDOM_FUNCTIONS_HPP_INCLUDED_
#define _BRG_RANDOM_FUNCTIONS_HPP_INCLUDED_

#include <cmath>
#include <random>

#include "IceBRG_main/common.h"
#include "IceBRG_main/math/misc_math.hpp"

namespace IceBRG
{

extern std::ranlux48 rng; // Initialised in random_functions.cpp

/** Global function declarations **/
#if (1)

// Generates a random int_type between min and max, inclusive of min, exclusive of max
template< typename T=int_type, typename T_in=int_type, typename T_gen=decltype(rng) >
T irand( T_in && min, T_in && max, T_gen & gen=rng )
{
	return std::uniform_int_distribution<T>(std::forward<T_in>(min),std::forward<T_in>(max)-1)(gen);
} // flt_type drand(flt_type min, flt_type max)

// Generates a random flt_type between min and max
template< typename T=flt_type, typename T_gen=decltype(rng) >
T drand( T_gen & gen=rng )
{
	return std::uniform_real_distribution<T>()(gen);
}
template< typename T=flt_type, typename T_in=flt_type, typename T_gen=decltype(rng) >
T drand( T_in && min, T_in && max, T_gen & gen=rng )
{
	return std::uniform_real_distribution<T>(std::forward<T_in>(min),std::forward<T_in>(max))(gen);
} // flt_type drand(flt_type min, flt_type max)

// Returns a random variable from a Gaussian distribution
template< typename T=flt_type, typename T_gen=decltype(rng) >
T Gaus_rand( T_gen & gen=rng )
{
	return std::normal_distribution<T>()(gen);
} // flt_type Gaus_rand()
template< typename T=flt_type, typename T_in=flt_type, typename T_gen=decltype(rng) >
T Gaus_rand( T_in && mean, T_in && stddev = 1.0, T_gen & gen=rng )
{
	return std::normal_distribution<T>(std::forward<T_in>(mean),std::forward<T_in>(stddev))(gen);

} // flt_type Gaus_rand(flt_type mean, flt_type stddev)

// Returns a random variable from a Gaussian distribution in log space
// Note that "mean" here is the desired mean, NOT the peak of the function (which differ in log space). If you want to use
// the peak, simply use the standard Gaus_rand version instead.
template< typename T=flt_type, typename T_gen=decltype(rng) >
T log10Gaus_rand( T_gen & gen=rng )
{
	const flt_type & fact = std::exp( -square( std::log( 10. ) ) / 2 );

	return ( fact * std::pow(10., Gaus_rand<T>(gen) ) );
} // flt_type log10Gaus_rand()
template< typename T=flt_type, typename T_in=flt_type, typename T_gen=decltype(rng) >
T log10Gaus_rand( T_in && mean, T_in && stddev = 1., T_gen & gen=rng )
{
	const flt_type & fact = std::exp( -square( stddev*std::log(10.) ) / 2 );

	return ( fact * std::pow(10., Gaus_rand<T,T_in>(std::forward<T_in>(mean),std::forward<T_in>(stddev),gen) ) );
} // flt_type log10Gaus_rand(flt_type mean, flt_type stddev)

// Returns a random variable from a Rayleigh distribution
template< typename T=flt_type, typename T_gen=decltype(rng) >
T Rayleigh_rand( T_gen & gen=rng )
{
	return std::sqrt(-2.*std::log(drand<T>(gen)));
}
template< typename T=flt_type, typename T_in=flt_type, typename T_gen=decltype(rng) >
T Rayleigh_rand( T_in && sigma, T_gen & gen=rng )
{
	return std::forward<T_in>(sigma)*Rayleigh_rand(gen);
}

// Returns a Poisson random variable.
template< typename T=int_type, typename T_gen=decltype(rng) >
T Pois_rand( T_gen & gen=rng )
{
	return std::poisson_distribution<T>()(gen);
} // T Pois_rand( T_gen & gen=rng )
template< typename T=int_type, typename T_in=flt_type, typename T_gen=decltype(rng) >
T Pois_rand( T_in && lambda=1., T_gen & gen=rng )
{
	return std::poisson_distribution<T>(std::forward<T_in>(lambda))(gen);
} // T Pois_rand( T_in && lambda=1., T_gen & gen=rng )

#endif // End global function declarations

}

#endif
