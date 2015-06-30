/**********************************************************************\
 @file get_errors_on_fit.hpp
 ------------------

 TODO <Insert file description here>

 **********************************************************************

 Copyright (C) 2015  Bryan R. Gillis

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

#ifndef _BRG_SRC_GET_ERRORS_ON_FIT_HPP_INCLUDED_
#define _BRG_SRC_GET_ERRORS_ON_FIT_HPP_INCLUDED_

#include <cmath>
#include <stdexcept>
#include <utility>

#include "common.h"

constexpr int_type counter_max = 10000000;
constexpr flt_type error_step_factor = 0.01;

template< typename T_func, typename T_in_params, typename T_chi_squared >
std::vector<flt_type> get_errors_on_in_params(const T_func & func,
	T_in_params & best_in_params, T_chi_squared & best_chi_squared )
{
	const int_type num_in_params = best_in_params.size();

	std::vector<flt_type> res(num_in_params);

	bool didnt_start_at_best = false;
	int_type counter = 0;
	do
	{
		if(++counter>counter_max) throw std::runtime_error("Could not get errors on fit - not at best fit point most likely.");

		didnt_start_at_best = false;
		// For each in parameter in turn
		for(int_type i=0; i<num_in_params; ++i)
		{
			// Make a small step up and down, and see how the chi-squared changes
			flt_type & best_in_param =  best_in_params[i];
			flt_type step_size = error_step_factor * best_in_param;

			flt_type high_in_param = best_in_param + step_size;
			std::swap(high_in_param,best_in_param);
			flt_type high_chi_squared = func(best_in_params).at(0); // where best_in_params now contains high_in_param

			// Check if this is actually better than the best
			if(high_chi_squared<best_chi_squared)
			{
				// This is actually better, so leave it swapped in, swap in the best chi_squared too, and restart the loop
				std::swap(high_chi_squared,best_chi_squared);
				didnt_start_at_best = true;
				continue; // We'll check other indices as well, in case they also have problems
			} // Otherwise continue

			std::swap(high_in_param,best_in_param); // Swap back

			flt_type low_in_param = best_in_param - step_size;
			std::swap(low_in_param,best_in_param);
			flt_type low_chi_squared = func(best_in_params).at(0); // where best_in_params now contains low_in_param

			// Check if this is actually better than the best
			if(low_chi_squared<best_chi_squared)
			{
				// This is actually better, so leave it swapped in, swap in the best chi_squared too, and restart the loop
				std::swap(low_chi_squared,best_chi_squared);
				didnt_start_at_best = true;
				break; // We'll check other indices as well, in case they also have problems
			} // Otherwise continue

			std::swap(low_in_param,best_in_param);

			flt_type high_step_factor = std::sqrt(1./(high_chi_squared-best_chi_squared));
			flt_type low_step_factor = std::sqrt(1./(low_chi_squared-best_chi_squared));

			flt_type error_on_in_param = (high_step_factor+low_step_factor)/2. * step_size;

			res[i] = std::move(error_on_in_param);
		}
	} while(didnt_start_at_best);

	return res;
};



#endif // _BRG_SRC_GET_ERRORS_ON_FIT_HPP_INCLUDED_
