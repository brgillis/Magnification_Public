/**********************************************************************\
 @file integrate.hpp
 ------------------

 Functions to be used for integrating functions in various manners.

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

#ifndef _BRG_INTEGRATE_HPP_INCLUDED_
#define _BRG_INTEGRATE_HPP_INCLUDED_

#include <cstdlib>
#include <cmath>
#include <utility>

#include "IceBRG_main/common.h"
#include "IceBRG_main/math/functor/functor_product.hpp"
#include "IceBRG_main/math/misc_math.hpp"
#include "IceBRG_main/math/safe_math.hpp"
#include "IceBRG_main/utility.hpp"
#include "IceBRG_main/units/units.hpp"

#ifndef ROMBERG_N_MAX
#define ROMBERG_N_MAX 20
#endif

namespace IceBRG
{

// Uses trapezoid-rule integration to estimate the integral of a function. Each output parameter is integrated independantly. For multiple input parameters,
// the function works iteratively, using the "passed" parameters seen at the end of the function. These parameters should not be entered by the user unless
// you're sure you know what you're doing. Due to the iterative nature of this function and the overhead involved, it may be unfeasibly slow for large-
// dimensional integration. In the case of num_in_param > ~4, Monte Carlo integration is typically superior.
// Note: For most smooth functions (of num_in_param < ~4), the integrate_Rhomberg function works better. This function is the superior choice for functions with // discontinuities cusps, corners, etc. It also has the benefit that the time_type spent is predefined by the input parameters, unlike the Rhomberg method which
// must find convergence, so there is no worry about facing a bizarre function which may take surprisingly long to integrate.
//
// Parameters:
// in_param_step: (first version only) The size of the steps used when integrating. Smaller is more accurate, but slower (order 1/in_param_step time_type).
// num_samples: (second version only) The number of steps used when integrating. Larger is more accurate, but slower (order num_samples time_type).
// num_passed_in_param & passed_in_param: Ignore these unless you know what you're doing.

// Scalar-in, scaler-out version.
template< typename f, typename T >
inline auto integrate_trapezoid( const f * func, const T & min_in_param, const T & max_in_param,
		const T & in_param_step) -> decltype(func(min_in_param)*min_in_param)
{
	typedef decltype(func(min_in_param)) Tfout;
	typedef decltype(Tfout()*min_in_param) Tout;

	Tout out_param( units_cast<Tout>(0.) ),
			temp_out_param( units_cast<Tout>(0.) ),
			last_out_param( units_cast<Tout>(0.) );

	T in_param( units_cast<T>(0.) );

	bool first_step = true;
	int_type num_steps;

	// Calculate number of steps for integration
	num_steps = (int_type)( ( max_in_param - min_in_param )
			/ safe_d( in_param_step ) ) + 1;

	// Standard trapezoid rule integration routine now

	for ( int_type i = 0; i < num_steps; i++ )
	{
		in_param = min_in_param + in_param_step * i;

		// If we have output param from last time_type, shift them to the last_out_param array
		last_out_param = temp_out_param;

		// Call function at this value
		temp_out_param = ( func )( in_param );

		if(first_step)
			first_step = false;
		else
			out_param += ( last_out_param + temp_out_param )
				* in_param_step / 2.;

	} // for( int_type i = 0; i < num_steps[0]; i++ )

	return out_param;
}

// Vector-in, vector-out version
template< typename f, typename T >
inline auto integrate_trapezoid( const f * func,
		const std::vector< T > & min_in_param,
		const std::vector< T > & max_in_param,
		const std::vector< T > & in_param_step,
		const std::vector< T > & passed_in_param = std::vector< T >( 0 ) ) ->
			decltype(multiply(func(min_in_param),min_in_param))
{
	std::vector< T > in_param( 0 );
	std::vector< T > new_min_in_param( 0 );
	std::vector< T > new_max_in_param( 0 );
	std::vector< T > new_in_param_step( 0 );
	std::vector< T > new_passed_in_param( 0 );
	std::vector< T > temp_out_param( 0 );
	std::vector< T > last_out_param( 0 );
	std::vector< T > out_param( 0 );

	auto num_in_param = ssize(min_in_param);
	auto num_passed_in_param = ssize(passed_in_param);
	auto num_out_param = ssize((func)(in_param,true));

	bool array_created = false; // So we can only create the out_param array once, after the first step
	std::vector< int_type > num_steps;
	int_type param_starting_index;
	int_type new_num_in_param = 0, new_num_passed_param = 0, num_tot_param =
			num_in_param + num_passed_in_param;

	// Check that we have a sane number of input parameters
	if ( ( num_in_param < 1 ) || ( num_in_param > MAX_STACK_DEPTH )
			|| ( num_in_param != ssize(min_in_param) )
			|| ( num_in_param != ssize(max_in_param) )
			|| ( num_in_param != ssize(in_param_step) )
			|| ( num_passed_in_param != ssize(passed_in_param) ) )
	{
		throw std::logic_error("Bad number of input param passed to integrate().");
	}

	make_array( num_steps, num_in_param );
	make_array( in_param, num_tot_param );

	// Delete out_param array if it exists
	del_array( out_param );

	// Calculate number of steps for integration
	for ( ssize_t i = 0; i < num_in_param; i++ )
	{
		num_steps[i] = (int_type)( ( max_in_param[i] - min_in_param[i] )
				/ safe_d( in_param_step[i] ) ) + 1;
	}

	// Were any param passed in from a previous iteration?
	if ( num_passed_in_param > 0 )
	{
		// Fill up in_param with the passed parameters
		for ( ssize_t j = 0; j < num_passed_in_param; j++ )
		{
			in_param[j] = passed_in_param[j];
		} // for( int_type j = 0; j < num_passed_param; j++ )
	} // if ( num_passed_param > 0 )

	param_starting_index = num_passed_in_param; // Set index for parameter we'll be integrating over

	if ( num_in_param == 1 )     // if (num_in_param < 1)
	{
		// Standard trapezoid rule integration routine now

		array_created = false;
		for ( int_type i = 0; i < num_steps[0]; i++ )
		{
			in_param[param_starting_index] = min_in_param[0]
					+ in_param_step[0] * i;

			// If we have output param from last time_type, shift them to the last_out_param array
			if ( !temp_out_param.empty() )
			{
				for ( ssize_t j = 0; j < num_out_param; j++ )
					last_out_param[j] = temp_out_param[j];
				del_array( temp_out_param );
			}

			// Call function at this value
			temp_out_param = ( func )( in_param );

			// Create output param arrays if necessary
			if ( !array_created )
			{
				make_array( out_param, num_out_param );
				make_array( last_out_param, num_out_param );
				array_created = true;
			} // If this is the first time_type, we don't do anything. Wait till next round to start adding in
			else
			{
				// Update the output parameters with those from the function call usind trapezoidal rule
				for ( ssize_t j = 0; j < num_out_param; j++ )
				{
					out_param[j] += ( last_out_param[j] + temp_out_param[j] )
									* in_param_step[0] / 2.;
				}

			}

		} // for( int_type i = 0; i < num_steps[0]; i++ )

	}
	else if ( num_in_param > 1 )   // else if(num_in_param == 1)
	{
		// In this case, we're going to have to iterate, calling the integration function for each step to integrate other dimensions

		// Set up new passed parameter array
		new_num_passed_param = num_passed_in_param + 1;
		new_num_in_param = num_in_param - 1;
		make_array( new_passed_in_param, new_num_passed_param );
		for ( ssize_t i = 0; i < num_passed_in_param; i++ )
			new_passed_in_param[i] = passed_in_param[i];

		// Set up new in-parameter arrays, excluding this first parameter
		make_array( new_min_in_param, num_in_param - 1 );
		make_array( new_max_in_param, num_in_param - 1 );
		make_array( new_in_param_step, num_in_param - 1 );
		for ( ssize_t i = 0; i < num_in_param - 1; i++ )
		{
			new_min_in_param[i] = min_in_param[i + 1];
			new_max_in_param[i] = max_in_param[i + 1];
			new_in_param_step[i] = in_param_step[i + 1];
		} // for( int_type i = 0; i < num_in_param-1; i++)

		array_created = false;
		for ( int_type i = 0; i < num_steps[param_starting_index]; i++ )
		{
			// Determine input param and add it to passed parameters array
			new_passed_in_param[new_num_passed_param - 1] =
					min_in_param[param_starting_index]
							+ in_param_step[param_starting_index] * i;

			// Call integrate on remaining in_param
			temp_out_param = integrate_trapezoid( func, new_min_in_param, new_max_in_param, new_in_param_step,
					new_passed_in_param );

			// Create output param array if necessary
			if ( !array_created )
			{
				make_array( out_param, num_out_param );
				array_created = true;
			}

			// Update the output parameters with those from the integrate call
			for ( ssize_t j = 0; j < num_out_param; j++ )
			{
				out_param[j] += temp_out_param[j]
						* in_param_step[param_starting_index];
			}
		}

	}
	else     // else if (num_in_param > 1)
	{
		throw std::runtime_error("Invalid path!");
	} // else

	return out_param;
}

// Scalar-in, scalar-out version
template< typename f, typename T >
inline auto integrate_trapezoid( const f * func, const T & min_in_param, const T & max_in_param,
		const int_type num_samples ) -> decltype(func(min_in_param)*min_in_param)
{
	T in_param_step( ( max_in_param - min_in_param )
				/ safe_d( num_samples - 1 ));
	return integrate_trapezoid( func, min_in_param, max_in_param,
			in_param_step );
}

// Vector-in, vector-out version
template< typename f, typename T >
inline auto integrate_trapezoid( const f * func,
		const std::vector< T > & min_in_param,
		const std::vector< T > & max_in_param, const int_type num_samples,
		const std::vector< T > & passed_in_param = std::vector< T >( 0 ) ) ->
			decltype(func(min_in_param))
{
	const typename std::vector<T>::size_type num_in_param(ssize(min_in_param));
	std::vector< T > in_param_step( num_in_param );
	for ( ssize_t i = 0; i < num_in_param; i++ )
	{
		in_param_step[i] = ( max_in_param[i] - min_in_param[i] )
				/ safe_d( num_samples - 1 );
	}
	return integrate_trapezoid( func, min_in_param, max_in_param,
			in_param_step, passed_in_param );
}

// Scalar-in, scalar-out version
template< typename f1, typename f2, typename T >
inline auto integrate_weighted_trapezoid( const f1 * func, const f2 * func_weight,
		const T & min_in_param, const T & max_in_param, const T & in_param_step )
			-> decltype(func(min_in_param))
{
	functor_product< f1, f2, T > fprod( func, func_weight );

	auto prod_out_param = integrate_trapezoid( fprod, min_in_param,
			max_in_param, in_param_step );
	auto weight_out_param = integrate_trapezoid( func_weight, min_in_param,
			max_in_param, in_param_step );

	return prod_out_param / safe_d( weight_out_param );
}

// Vector-in, vector-out version
template< typename f1, typename f2, typename T >
inline auto integrate_weighted_trapezoid( const f1 * func, const f2 * func_weight,
		const std::vector< T > & min_in_param,
		const std::vector< T > & max_in_param,
		const std::vector< T > & in_param_step,
		const std::vector< T > & passed_in_param = std::vector< T >( 0 ) ) ->
			decltype(func(min_in_param))
{
	functor_product< f1, f2, T > fprod( func, func_weight );

	auto prod_out_param = integrate_trapezoid( fprod, min_in_param,
			max_in_param, in_param_step, passed_in_param );
	auto weight_out_param = integrate_trapezoid( func_weight, min_in_param,
			max_in_param, in_param_step, passed_in_param );

	std::vector<T> out_param( ssize(prod_out_param) );
	for ( ssize_t i = 0; i < ssize(prod_out_param); i++ )
	{
		out_param[i] = prod_out_param[i] / safe_d( weight_out_param[i] );
	}

	return out_param;
}

// Scalar-in, scalar-out version
template< typename f1, typename f2, typename T >
inline auto integrate_weighted_trapezoid( const f1 * func, const f2 * func_weight,
		const T & min_in_param,
		const T & max_in_param, const int_type num_samples ) ->
			decltype(func(min_in_param))
{
	functor_product< f1, f2, T > fprod( func, func_weight );

	auto prod_out_param = integrate_trapezoid( fprod, min_in_param,
			max_in_param, num_samples );
	auto weight_out_param = integrate_trapezoid( func_weight, min_in_param,
			max_in_param, num_samples );

	return prod_out_param / safe_d( weight_out_param );
}

// Vector-in, vector-out version
template< typename f1, typename f2, typename T >
inline auto integrate_weighted_trapezoid( const f1 * func, const f2 * func_weight,
		const std::vector< T > & min_in_param,
		const std::vector< T > & max_in_param, const int_type num_samples,
		const std::vector< T > & passed_in_param = std::vector< T >( 0 ) ) ->
			decltype(func(min_in_param))
{
	functor_product< f1, f2, T > fprod( func, func_weight );

	auto prod_out_param = integrate( fprod, min_in_param,
			max_in_param, num_samples, passed_in_param );
	auto weight_out_param = integrate( func_weight, min_in_param,
			max_in_param, num_samples, passed_in_param );

	std::vector<T> out_param( ssize(prod_out_param) );
	for ( ssize_t i = 0; i < ssize(prod_out_param); i++ )
	{
		out_param[i] = prod_out_param[i] / safe_d( weight_out_param[i] );
	}

	return out_param;
}

// Monte-carlo integration method
template< typename f, typename T >
inline auto integrate_mc( const f * func,
		const std::vector< T > & min_in_param,
		const std::vector< T > & max_in_param, const int_type num_samples ) ->
			decltype(multiply(func(min_in_param),min_in_param))
{
	bool first_sample = true;

	auto test_in_param = drand(min_in_param,max_in_param);
	auto out_param = func(test_in_param);

	for(ssize_t i=0; i<num_samples-1; ++i )
	{
		test_in_param = drand(min_in_param,max_in_param);
		auto test_out_param = func(test_in_param);
		for(ssize_t j=0; j<ssize(test_out_param); ++j)
			out_param[j] += test_out_param[j];
	}

	for(ssize_t j=0; j<ssize(out_param); ++j)
		out_param[j] *= (max_in_param[j]-min_in_param[j])/num_samples;

	return out_param;
}

// Uses Romberg's rule to integrate a function. Each output parameter is integrated independently. For multiple input parameters,
// the function works iteratively, using the "passed" parameters seen at the end of the function. These parameters should not be entered by the user unless
// you're sure you know what you're doing. Due to the iterative nature of this function and the overhead involved, it may be unfeasibly slow for large-
// dimensional integration. In the case of num_in_param > ~4, Monte Carlo integration is typically superior.
// Note: As this integration rule estimates a polynomial form for a function in order to integrate, it has difficulty with functions that have discontinities,
// cusps, corners, etc. The integrate() function above is typically better in these cases.
//
// Parameters:
// precision: The threshold for determining convergence, by comparing the difference of two successive estimates of the integral to their mean. Smaller means
//            more accurate, but longer time_type to compute.
// loosen_precision: If num_in_param > 1 and this is set to true, the function will accept a lower precision threshold for the sub-integrals performed. This
//                   will dramatically increase speed of the function, but decrease accuracy (whether the trade-off is worth it should be investigated)
// num_passed_in_param & passed_in_param: Ignore these unless you know what you're doing.

// Scalar-in, scalar-out version.
template< typename f, typename Tin >
inline auto integrate_Romberg( const f & func,
		const Tin & min_in_param, const Tin & max_in_param, flt_type precision = 0.00001,
		bool tighten_precision = false ) -> decltype(func(min_in_param)*min_in_param)
{
	typedef decltype(func(min_in_param)) Tfout;
	typedef decltype(Tfout()*min_in_param) Tout;
	Tin in_param(0);
	std::vector< std::vector< Tout > > R( 0 );
	std::vector< Tout > Rn( 0 );
	Tout Rnm;
	Tin a0, b0;
	Tfout fa( units_cast<Tfout>(0.) ), fb( units_cast<Tfout>(0.) );
	Tfout ftot( units_cast<Tfout>(0.) );
	flt_type d;

	// Rhomberg's rule integration routine

	a0 = min_in_param;
	b0 = max_in_param;

	R.reserve(ROMBERG_N_MAX);

	// Get R[0][0] first
	fa = func( a0 );
	fb = func( b0 );

	Rnm = 0.5 * ( b0 - a0 ) * ( fa + fb );

	Rn.push_back( Rnm );
	R.push_back( Rn );
	Rn.resize( 0 );

	for ( int_type n = 1; n < ROMBERG_N_MAX; n++ )
	{
		Rn.reserve(n+1);

		// Get R[n][0]

		ftot = units_cast<Tfout>(0.);

		for ( int_type k = 1; k <= runtime_ipow( 2, n - 1 ); k++ )
		{
			in_param = a0 + (flt_type)( 2 * k - 1 ) * ( b0 - a0 ) / (flt_type)runtime_ipow( 2, n );
			ftot += ( func )( in_param );
		}

		Rnm = 0.5 * R[n - 1][0] + ( b0 - a0 ) / (flt_type)runtime_ipow( 2, n ) * ftot;

		Rn.push_back( Rnm );

		for ( int_type m = 1; m <= n; m++ )
		{
			Rnm = ( (flt_type)runtime_ipow( 4, m ) * Rn[m - 1] - R[n - 1][m - 1] )
					/ (flt_type)( runtime_ipow( 4, m ) - 1 );
			Rn.push_back( Rnm );
		}

		R.push_back( Rn );
		Rn.resize( 0 );

		// Check for convergence
		d = ( 2. * abs( R[n][n] - R[n - 1][n - 1] )
						/ safe_d( abs( R[n][n] + R[n - 1][n - 1] ) ) );
		if ( d < precision )
		{
			return R[n][n];
		}

	} // for(int_type n = 0; n < RHOMBERG_N_MAX; n++)

	throw std::runtime_error("integrate_Romberg did not converge.");
	return R.back().back();
}

// Vector-in, vector-out version
template< typename f, typename T >
inline auto integrate_Romberg( const f & func,
		const std::vector< T > & min_in_param,
		const std::vector< T > & max_in_param,
		const flt_type & precision = 0.00001,
		const bool tighten_precision = false,
		const std::vector< T > & passed_in_param = std::vector< T >( 0 ) ) ->
			decltype(multiply(func(min_in_param),min_in_param))
{
	std::vector< T > in_param( 0 );
	std::vector< T > new_in_param( 0 );
	std::vector< T > new_min_in_param( 0 );
	std::vector< T > new_max_in_param( 0 );
	std::vector< T > new_in_param_step( 0 );
	std::vector< T > new_passed_in_param( 0 );
	std::vector< T > temp_out_param( 0 );
	std::vector< T > last_out_param( 0 );
	std::vector< std::vector< std::vector< T > > > R( 0 );
	std::vector< std::vector< T > > Rn( 0 );
	std::vector< T > Rnm;
	std::vector< T > a( 0 ), b( 0 );
	T a0, b0;
	std::vector< T > fa( 0 ), fb( 0 ), ftot( 0 );
	T d;

	ssize_t param_starting_index;
	ssize_t num_in_param = ssize(min_in_param), num_passed_in_param = ssize(passed_in_param),
			new_num_in_param = 0, new_num_passed_param = 0, num_tot_param =
			num_in_param + num_passed_in_param;

	// Check that we have a sane number of input parameters
	if ( ( num_in_param < 1 ) || ( num_in_param > MAX_STACK_DEPTH )
			|| ( num_in_param != ssize(min_in_param) )
			|| ( num_in_param != ssize(max_in_param) )
			|| ( num_passed_in_param != ssize(passed_in_param) ) )
	{
		throw std::logic_error("Bad number of input param passed to integrate_Rhomberg().");
	}

	make_array( in_param, num_tot_param );

	// Were any param passed in from a previous iteration?
	if ( num_passed_in_param > 0 )
	{
		// Fill up in_param with the passed parameters
		for ( ssize_t j = 0; j < num_passed_in_param; j++ )
		{
			in_param[j] = passed_in_param[j];
		} // for( int_type j = 0; j < num_passed_param; j++ )
	} // if ( num_passed_param > 0 )

	param_starting_index = num_passed_in_param; // Set index for parameter we'll be integrating over

	if ( num_in_param < 1 ) // To catch errors that might have slipped through
	{
		throw std::runtime_error("Invalid path!");
	}
	else if ( num_in_param == 1 )     // if (num_in_param < 1)
	{
		// Rhomberg's rule integration routine now

		a0 = min_in_param[0];
		b0 = max_in_param[0];

		// Get R[0][0] first

		in_param[param_starting_index] = a0;
		fa = ( func )( in_param );

		in_param[param_starting_index] = b0;
		fb = ( func )( in_param );

		ssize_t num_out_param = ssize(fa);

		Rnm.resize( num_out_param );

		for ( ssize_t i = 0; i < num_out_param; i++ )
			Rnm[i] = 0.5 * ( b0 - a0 ) * ( fa[i] + fb[i] );

		Rn.push_back( Rnm );
		R.push_back( Rn );
		Rn.resize( 0 );

		for ( int_type n = 1; n < ROMBERG_N_MAX; n++ )
		{
			// Get R[n][0]

			make_array( ftot, num_out_param );
			for ( int_type k = 1; k <= runtime_ipow( 2, n - 1 ); k++ )
			{
				in_param[param_starting_index] = a0
						+ ( 2 * k - 1 ) * ( b0 - a0 ) / runtime_ipow( 2, n );
				( func )( in_param, temp_out_param );
				for ( ssize_t i = 0; i < num_out_param; i++ )
					ftot[i] += temp_out_param[i];
			}

			for ( ssize_t i = 0; i < num_out_param; i++ )
				Rnm[i] = 0.5 * R[n - 1][0][i]
						+ ( b0 - a0 ) / runtime_ipow( 2, n ) * ftot[i];

			Rn.push_back( Rnm );

			for ( int_type m = 1; m <= n; m++ )
			{
				for ( ssize_t i = 0; i < num_out_param; i++ )
					Rnm[i] = ( runtime_ipow( 4, m ) * Rn[m - 1][i]
							- R[n - 1][m - 1][i] ) / ( runtime_ipow( 4, m ) - 1 );
				Rn.push_back( Rnm );
			}

			R.push_back( Rn );
			Rn.resize( 0 );

			// Check for passed_in_paramconvergence
			d = 0;
			for ( ssize_t i = 0; i < num_out_param; i++ )
			{
				d = quad_add( d,( 2 * fabs( R[n][n][i] - R[n - 1][n - 1][i] )
							/ safe_d( fabs( R[n][n][i] + R[n - 1][n - 1][i] ) ) ) );
			}
			if ( d < precision )
			{
				return R[n][n];
				break;
			}

		} // for(int_type n = 0; n < RHOMBERG_N_MAX; n++)

	} // else if(num_in_param == 1)
	else if ( num_in_param > 1 )
	{
		// In this case, we're going to have to iterate, calling the integration function for each step to integrate other dimensions

		// Set up new passed parameter array
		new_num_passed_param = num_passed_in_param + 1;
		new_num_in_param = num_in_param - 1;
		flt_type new_precision;
		if ( tighten_precision )
			new_precision = std::pow( precision,
					(flt_type)num_in_param / new_num_in_param );
		else
			new_precision = precision;
		make_array( new_passed_in_param,
				new_num_passed_param );
		for ( ssize_t i = 0; i < num_passed_in_param; i++ )
			new_passed_in_param[i] = passed_in_param[i];

		// Set up new in-parameter arrays, excluding this first parameter
		make_array( new_min_in_param, num_in_param - 1 );
		make_array( new_max_in_param, num_in_param - 1 );
		for ( ssize_t i = 0; i < num_in_param - 1; i++ )
		{
			new_min_in_param[i] = min_in_param[i + 1];
			new_max_in_param[i] = max_in_param[i + 1];
		} // for( int_type i = 0; i < num_in_param-1; i++)

		a0 = min_in_param[param_starting_index];
		b0 = max_in_param[param_starting_index];

		// Determine input param and add it to passed parameters array
		new_passed_in_param[new_num_passed_param - 1] = a0;
		// Call integrate on remaining in_param
		fa = IceBRG::integrate_Romberg( func,
				new_min_in_param, new_max_in_param,
				new_precision, tighten_precision,
				new_passed_in_param );
		// Determine input param and add it to passed parameters array
		new_passed_in_param[new_num_passed_param - 1] = b0;
		// Call integrate on remaining in_param
		fb = IceBRG::integrate_Romberg( func,
				new_min_in_param, new_max_in_param,
				new_precision, tighten_precision,
				new_passed_in_param );

		ssize_t num_out_param = ssize(fa);

		Rnm.resize( num_out_param );

		for ( ssize_t i = 0; i < num_out_param; i++ )
			Rnm[i] = 0.5 * ( b0 - a0 ) * ( fa[i] + fb[i] );

		Rn.push_back( Rnm );
		R.push_back( Rn );
		Rn.resize( 0 );

		for ( int_type n = 1; n < ROMBERG_N_MAX; n++ )
		{
			// Get R[n][0]

			make_array( ftot, num_out_param );
			for ( int_type k = 1; k <= runtime_ipow( 2, n - 1 ); k++ )
			{
				new_passed_in_param[new_num_passed_param - 1] = a0
						+ ( 2 * k - 1 ) * ( b0 - a0 ) / runtime_ipow( 2., n );
				temp_out_param = IceBRG::integrate_Romberg( func,
						new_min_in_param,
						new_max_in_param,
						new_precision, tighten_precision,
						new_passed_in_param );
				for ( ssize_t i = 0; i < num_out_param; i++ )
					ftot[i] += temp_out_param[i];
			}

			for ( ssize_t i = 0; i < num_out_param; i++ )
				Rnm[i] = 0.5 * R[n - 1][0][i]
						+ ( b0 - a0 ) / runtime_ipow( 2, n ) * ftot[i];

			Rn.push_back( Rnm );

			for ( int_type m = 1; m <= n; m++ )
			{
				for ( ssize_t i = 0; i < num_out_param; i++ )
					Rnm[i] = ( runtime_ipow( 4, m ) * Rn[m - 1][i]
							- R[n - 1][m - 1][i] ) / ( runtime_ipow( 4, m ) - 1 );
				Rn.push_back( Rnm );
			}

			R.push_back( Rn );
			Rn.resize( 0 );

			// Check for convergence
			d = 0;
			for ( ssize_t i = 0; i < num_out_param; i++ )
			{
				d = quad_add( d,
						( 2 * fabs( R[n][n][i] - R[n - 1][n - 1][i] )
								/ fabs( R[n][n][i] + R[n - 1][n - 1][i] ) ) );
			}
			if ( d < precision )
			{
				return R[n][n];
			}
		}

	}
	else     // else if (num_in_param > 1)
	{
		throw std::runtime_error("Invalid path!");
	} // else

	throw std::runtime_error("Integrate_Romberg did not converge.");

	return R.back().back();
}

// Scalar-in, scalar-out version
template< typename f_in_1, typename f_in_2, typename T >
inline auto integrate_product_Romberg( const f_in_1 & func1,
		const f_in_2 & func2, const T & min_in_param, const T & max_in_param,
		const flt_type & precision = 0.00001, const bool tighten_precision = false ) ->
			decltype(func1(min_in_param)*func2(min_in_param)*min_in_param)
{
	functor_product< f_in_1, f_in_2, T > fprod( func1, func2 );

	return integrate_Romberg( fprod, min_in_param, max_in_param,
			precision, tighten_precision );
}

// Vector-in, vector-out version
template< typename f_in_1, typename f_in_2, typename T >
inline auto integrate_product_Romberg( const f_in_1 & func1,
		const f_in_2 & func2, const std::vector< T > & min_in_param,
		const std::vector< T > & max_in_param, const flt_type & precision = 0.00001,
		const bool tighten_precision = false,
		const std::vector< T > & passed_in_param = std::vector< T >( 0 ) ) ->
			decltype(multiply(multiply(func1(min_in_param),func2(min_in_param)),min_in_param))
{
	functor_product< f_in_1, f_in_2, T > fprod( func1, func2 );

	return integrate_Romberg( fprod, min_in_param, max_in_param,
			precision, tighten_precision, passed_in_param );
}

// Scalar-in, scalar-out version
template< typename f_in_1, typename f_in_2, typename T >
inline auto integrate_weighted_Romberg( const f_in_1 & func,
		const f_in_2 & func_weight, const T & min_in_param, const T & max_in_param,
		const flt_type & precision = 0.00001, const bool tighten_precision = false ) ->
			decltype(func(min_in_param))
{
	functor_product< f_in_1, f_in_2, T > fprod( func, func_weight );

	auto prod_out_param = integrate_Romberg( fprod, min_in_param, max_in_param,
			precision, tighten_precision );
	auto weight_out_param = integrate_Romberg( func_weight, min_in_param, max_in_param,
			precision, tighten_precision );

	auto res = prod_out_param / safe_d( weight_out_param );
	return res;
}

// Vector-in, vector-out version
template< typename f_in_1, typename f_in_2, typename T >
inline auto integrate_weighted_Romberg( const f_in_1 & func,
		const f_in_2 & func_weight,	const std::vector< T > & min_in_param,
		const std::vector< T > & max_in_param, const flt_type & precision = 0.00001,
		const bool tighten_precision = false,
		const std::vector< T > & passed_in_param = std::vector< T >( 0 ) ) ->
			decltype(func(min_in_param))
{
	functor_product< f_in_1, f_in_2, T > fprod( func, func_weight );

	auto prod_out_param = integrate_Romberg( fprod, min_in_param, max_in_param,
			precision, tighten_precision, passed_in_param );
	auto weight_out_param = integrate_Romberg( func_weight, min_in_param, max_in_param,
			precision, tighten_precision, passed_in_param );

	decltype(func(min_in_param)) out_param( ssize(prod_out_param) );
	for ( ssize_t i = 0; i < ssize(prod_out_param); i++ )
	{
		out_param[i] = prod_out_param[i] / safe_d( weight_out_param[i] );
	}

	return out_param;
}

} // namespace IceBRG

#endif // _BRG_INTEGRATE_HPP_INCLUDED_
