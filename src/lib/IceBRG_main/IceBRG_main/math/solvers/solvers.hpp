/**********************************************************************\
  @file solvers.hpp

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

#ifndef _BRG_SOLVERS_HPP_INCLUDED_
#define _BRG_SOLVERS_HPP_INCLUDED_

#include "IceBRG_main/common.h"

#include <cstdlib>
#include <cmath>
#include <type_traits>
#include <utility>

#include "IceBRG_main/container/is_container.hpp"
#include "IceBRG_main/container/generic_functions.hpp"
#include "IceBRG_main/container/tuple.hpp"
#include "IceBRG_main/math/misc_math.hpp"
#include "IceBRG_main/math/random/random_functions.hpp"
#include "IceBRG_main/units/units.hpp"
#include "IceBRG_main/utility.hpp"
#include "IceBRG_main/vector/elementwise_functions.hpp"
#include "IceBRG_main/vector/summary_functions.hpp"

namespace IceBRG
{

// Various methods to solve a function (a child of function_class with an actual function defined must be used):

// solve_iterate: Only valid for functions of the form x = f(x). The defined function should be the f(x), and the solution
// value should be the x where f(x)=x. Note that this solver cannot solve all functions of this form, particularly when the
// f(x) is undefined for certain input values which can occur as output.
//
// Parameters:
// slowdown: Makes the solver slower to converge, but more likely to converge, by using the new test value as the mean of the
//           most recent output and the slowdown last inputs.
// precision: How close successive values must be to each other (in a fraction of the mean) before they're said to have converged.
//            Smaller values make the output more precise, but make the solver take longer.
// max_counter: Maximum number of loops the solver can take before it gives up.
template< typename f, typename T, BRG_F_NOT_CONTAINER(T) >
inline const T solve_iterate( const f & func, const T &init_param = 0,
		const int_type slowdown = 1, const flt_type & precision = 0.0001,
		const int_type max_counter = 10000 )
{
	bool converged_flag;
	int_type counter = 0;

	T new_value = init_param;
	T mean_value = 0;
	std::vector< T > past_values( 0 );
	int_type slowdown_to_use;

	if ( slowdown < 0 )
		slowdown_to_use = 0;
	else
		slowdown_to_use = slowdown;
	past_values.resize( slowdown_to_use + 1, 0 );

	while ( counter < max_counter )
	{
		counter++;

		// Shift back values
		for ( int_type i = slowdown_to_use; i > 0; i-- )
		{
			past_values[i] = past_values[i - 1];
		}
		past_values[0] = new_value;

		// Find new_value, based on standard iteration
		new_value = ( func )( new_value );

		// First check if we have an equality. Likely will only happen if zero is a solution
		if ( new_value == past_values[0] )
		{
			counter = 0;
			break;
		}

		if ( slowdown_to_use > 0 )
		{
			mean_value = 0;
			mean_value += new_value / (flt_type)min( slowdown_to_use + 1, counter + 1 );
			for ( int_type i = 0; i < min( slowdown_to_use, counter ); i++ )
				mean_value += past_values[i]
						/ (flt_type)min( slowdown_to_use + 1, counter + 1 );
			new_value = mean_value;
		}

		converged_flag = true;
		// Check for convergence
		for ( int_type i = 0; i < slowdown_to_use + 1; i++ )
		{
			if ( abs(
					2. * ( new_value - past_values[i] )
							/ safe_d( new_value + past_values[i] ) )
					> precision )
			{
				converged_flag = false;
				break;
			}
		}

		if ( converged_flag )
		{
			counter = 0;
			break;
		}
	}

	if ( counter != 0 )
	{
		throw std::runtime_error("solve_iterate did not converge.");
	}
	return new_value;
}

// solve_sd: Steepest-descent solver. Finds the minimum value of a function. May fail if it reaches input values which give undefined
//           output. Only valid for functions which return only one output value. Returns the best set of input parameters in the
//           result_in_params vector.
//
// Parameters:
// precision: How close successive values must be to each other (in a fraction of the mean) before they're said to have converged.
//            Smaller values make the output more precise, but make the solver take longer.
// lambda: Relative size of the step size. This implementation scales lambda by the derivative at the starting point over the value
//         at the starting point, so less guesswork is needed in choosing a proper value. Be careful about setting lambda to 1 though,
//         as this will often lead the the function stepping immediately to an input of zero, which will be undefined for many functions.
// cusp_override_power: Normally, a steepest-descent algorithm can only handle functions where the derivative is zero at the minimum. To
//                      handle functions where the derivative is undefined, try changing this value. If the function is expected to have
//                      a corner at the minimum (for instance, you took the absolute value of the distance_type from a target value), set this
//                      to one. The program will then solve the square of the input function instead. If the function is expected to reach
//                      a cusp at the minimum, set this value to some higher value (the specific value needed will depend on the order of
//                      the cusp).
// max_steps: Maximum number of steps the solver can take before it gives up.
//
// WARNING: This function hasn't been tested in a long while; it may no longer work.
template< typename f, typename T, BRG_F_NOT_CONTAINER(T) >
T solve_sd( const f & func, const T & init_in_params,
		const flt_type & precision = 0.00001, const flt_type & lambda = 0.1,
		const flt_type & cusp_override_power = 0, const int_type max_steps = 10000)
{
	T Jacobian( 0 );
	T current_in_params( 0 );
	T last_in_params( 0 );
	T in_params_to_use = init_in_params;
	T lambda_norm( 0 );
	T mag;

	const int_type lambda_shortening_intervals = 10;
	const flt_type & lambda_shortening_factor = 10;
	int_type num_out_params = 0;
	int_type num_steps_taken = 0;
	bool converged_flag = false;

	// Sanity checks on variables
	const flt_type & lambda_to_use = max( SMALL_FACTOR,
			min( 1 / SMALL_FACTOR, lambda ) );
	const int_type max_steps_to_use = (int_type)max( 1, max_steps );

	// Get normalized step size
	lambda_norm = lambda_to_use / safe_d(differentiate( func, in_params_to_use, 1, cusp_override_power ));

	if ( in_params_to_use != 0 )
		lambda_norm *= in_params_to_use;

	// Initialize solver
	num_steps_taken = 0;
	current_in_params = in_params_to_use;

	// Solver

	while ( num_steps_taken < max_steps_to_use )
	{
		num_steps_taken++;
		last_in_params = current_in_params;

		// Get derivative at current location
		Jacobian = differentiate( func,	current_in_params, 1, cusp_override_power );

		// Make a step
		current_in_params -= lambda_norm * Jacobian;

		// Check if we've converged
		converged_flag = true;

		if ( isbad( current_in_params ) )
		{
			throw std::runtime_error("Got bad value for in_params in solve_sd.");
		}
		if ( abs(
				2 * ( current_in_params - last_in_params )
						/ safe_d( current_in_params + last_in_params ) )
				> precision )
		{
			converged_flag = false;
			break;
		}

		// In case we get to high steps, start decreasing lambda_norm
		if ( divisible( num_steps_taken,
				max_steps_to_use / lambda_shortening_intervals ) )
		{
			lambda_norm /= lambda_shortening_factor;
		}
	}

	// Check if we converged
	if ( !converged_flag )
	{
		throw std::runtime_error("solve_sd did not converge.");
	}
	return current_in_params;

}

// Vector-in, vector-out version
//
// WARNING: This function hasn't been tested in a long while; it may no longer work.
template< typename f, typename T, BRG_F_NOT_CONTAINER(T) >
std::vector< T > solve_sd( const f & func, const std::vector< T > & init_in_params,
		const flt_type & precision = 0.00001,
		const flt_type & lambda = 0.1, const flt_type & cusp_override_power = 0,
		const int_type max_steps = 10000)
{
	std::vector< std::vector< T > > Jacobian( 0 );
	std::vector< T > current_in_params( 0 );
	std::vector< T > last_in_params( 0 );
	std::vector< T > out_params( 0 );
	std::vector< T > in_params_to_use = init_in_params;
	T lambda_norm( 0 );
	T mag;
	typedef typename std::vector<T>::size_type vsize_t;
	vsize_t num_in_params = ssize(init_in_params);

	const int_type lambda_shortening_intervals = 10;
	const flt_type & lambda_shortening_factor = 10;
	vsize_t num_out_params = 0;
	int_type num_steps_taken = 0;
	bool converged_flag = false;

	// Sanity checks on variables
	assert(num_in_params >= 1);
	const flt_type & lambda_to_use = max( SMALL_FACTOR,
			min( 1 / SMALL_FACTOR, lambda ) );
	const int_type max_steps_to_use = (int_type)max( 1, max_steps );

	// Get normalized step size
	Jacobian = differentiate( func, in_params_to_use, 1, cusp_override_power );
	mag = 0;
	for ( vsize_t i = 0; i < num_in_params; i++ )
	{
		mag = quad_add( mag, Jacobian[0][i] );
	}
	lambda_norm = lambda_to_use / mag;
	mag = 0;
	for ( vsize_t i = 0; i < num_in_params; i++ )
	{
		mag = quad_add( mag, in_params_to_use[i] );
	}
	if ( mag == 0 )
		mag = 1;
	lambda_norm *= mag;

	// Initialize solver
	num_steps_taken = 0;
	current_in_params = in_params_to_use;

	// Solver

	while ( num_steps_taken < max_steps_to_use )
	{
		num_steps_taken++;
		last_in_params = current_in_params;

		// Get derivative at current location
		Jacobian = differentiate( func, current_in_params, 1, cusp_override_power );

		// Make a step
		for ( vsize_t i = 0; i < num_in_params; i++ )
		{
			current_in_params[i] -= lambda_norm * Jacobian[0][i];
		}

		// Check if we've converged
		converged_flag = true;

		for ( vsize_t i = 0; i < num_in_params; i++ )
		{
			if ( isbad( current_in_params[i] ) )
			{
				throw std::runtime_error("ERROR: Somehow got NaN for in_params in solve_sd.");
			}
			if ( abs(
					2 * ( current_in_params[i] - last_in_params[i] )
							/ safe_d(
									current_in_params[i]
											+ last_in_params[i] ) )
					> precision )
			{
				converged_flag = false;
				break;
			}
		}

		if ( converged_flag )
			break;

		// In case we get to high steps, start decreasing lambda_norm
		if ( divisible( num_steps_taken,
				max_steps_to_use / lambda_shortening_intervals ) )
		{
			lambda_norm /= lambda_shortening_factor;
		}
	}

	// Check if we converged
	if ( !converged_flag )
	{
		throw std::runtime_error("solve_sd did not converge.");
	}
	return current_in_params;

}

// solve_grid: Custom solver which searches a grid of input parameters for the ones which give output closest to the target. The best point is then
//             used as the starting point for a narrowing search on progressively finer grids of 3^n points (n = number of input parameters).
//             This solver has the advantage that it can better handle regions of input space which give undefined results, and it is less likely to
//             get stuck in local minima. It is, however, slower than other methods, especially for high numbers of input parameters.
//             This function is set up to handle multiple output parameters. If it is more important that one be close to the target than another,
//             use the out_params_weight vector to assign it a higher weight.
//
// Parameters:
// min/max_in_params: Limits to the search space for a solution
// search_in_params_step: (first version only) Step size used in the initial grid search for the optimum position
// num_search_steps: (second version only) Number of steps to use in the initial grid search (usually around 10 is good).
// target_out_params: Desired output parameters.
// precision: How small must the step size be relative to the steps in the initial grid search before the search is ended.
// search_precision: In the event the initial grid search fails to find any values that return defined results, the algorithm will try one more
//                   time_type after shrinking the grid step size by this factor.
// out_params_weight: If there are multiple output parameters, this represents how relatively important it is that each be close to the target
//                    value.

// Scalar-in, scalar-out version
template< typename f, typename Tin, typename Tout, BRG_F_NOT_CONTAINER(Tin), BRG_F_NOT_CONTAINER(Tout) >
Tin solve_grid( const f & func, const Tin & init_min_in_param, const Tin & init_max_in_param,
		const Tin & init_init_in_params_step, const Tout & target_out_param,
		const flt_type & init_init_precision = 0.00001, const flt_type & search_precision = 0.1 )
{

	Tout d = 0, d_best = units_cast<Tout>(std::numeric_limits<flt_type>::max());
	int_type i_best = -1;
	Tin init_in_param_step( units_cast<Tin>(0.) );
	Tin in_param_step( units_cast<Tin>(0.) );
	Tin test_in_param( units_cast<Tin>(0.) );
	Tin best_in_param( units_cast<Tin>(0.) );
	Tout test_out_param( units_cast<Tout>(0.) );
	Tin min_in_param = init_min_in_param, max_in_params =
			init_max_in_param;

	const int_type default_step_number = 10;
	const flt_type & grid_shortening_factor = 0.5;

	flt_type init_precision = init_init_precision, precision = init_precision;
	flt_type step_dist = 1;
	int_type num_test_points = 0;
	int_type num_in_params_steps( 0 );

	// Check for sanity and set up parameters
	if ( max_in_params < min_in_param )
		std::swap( max_in_params, min_in_param );
	if ( init_init_in_params_step < units_cast<Tin>(0.) )
	{
		init_in_param_step = min( -init_init_in_params_step,
				max_in_params - min_in_param );
	}
	else if ( init_init_in_params_step == units_cast<Tin>(0) )
	{
		init_in_param_step = ( max_in_params - min_in_param )
				/ (flt_type)default_step_number;
	}
	else
	{
		init_in_param_step = min( init_init_in_params_step,
				max_in_params - min_in_param );
	}
	num_in_params_steps = (int_type)floor(
			( max_in_params - min_in_param ) / init_in_param_step )
			+ 1.;
	num_test_points = num_in_params_steps;

	// First step is to search solution space for the best starting point
	i_best = 0;
	d_best = units_cast<Tout>(std::numeric_limits<flt_type>::max());
	in_param_step = init_in_param_step;
	bool starting_point_found = false;
	for ( int_type i = 0; i < num_test_points; i++ )
	{
		test_in_param = min_in_param + in_param_step * (flt_type)i;

		try
		{
			test_out_param = ( func )( test_in_param );
			d = abs( test_out_param - target_out_param );
			if ( d < d_best )
			{
				d_best = d;
				i_best = i;
				starting_point_found = true;
			}
		}
		catch (const std::exception &e)
		{
		}

	} // for(int_type i = 0; i < num_test_points; i++ )

	if ( !starting_point_found )
	{
		// Try a finer search to see if that can find it
		int_type search_factor = (int_type)( 1. / search_precision );
		in_param_step *= search_precision;

		// Recalculate number of test points
		num_test_points = num_in_params_steps * search_factor;

		for ( int_type i = 0; i < num_test_points; i++ )
		{
			// Figure out test_in_params for this point and get value there
			test_in_param = min_in_param + in_param_step * (flt_type)i;
			try
			{
				test_out_param = ( func )( test_in_param );
				d = abs( test_out_param - target_out_param );
				if ( d < d_best )
				{
					d_best = d;
					i_best = i;
				}
			}
			catch (const std::exception &e)
			{
			}

		} // for(int_type i = 0; i < num_test_points; i++ )

	} // if(!starting_point_found)

	// Check again to see if we've found a good start point
	if ( !starting_point_found )
	{
		// Nope. The function might just be undefined everywhere. Throw an exception
		throw std::runtime_error("Solve_grid could not find any defined range points.");
	} // if(!starting_point_found)

	// Get best_in_params
	best_in_param = min_in_param + in_param_step * (flt_type)i_best;

	// Narrowing search
	step_dist = 1;
	num_test_points = 3;
	in_param_step = init_in_param_step;
	precision = init_precision;

	while ( step_dist > precision )
	{
		in_param_step *= grid_shortening_factor;

		i_best = 0;
		d_best = units_cast<Tout>(std::numeric_limits<flt_type>::max());
		for ( int_type i = 0; i < num_test_points; i++ )
		{
			test_in_param = best_in_param + in_param_step * (flt_type)i - in_param_step;
			try
			{
				test_out_param = ( func )( test_in_param );
				d = abs( test_out_param - target_out_param );
				if ( d < d_best )
				{
					d_best = d;
					i_best = i;
				}
			}
			catch (const std::exception &e)
			{
			}

		} // for(int_type i = 0; i < num_test_points; i++ )

		// Figure out best_in_params for next step
		best_in_param += in_param_step * (flt_type)i_best - in_param_step;

		step_dist *= grid_shortening_factor;

	} // while( step_dist > precision )

	return best_in_param;
}

// Vector-in, vector-out version
//
// WARNING: This function hasn't been tested in a long while; it may no longer work.
template< typename f, typename Tin, BRG_F_NOT_CONTAINER(Tin) >
std::vector< Tin > solve_grid( const f & func,
		const std::vector< Tin > & init_min_in_params,
		const std::vector< Tin > & init_max_in_params,
		const std::vector< Tin > & init_init_in_params_step,
		const decltype(f()(std::vector< Tin >())) & target_out_params = decltype(f()(std::vector< Tin >()))(),
		const flt_type & init_init_precision = 0.00001, const flt_type & search_precision = 0.1,
		const std::vector< flt_type > & init_out_params_weight = std::vector<
				flt_type >( 0 ))
{
	typedef typename std::vector<Tin>::size_type vsize_t;
	vsize_t num_in_params = ssize(init_min_in_params);
	vsize_t num_out_params = 0;

	Tin d = 0, d_best = std::numeric_limits<flt_type>::max();
	int_type i_resid = 0, i_temp = 0, i_best = 0;
	std::vector< Tin > init_in_params_step( num_in_params, 0 );
	std::vector< Tin > in_params_step( num_in_params, 0 );
	std::vector< Tin > test_in_params( num_in_params, 0 );
	std::vector< Tin > best_in_params( num_in_params, 0 );
	std::vector< Tin > test_out_params( num_out_params, 0 );
	std::vector< Tin > min_in_params = init_min_in_params,
			max_in_params = init_max_in_params;
	std::vector< flt_type > out_params_weight = init_out_params_weight;

	const int_type default_step_number = 10;
	const flt_type & grid_shortening_factor = 0.5;

	flt_type init_precision = init_init_precision, precision = init_precision;
	flt_type step_dist = 1;
	int_type num_test_points = 0;
	int_type divisor = 1;
	flt_type total_weight = 0;
	std::vector< int_type > num_in_params_steps( num_in_params, 0 );

	// Check for sanity and set up parameters

	if(ssize(target_out_params)==0) target_out_params = std::vector<Tin>(0,ssize(func(init_min_in_params)));

	num_test_points = 1;
	if ( ssize(out_params_weight) == 0 )
	{
		out_params_weight.resize( ssize(target_out_params), 1 );
	}
	else if ( ssize(out_params_weight) != ssize(target_out_params) )
	{
		out_params_weight.resize( ssize(target_out_params), 0 );
	}
	total_weight = 0;
	for ( size_t i = 0; i < num_in_params; i++ )
	{
		if ( max_in_params.at( i ) < min_in_params.at( i ) )
			std::swap( max_in_params[i], min_in_params[i] );
		if ( init_init_in_params_step.at( i ) < 0 )
		{
			init_in_params_step.at( i ) = min(
					-init_init_in_params_step[i],
					max_in_params.at( i ) - min_in_params.at( i ) );
		}
		else if ( init_init_in_params_step.at( i ) == 0 )
		{
			init_in_params_step.at( i ) = ( max_in_params.at( i )
					- min_in_params.at( i ) ) / default_step_number;
		}
		else
		{
			init_in_params_step.at( i ) = min( init_init_in_params_step[i],
					max_in_params.at( i ) - min_in_params.at( i ) );
		}
		num_in_params_steps.at( i ) = (int_type)floor(
				( max_in_params.at( i ) - min_in_params.at( i ) )
						/ init_in_params_step.at( i ) ) + 1;
		num_test_points *= num_in_params_steps.at( i );
	}
	for(size_t i = 0; i < ssize(out_params_weight); i++)
	{
		out_params_weight.at( i ) = abs( out_params_weight.at( i ) );
		total_weight += out_params_weight.at( i );
	}
	if ( ( init_init_precision > 0 ) && ( init_init_precision <= 1 ) )
		init_precision = init_init_precision;
	if ( total_weight <= 0 )
	{
		out_params_weight.resize( num_in_params, 1 );
		total_weight = num_in_params;
	}

	// First step is to search solution space for the best starting point
	i_best = 0;
	d_best = std::numeric_limits<flt_type>::max();
	in_params_step = init_in_params_step;
	bool starting_point_found = false;
	for ( int_type i = 0; i < num_test_points; i++ )
	{
		// Figure out test_in_params for this point
		i_resid = i;
		divisor = num_test_points;
		for ( int_type j = 0; j < num_in_params; j++ )
		{
			divisor /= num_in_params_steps.at( j );
			i_temp = (int_type)( i_resid / divisor );
			i_resid -= i_temp * divisor;
			test_in_params.at( j ) = min_in_params.at( j )
					+ in_params_step.at( j ) * i_temp;
		}
		try
		{
			test_out_params = ( func )( test_in_params );
			assert(ssize(test_out_params) == ssize(target_out_params));
			assert(ssize(test_out_params) == ssize(out_params_weight));
			d = weighted_dist( test_out_params, target_out_params,
					out_params_weight );
			if ( d < d_best )
			{
				starting_point_found = true;
				d_best = d;
				i_best = i;
			}
		}
		catch (const std::exception &e)
		{
		}

	} // for(int_type i = 0; i < num_test_points; i++ )

	if ( !starting_point_found ) // We didn't find any suitable starting point
	{
		// Try a finer search to see if that can find it
		int_type search_factor = (int_type)( 1. / search_precision );
		in_params_step = init_in_params_step;
		for ( vsize_t i = 0; i < ssize(in_params_step); i++ )
			in_params_step[i] /= search_factor;
		i_best = 0;
		d_best = std::numeric_limits<flt_type>::max();

		// Recalculate number of test points
		num_test_points = 1;
		for ( vsize_t i = 0; i < num_in_params; i++ )
			num_test_points *= num_in_params_steps.at( i ) * search_factor;

		for ( int_type i = 0; i < num_test_points; i++ )
		{
			// Figure out test_in_params for this point
			i_resid = i;
			divisor = num_test_points;
			for ( vsize_t j = 0; j < num_in_params; j++ )
			{
				divisor /= num_in_params_steps.at( j );
				i_temp = (int_type)( i_resid / divisor );
				i_resid -= i_temp * divisor;
				test_in_params.at( j ) = min_in_params.at( j )
						+ in_params_step.at( j ) * i_temp;
			}
			try
			{
				test_out_params = ( func )( test_in_params );

				assert(ssize(test_out_params) == ssize(target_out_params));
				assert(ssize(test_out_params) == ssize(out_params_weight));

				d = weighted_dist( test_out_params, target_out_params,
						out_params_weight );
				if ( d < d_best )
				{
					starting_point_found = true;
					d_best = d;
					i_best = i;
				}
			}
			catch (const std::exception &e)
			{
			}

		} // for(int_type i = 0; i < num_test_points; i++ )

	} // if(!starting_point_found)

	// Check again to see if we've found a good start point
	if ( !starting_point_found )
	{
		// Nope. The function might just be undefined everywhere. Throw an exception
		throw std::runtime_error("Solve_grid could not find any defined range points.");
	} // if(!starting_point_found)

	// Get best_in_params
	best_in_params.resize( num_in_params, 0 );
	i_resid = i_best;
	divisor = num_test_points;
	for ( vsize_t j = 0; j < num_in_params; j++ )
	{
		divisor /= num_in_params_steps.at( j );
		i_temp = (int_type)( i_resid / divisor );
		i_resid -= i_temp * divisor;
		best_in_params.at( j ) = min_in_params.at( j )
				+ in_params_step.at( j ) * i_temp;
	}

	// Narrowing search
	step_dist = 1;
	num_test_points = runtime_ipow( 3, num_in_params );
	in_params_step = init_in_params_step;
	precision = init_precision;

	while ( step_dist > precision )
	{
		for ( vsize_t i = 0; i < num_in_params; i++ )
		{
			in_params_step[i] *= grid_shortening_factor;
		}

		i_best = -1;
		d_best = std::numeric_limits<flt_type>::max();
		for ( int_type i = 0; i < num_test_points; i++ )
		{
			// Figure out test_in_params for this point
			i_resid = i;
			divisor = num_test_points;
			for ( vsize_t j = 0; j < num_in_params; j++ )
			{
				divisor /= 3;
				i_temp = (int_type)( i_resid / divisor );
				i_resid -= i_temp * divisor;
				i_temp -= 1;
				test_in_params.at( j ) = best_in_params.at( j )
						+ in_params_step.at( j ) * i_temp;
			}
			try
			{
				test_out_params = ( func )( test_in_params );

				assert(ssize(test_out_params) == ssize(target_out_params));
				assert(ssize(test_out_params) == ssize(out_params_weight));

				d = weighted_dist( test_out_params, target_out_params,
						out_params_weight );
				if ( d < d_best )
				{
					d_best = d;
					i_best = i;
				}
			}
			catch (const std::exception &e)
			{
			}

		} // for(int_type i = 0; i < num_test_points; i++ )

		// Figure out test_in_params for this point
		i_resid = i_best;
		divisor = num_test_points;
		for ( int_type j = 0; j < num_in_params; j++ )
		{
			divisor /= 3;
			i_temp = (int_type)( i_resid / divisor );
			i_resid -= i_temp * divisor;
			i_temp -= 1;
			test_in_params.at( j ) = best_in_params.at( j )
					+ in_params_step.at( j ) * i_temp;
		}
		best_in_params = test_in_params;

		step_dist *= grid_shortening_factor;

	} // while( step_dist > precision )

	return best_in_params;
}

// Scalar-in, scalar-out version
template< typename f, typename Tin, typename Tout, BRG_F_NOT_CONTAINER(Tin), BRG_F_NOT_CONTAINER(Tout)>
Tin solve_grid( const f & func, const Tin & init_min_in_params, const Tin & init_max_in_params,
		const int_type & num_search_steps, const Tout & target_out_params,
		const flt_type & init_init_precision = 0.00001,
		const flt_type & search_precision = 0.1 )
{
	const int_type steps = (int_type)max( num_search_steps, 1 );

	Tin in_params_step(( init_max_in_params - init_min_in_params ) / (flt_type)steps);
	return IceBRG::solve_grid( func, init_min_in_params,
			init_max_in_params, in_params_step, target_out_params,
			init_init_precision, search_precision);
} // const int_type solve_grid(...)

// Vector-in, vector-out version
template< typename f, typename Tin, typename Tout, BRG_F_NOT_CONTAINER(Tin), BRG_F_NOT_CONTAINER(Tout) >
std::vector<Tin> solve_grid( const f & func,
		const std::vector< Tin > & init_min_in_params,
		const std::vector< Tin > & init_max_in_params,
		const int_type & num_search_steps, const std::vector< Tout > & target_out_params,
		const flt_type & init_init_precision = 0.00001, const flt_type & search_precision = 0.1,
		const std::vector< flt_type > & out_params_weight =
				std::vector< flt_type >( 0 ) )
{
	std::vector< Tin > in_params_step( ssize(init_min_in_params), 0 );

	const int_type steps = (int_type)max( num_search_steps, 1 );

	assert(ssize(init_min_in_params)==ssize(init_max_in_params));

	for ( size_t i = 0; i < ssize(init_min_in_params); i++ )
		in_params_step[i] = ( init_max_in_params.at[i]
				- init_min_in_params.at[i] ) / (flt_type)steps;

	return IceBRG::solve_grid( func, init_min_in_params,
			init_max_in_params, in_params_step, target_out_params,
			init_init_precision, search_precision,
			out_params_weight );
} // const int_type solve_grid(...)

/**
 * Attempts to find the minimum output value for the passed function using a Metropolis-Hastings
 * MCMC algorithm with simulated annealing.
 *
 * @param func
 * @param init_in_param
 * @param init_min_in_param
 * @param init_max_in_param
 * @param init_in_param_step_sigma
 * @param result_in_param
 * @param result_out_param
 * @param max_steps
 * @param annealing_period
 * @param annealing_factor
 * @return
 */
