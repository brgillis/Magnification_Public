/**********************************************************************\
 @file differentiate.hpp
 ------------------

 Functions to differentiate a function pointer.

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

#ifndef _BRG_DIFFERENTIATE_HPP_INCLUDED_
#define _BRG_DIFFERENTIATE_HPP_INCLUDED_

#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "IceBRG_main/common.h"

#include "IceBRG_main/math/misc_math.hpp"
#include "IceBRG_main/math/safe_math.hpp"
#include "IceBRG_main/utility.hpp"
#include "IceBRG_main/vector/make_vector.hpp"

namespace IceBRG {

// Differentiates an arbitrary function numerically. The results are returned in the 2-D vector Jacobian, where Jacobian[i][j] represents the derivative of
// y_i with respect to x_j, with y_i being the output variables and x_j being the input variables, at the position labeled by in_params.
// In current implementation, the size of the differential used is a fraction of the input parameters (SMALL_FACTOR*in_params, where SMALL_FACTOR is defined
// in the brg_global.h header). If any in parameter is zero, the function uses the others as a guide for the size (and if using units, takes the units from
// the passed zero, so make sure your zeros have units if you do this!). If all in_params are zero, the function uses SMALL_FACTOR as the value. Be careful
// about this if evaluating a derivative at zero where the function changes on scales smaller than this - use a value slightly offset from zero instead.
//
// Parameters:
// order: Order of differentiation (1 = first derivative, 2 = second derivative, etc.). Order > 1 is NYI
// power: Used if, instead of the derivative of f(x), you want the derivative of (f(x))^2, for instance (set power = 2 for that), without setting up a
//        different function class.

// Scalar-in, scalar-out version
template< typename f, typename T >
inline T differentiate( const f * func, const T & in_param,
		const int_type order = 1, const flt_type & power = 1,
		const T & factor=SMALL_FACTOR )
{

	T d_in_param( 0 );
	T low_in_param( 0 );
	T high_in_param( 0 );
	T low_out_param( 0 );
	T high_out_param( 0 );
	T small_factor_with_units(factor);

	bool power_flag = false;
	bool zero_in_flag = false;

	int_type order_to_use = max( order, 1 );

	if ( ( order_to_use > 1 ) )
	{
		throw std::logic_error("IceBRG::differentiate with order > 1 is not currently supported.\n");
	}

	if ( power != 1 )
		power_flag = true;
	else
		power_flag = false;

	// Check if any in_params are zero. If so, estimate small factor from other in_params
	if ( in_param == 0 )
	{
		zero_in_flag = true;
	}
	else     // if(in_params==0)
	{
		small_factor_with_units = in_param * factor;
		d_in_param = small_factor_with_units;
	} // else

	if ( zero_in_flag )
	{
		if ( small_factor_with_units == 0 )
		{
			d_in_param = factor;
		}
		else
		{
			if ( in_param == 0 )
			{
				d_in_param = small_factor_with_units;
			} // if(in_params[i]==0)
		}
	}

	bool bad_function_result = false;
	int_type counter = 0;

	T Jacobian=0;

	do {
		counter++;
		bad_function_result = false;

		low_in_param = in_param - d_in_param;
		high_in_param = in_param + d_in_param;

		// Run the function to get value at test point
		try
		{
			low_out_param = ( *func )( low_in_param );
			high_out_param = ( *func )( high_in_param );
		}
		catch(const std::runtime_error &e)
		{
			bad_function_result = true;
			d_in_param /= 10; // Try again with smaller step
			continue;
		}

		// Record this derivative
		Jacobian = ( high_out_param - low_out_param ) / (2*d_in_param);
		if ( power_flag )
			Jacobian *= power * safe_pow( ( high_out_param + low_out_param )/2, power - 1 );
		if(isbad(Jacobian))
		{
			bad_function_result = true;
			d_in_param /= 10; // Try again with smaller step
			continue;
		}
	} while ((bad_function_result) && (counter<3));

	if(counter>=3)
		throw std::runtime_error("Cannot differentiate function due to lack of valid nearby points found.");

	return Jacobian;
}

// Vector-in, vector-out version
template< typename f, typename T >
inline std::vector< std::vector< T > > differentiate( const f * func, const std::vector< T > & in_params,
		const int_type order = 1, const flt_type & power = 1 )
{
	auto num_in_params = ssize(in_params);
	std::vector< std::vector< T > > Jacobian;

	std::vector< T > d_in_params( 0 );
	std::vector< T > base_out_params( 0 );
	std::vector< T > test_in_params( 0 );
	std::vector< T > test_out_params( 0 );
	T small_factor_with_units = SMALL_FACTOR;

	bool power_flag = false;
	bool zero_in_flag = false;

	int_type order_to_use = (int_type)max( order, 1 );

	if ( ( order_to_use > 1 ) )
	{
		throw std::logic_error("IceBRG::differentiate with order > 1 is not currently supported.\n");
	}

	if ( power != 1 )
		power_flag = true;
	else
		power_flag = false;

	// Delete std::vectors we'll be overwriting in case they previously existed
	Jacobian.clear();

	// Set up differentials
	make_vector_zeroes( d_in_params, num_in_params );
	test_in_params = in_params;

	// Check if any in_params are zero. If so, estimate small factor from other in_params
	for ( size_t i = 0; i < num_in_params; i++ )
	{
		if ( in_params[i] == 0 )
		{
			zero_in_flag = true;
		}
		else     // if(in_params[i]==0)
		{
			small_factor_with_units = in_params[i] * SMALL_FACTOR;
			d_in_params[i] = small_factor_with_units;
		} // else
	} // for( size_t i = 0; i < num_in_params; i++ )

	if ( zero_in_flag )
	{
		if ( small_factor_with_units == 0 )
		{
			// At least try to get the units right
			for ( size_t i = 0; i < num_in_params; i++ )
			{
#ifdef _BRG_USE_UNITS_
				d_in_params[i].set(SMALL_FACTOR,in_params[i].get_unit_powers());
#else
				d_in_params[i] = SMALL_FACTOR;
#endif
			} // for( size_t i = 0; i < num_in_params; i++ )
		}
		else
		{
			for ( size_t i = 0; i < num_in_params; i++ )
			{
				if ( in_params[i] == 0 )
				{
#ifdef _BRG_USE_UNITS_
					d_in_params[i].set(small_factor_with_units.get_value(),in_params[i].get_unit_powers());
#else
					d_in_params[i] = small_factor_with_units;
#endif
				} // if(in_params[i]==0)
			} // for( size_t i = 0; i < num_in_params; i++ )
		}
	}

	// Get value of function at input parameters
	base_out_params = ( *func )( in_params );
	auto num_out_params=ssize(base_out_params);

	// Set up Jacobian
	make_vector_default( Jacobian, num_out_params, num_in_params );

	// Loop over input and output dimensions to get Jacobian

	bool bad_function_result = false;
	int_type counter = 0;
	do {
		counter++;
		bad_function_result = false;
		for ( size_t j = 0; j < num_in_params; j++ )
		{
			// Set up test input parameters
			for ( size_t j2 = 0; j2 < num_in_params; j2++ )
			{
				if ( j2 == j )
				{
					test_in_params[j2] = in_params[j2] + d_in_params[j2];
				} // if( j2==j )
				else
				{
					test_in_params[j2] = in_params[j2];
				} // else
			}

			// Run the function to get value at test point
			try
			{
				test_out_params = ( *func )( test_in_params );
			}
			catch(const std::exception &e)
			{
				bad_function_result = true;
				for(ssize_t j=0; j< ssize(in_params); j++)
					d_in_params[j] /= 10; // Try again with smaller step size
				continue;
			}

			// Record this derivative
			for ( size_t i = 0; i < num_out_params; i++ )
			{
				Jacobian[i][j] = ( test_out_params[i] - base_out_params[i] )
						/ d_in_params[j];
				if ( power_flag )
					Jacobian[i][j] *= power
							* safe_pow( base_out_params[i], power - 1 );
				if(isbad(Jacobian[i][j]))
				{
					bad_function_result = true;
					for(size_t j=0; j< ssize(in_params); j++)
						d_in_params[j] /= 10; // Try again with smaller step size
					continue;
				}
			} // for( int_type i = 0; i < num_out_params; i++)
		} // for( size_t j = 0; j < num_in_params; j++)
	} while (bad_function_result && (counter<3));

	if(counter>=3)
		throw std::runtime_error("Cannot differentiate function due to lack of valid nearby points found.");

	return Jacobian;
}

}



#endif // _BRG_DIFFERENTIATE_HPP_INCLUDED_
