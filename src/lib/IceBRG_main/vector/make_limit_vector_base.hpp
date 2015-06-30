/**********************************************************************\
 @file make_limit_vector.hpp
 ------------------

 TODO <Insert file description here>

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

#ifndef _BRG_VECTOR_MAKE_LIMIT_VECTOR_BASE_HPP_INCLUDED_
#define _BRG_VECTOR_MAKE_LIMIT_VECTOR_BASE_HPP_INCLUDED_

#include <type_traits>
#include <vector>

#include "IceBRG_main/common.h"

#include "IceBRG_main/math/misc_math.hpp"

namespace IceBRG {

template<typename T, typename A=std::allocator<T>>
std::vector<T,A> make_linear_limit_vector_base(const T & min, const T & max, const size_t & num_bins)
{
	if(max<=min)
		throw std::logic_error("max must be greater than min for limit vector.\n");
	if(num_bins<1)
		throw std::logic_error("num_bins must be greater than zero for limit vector.\n");
	if(( isinf(min) || isinf(max) ) && (num_bins>1))
		throw std::logic_error("Cannot generate limit vector with and infinite limits and multiple bins.");

	T step = (max-min)/(flt_type)num_bins;

	std::vector<T,A> result(1,min);

	if(!isinf(step))
	{
		for(size_t limit_num=1; limit_num<num_bins; ++limit_num)
		{
			// Recalculate at each step to minimize round-off error
			result.push_back( min + step*(flt_type)limit_num );
		}
	}
	result.push_back(max);

	return result;
}

template<typename T, typename A=std::allocator<T>>
std::vector<T,A> make_log_limit_vector_base(const T & min, const T & max, const size_t & num_bins)
{
	if(std::is_integral<T>::value)
		throw std::logic_error("Log limit vectors cannot be of integral type.\n");
	if(max<=min)
		throw std::logic_error("max must be greater than min for limit vector.\n");
	if(value_of(min)<=0)
		throw std::logic_error("min must be greater than zero for log limit vector.\n");
	if(num_bins<1)
		throw std::logic_error("num_bins must be greater than zero for limit vector.\n");

	std::vector<T,A> result(1,min);

	flt_type log_step = std::pow(value_of(max/min),1./(num_bins));

	if( isinf(max) )
	{
		if(num_bins>1)
			throw std::logic_error("Cannot generate limit vector with and infinite limits and multiple bins.");
		result.push_back(max);
		return result;
	}

	for(size_t limit_num=1; limit_num<num_bins; ++limit_num)
	{
		// Recalculate at each step to minimize round-off error
		result.push_back( min * IceBRG::runtime_ipow(log_step,limit_num) );
	}

	result.push_back(max);

	return result;
}

} // end namespace IceBRG

#endif // _BRG_VECTOR_MAKE_LIMIT_VECTOR_BASE_HPP_INCLUDED_