template< typename f, typename T, BRG_F_NOT_CONTAINER(T) >
T solve_MCMC( const f & func, const T & init_in_param, const T & init_min_in_param,
		const T  &init_max_in_param, const T & init_in_param_step_sigma,
		const int_type & max_steps=1000000, const int_type & annealing_period=100000,
		const flt_type & annealing_factor=4)
{
	static_assert(boost::is_convertible<decltype(func(init_in_param)),flt_type>::value,
			"The output of the function passed to solve_MCMC must be convertible to a float-type.");

	bool bounds_check = true;

	// Check how bounds were passed in
	if(init_min_in_param==init_max_in_param) bounds_check = false;

	T min_in_param, max_in_param;
	if(init_min_in_param>init_max_in_param)
	{
		// Swap them
		max_in_param = init_min_in_param;
		min_in_param = init_max_in_param;
	}
	else
	{
		min_in_param = init_min_in_param;
		max_in_param = init_max_in_param;
	}
	T in_param_step_sigma;

	// Check step size
	if(init_in_param_step_sigma <= 0)
	{
		if(bounds_check)
		{
			in_param_step_sigma = (max_in_param-min_in_param)/10.;
		}
		else
		{
			in_param_step_sigma = 1; // Default behaviour
		}
	}
	else
	{
		in_param_step_sigma = init_in_param_step_sigma;
	}

	// Initialise
	flt_type annealing = 1;
	bool last_cycle = false;
	T current_in_param = init_in_param, test_in_param = init_in_param, best_in_param = init_in_param;
	T out_param, best_out_param;
	T mean_in_param = 0;
	int_type last_cycle_count = 0;

	// Get value at initial point
	try
	{
		out_param = func(test_in_param);
	}
	catch(const std::exception &e)
	{
		std::cerr << "Cannot execute solve_MCMC at initial point.\n";
		throw;
	}
	best_out_param = out_param;
	flt_type last_likelihood = std::exp(-annealing*out_param/2);

	for(int_type step = 0; step < max_steps; step++)
	{
		// Get a new value
		test_in_param = current_in_param + in_param_step_sigma*IceBRG::Gaus_rand()/annealing;

		// Check if it's in bounds
		if(bounds_check)
			test_in_param = IceBRG::bound(min_in_param,test_in_param,max_in_param);

		// Find the result for this value
		bool good_result = true;
		try
		{
			out_param = func(test_in_param);
		}
		catch(const std::exception &e)
		{
			good_result = false;
		}
		flt_type new_likelihood = std::exp(-annealing*out_param/2);

		// If it's usable, check if we should step to it
		if(good_result)
		{
			bool step_to_it = false;
			if(new_likelihood>=last_likelihood)
			{
				step_to_it = true;
			}
			else
			{
				if(drand() < new_likelihood/last_likelihood)
					step_to_it = true;
			}
			if(step_to_it)
			{
				last_likelihood = new_likelihood;
				current_in_param = test_in_param;

				// Check if we have a new best point
				if(out_param < best_out_param)
				{
					best_out_param = out_param;
					best_in_param = current_in_param;
				}
			}
		}

		// If we're on the last cycle, add to the mean
		if(last_cycle)
		{
			mean_in_param += current_in_param;
			last_cycle_count += 1;
		}

		if(IceBRG::divisible(annealing_period,step))
		{
			annealing *= annealing_factor;

			// Recalculate likelihood
			last_likelihood = std::pow(last_likelihood,annealing_factor);

			// Check if we're going into the last cycle
			if(max_steps-step<=annealing_period)
				last_cycle = true;
		}
	} // for(int_type step = 0; step < max_steps; step++)

	// Calculate mean now
	mean_in_param /= last_cycle_count;

	// Check if mean actually gives a better best
	try
	{
		out_param = func(mean_in_param);
	}
	catch(const std::exception &e)
	{
		// Just leave it, no need to do anything
	}

	return best_in_param;
}

/** Attempts to find the minimum output value for the passed function using a Metropolis-Hastings
 *  MCMC algorithm with simulated annealing. Vector version
 *
 * @param func
 * @param init_in_param
 * @param init_min_in_param
 * @param init_max_in_param
 * @param init_in_param_step_sigma
 * @param result_in_param
 * @param result_out_param
 * @param max_steps
 * @param annealing_period
 * @param annealing_factor
 * @return
 */
template< typename f, typename T, BRG_F_IS_CONTAINER_OR_TUPLE(T) >
T solve_MCMC( const f & func, const T & init_in_params,
		const T & init_min_in_params,
		const T & init_max_in_params,
		const T & init_in_param_step_sigmas,
		const int_type & max_steps=1000000,
		const int_type & annealing_period=100000, const flt_type & annealing_factor=4)
{
	static_assert(boost::is_convertible<decltype(func(init_in_params)),flt_type>::value,
			"The output of the function passed to solve_MCMC must be convertible to a float-type.");

	bool check_lower_bound = true;
	bool check_upper_bound = true;

	// Check how bounds were passed in
	if(ssize(init_min_in_params)==0)
		check_lower_bound = false;
	if(ssize(init_min_in_params)==0)
		check_upper_bound = false;

	T min_in_params = init_min_in_params;
	T max_in_params = init_max_in_params;

	if(check_lower_bound && check_upper_bound)
		fix_min_and_max(min_in_params,max_in_params);

	// Check step sizes

	T in_param_step_sigmas = init_in_param_step_sigmas;

	if(check_lower_bound && check_upper_bound)
		fix_step_sigmas(in_param_step_sigmas,min_in_params,max_in_params);
	else
		fix_step_sigmas(in_param_step_sigmas);

	// Initialise
	flt_type annealing = 1;
	bool last_cycle = false;
	T current_in_params = init_in_params, test_in_params = init_in_params,
			best_in_params = init_in_params;
	flt_type out_param, best_out_param;
	T mean_in_params(multiply(init_in_params,0.));
	int_type last_cycle_count = 0;

	// Get value at initial point
	try
	{
		out_param = func(test_in_params);
	}
	catch(const std::exception &e)
	{
		std::cerr << "Cannot execute solve_MCMC at initial point.\n";
		throw;
	}
	best_out_param = out_param;
	flt_type last_log_likelihood = -annealing*out_param/2.;

	auto new_Gaus_rand = [] () {return IceBRG::Gaus_rand();};

	for(int_type step = 0; step < max_steps; step++)
	{
		// Get a new value

		test_in_params = add(current_in_params, multiply(
				rand_container_of_size<typename all_flt_type<T>::type>(new_Gaus_rand,
						ssize(current_in_params)),
				divide(in_param_step_sigmas,annealing)));

		// Check if it's in bounds
		if(check_lower_bound)
			test_in_params = max(min_in_params,test_in_params);
		if(check_upper_bound)
			test_in_params = min(test_in_params,max_in_params);

		// Find the result for this value
		bool good_result = true;
		try
		{
			out_param = func(test_in_params);
		}
		catch(const std::exception &e)
		{
			good_result = false;
		}
		flt_type new_log_likelihood = -annealing*out_param/2.;

		// If it's usable, check if we should step to it
		bool step_to_it = false;
		if(good_result)
		{
			if(new_log_likelihood>=last_log_likelihood)
			{
				step_to_it = true;
			}
			else
			{
				if(drand() < std::exp(new_log_likelihood - last_log_likelihood))
					step_to_it = true;
			}
			if(step_to_it)
			{
				last_log_likelihood = new_log_likelihood;
				current_in_params = test_in_params;

				// Check if we have a new best point
				if(out_param < best_out_param)
				{
					best_out_param = out_param;
					best_in_params = current_in_params;
				}
			}
		}

		// If we're on the last cycle, add to the mean
		if(last_cycle)
		{
			mean_in_params = IceBRG::add(mean_in_params,current_in_params);
			last_cycle_count += 1;
		}

		if((step!=0)&&(IceBRG::divisible(step,annealing_period)))
		{
			annealing *= annealing_factor;

			// Recalculate likelihood
			last_log_likelihood *= annealing_factor;

			// Check if we're going into the last cycle
			if(max_steps-step<=annealing_period)
				last_cycle = true;
		}
	} // for(int_type step = 0; step < max_steps; step++)

	// Calculate mean now
	mean_in_params = IceBRG::divide(mean_in_params,(flt_type)safe_d(last_cycle_count));

	// Check if mean actually gives a better best
	try
	{
		out_param = func(mean_in_params);
		if(out_param < best_out_param)
		{
			best_in_params = mean_in_params;
			best_out_param = out_param;
		}
	}
	catch(const std::exception &e)
	{
		// Just leave it, no need to do anything
	}

	return best_in_params;
}

} // namespace IceBRG

#endif // __BRG_SOLVERS_HPP_INCLUDED__
