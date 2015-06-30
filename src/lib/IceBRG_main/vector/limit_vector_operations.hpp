/**********************************************************************\
 @file limit_vector_operations.hpp
 ------------------

 Operations that allow one to treat a regular vector as if it's a
 limit vector (with type GENERAL) without converting it first.

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

#ifndef _BRG_LIMIT_VECTOR_OPERATIONS_HPP_INCLUDED_
#define _BRG_LIMIT_VECTOR_OPERATIONS_HPP_INCLUDED_

#include <cassert>
#include <stdexcept>
#include <vector>

#include "IceBRG_main/common.h"

#include "IceBRG_main/utility.hpp"
#include "IceBRG_main/vector/limit_vector.hpp"
#include "IceBRG_main/vector/summary_functions.hpp"

namespace IceBRG {

template<typename T, typename A=std::allocator<T>>
bool above_limits(const T & val, const std::vector<T,A> & vec)
{
	assert(is_monotonically_increasing(vec));
	return val>vec.back();
}
template<typename T, typename A=std::allocator<T>>
bool above_limits(const T & val, const IceBRG::limit_vector<T,A> & vec)
{
	return vec.above_limits(val);
}

template<typename T, typename A=std::allocator<T>>
bool checked_above_limits(const T & val, const std::vector<T,A> & vec)
{
	if(!is_monotonically_increasing(vec))
		throw std::logic_error("Invalid limit vector passed to above_limits.");
	return above_limits(val,vec);
}
template<typename T, typename A=std::allocator<T>>
bool checked_above_limits(const T & val, const IceBRG::limit_vector<T,A> & vec)
{
	return vec.above_limits(val);
}

template<typename T, typename A=std::allocator<T>>
bool under_limits(const T & val, const std::vector<T,A> & vec)
{
	assert(is_monotonically_increasing(vec));
	return val<vec.front();
}
template<typename T, typename A=std::allocator<T>>
bool under_limits(const T & val, const IceBRG::limit_vector<T,A> & vec)
{
	return vec.under_limits(val);
}

template<typename T, typename A=std::allocator<T>>
bool checked_under_limits(const T & val, const std::vector<T,A> & vec)
{
	if(!is_monotonically_increasing(vec))
		throw std::logic_error("Invalid limit vector passed to under_limits.");
	return under_limits(val,vec);
}
template<typename T, typename A=std::allocator<T>>
bool checked_under_limits(const T & val, const IceBRG::limit_vector<T,A> & vec)
{
	return vec.under_limits(val);
}

template<typename T, typename A=std::allocator<T>>
bool outside_limits(const T & val, const std::vector<T,A> & vec)
{
	return (above_limits(val,vec) or under_limits(val,vec));
}
template<typename T, typename A=std::allocator<T>>
bool outside_limits(const T & val, const IceBRG::limit_vector<T,A> & vec)
{
	return vec.outside_limits(val);
}

template<typename T, typename A=std::allocator<T>>
bool checked_outside_limits(const T & val, const std::vector<T,A> & vec)
{
	return (checked_above_limits(val,vec) or checked_under_limits(val,vec));
}
template<typename T, typename A=std::allocator<T>>
bool checked_outside_limits(const T & val, const IceBRG::limit_vector<T,A> & vec)
{
	return vec.outside_limits(val);
}

template<typename T, typename A=std::allocator<T>>
bool inside_limits(const T & val, const std::vector<T,A> & vec)
{
	return !outside_limits(val,vec);
}
template<typename T, typename A=std::allocator<T>>
bool inside_limits(const T & val, const IceBRG::limit_vector<T,A> & vec)
{
	return vec.inside_limits(val);
}

template<typename T, typename A=std::allocator<T>>
bool checked_inside_limits(const T & val, const std::vector<T,A> & vec)
{
	return !checked_outside_limits(val,vec);
}
template<typename T, typename A=std::allocator<T>>
bool checked_inside_limits(const T & val, const IceBRG::limit_vector<T,A> & vec)
{
	return vec.inside_limits(val);
}

template<typename T, typename A=std::allocator<T>>
ssize_t checked_get_bin_index(const T & val, const std::vector<T,A> & vec)
{
	if(!is_monotonically_increasing(vec))
		throw std::logic_error("Invalid limit vector passed to checked_get_bin_index.");
	if(outside_limits(val,vec))
		throw std::runtime_error("Value outside limits of vector in checked_get_bin_index.");
	return get_bin_index(val,vec);
}
template<typename T, typename A=std::allocator<T>>
ssize_t checked_get_bin_index(const T & val, const IceBRG::limit_vector<T,A> & vec)
{
	if(vec.outside_limits(val))
		throw std::runtime_error("Value outside limits of vector in checked_get_bin_index.");
	 vec.get_bin_index(val);
}

template<typename T, typename A=std::allocator<T>>
ssize_t get_bin_index(const T & val, const std::vector<T,A> & vec)
{
	assert(is_monotonically_increasing(vec));
	for(ssize_t i=1; i<ssize(vec); ++i)
	{
		if(vec[i]>=val) return i-1;
	}
	if(above_limits(val,vec)) return ssize(vec)+1;
	return ssize(vec);
}
template<typename T, typename A=std::allocator<T>>
ssize_t get_bin_index(const T & val, const IceBRG::limit_vector<T,A> & vec)
{
	return vec.get_bin_index(val);
}

template<typename T, typename A=std::allocator<T>>
ssize_t get_bin_index_no_overflow(const T & val, const std::vector<T,A> & vec)
{
	assert(is_monotonically_increasing(vec));
	for(ssize_t i=1; i<ssize(vec); ++i)
	{
		if(vec[i]>=val) return i-1;
	}
	if(above_limits(val,vec)) return ssize(vec)-2;
	return 0;
}
template<typename T, typename A=std::allocator<T>>
ssize_t get_bin_index_no_overflow(const T & val, const IceBRG::limit_vector<T,A> & vec)
{
	return vec.get_bin_index_no_overflow(val);
}

template<typename T1, typename A1=std::allocator<T1>, typename T2=T1, typename A2=std::allocator<T2>>
T2 interpolate_bins(const T2 & val, const std::vector<T1,A1> & lim_vec, const std::vector<T2,A2> & val_vec)
{
	assert(ssize(val_vec)>1);
	assert(ssize(lim_vec)==ssize(val_vec)+1);
	assert(is_monotonically_increasing(lim_vec));
	assert(ssize(lim_vec)>2);

	ssize_t bin_i=ssize(lim_vec)-3;

	for(ssize_t i=0; i<ssize(lim_vec)-2; ++i)
	{
		if((lim_vec[i]+lim_vec[i+1])/2>=val)
		{
			if(i==0)
				bin_i=0;
			else
				bin_i=i-1;
			break;
		}
	}

	T1 xlo = (lim_vec[bin_i]+lim_vec[bin_i+1])/2;
	T1 xhi = (lim_vec[bin_i+1]+lim_vec[bin_i+2])/2;
	const T2 & ylo = val_vec[bin_i];
	const T2 & yhi = val_vec[bin_i+1];

	return ylo + (yhi-ylo)/(xhi-xlo) * (val-xlo);
}
template<typename T1, typename A1=std::allocator<T1>, typename T2=T1, typename A2=std::allocator<T2>>
T2 interpolate_bins(const T2 & val, const IceBRG::limit_vector<T1,A1> & lim_vec, const std::vector<T2,A2> & val_vec)
{
	return lim_vec.interpolate_bins(val,val_vec);
}

template<typename T1, typename A1=std::allocator<T1>>
std::vector<T1,A1> get_bin_mids_from_limits(std::vector<T1,A1> vec)
{
	assert(is_monotonically_increasing(vec));

	for(ssize_t i=0; i<ssize(vec)-1; ++i)
	{
		vec[i] += (vec[i+1]-vec[i])/2;
	}

	vec.pop_back();
	return vec;
}
template<typename T1, typename A1=std::allocator<T1>>
std::vector<T1,A1> get_bin_mids_from_limits(const IceBRG::limit_vector<T1,A1> & vec)
{
	return vec.get_bin_mids();
}

template<typename T1, typename A1=std::allocator<T1>>
std::vector<T1,A1> get_bin_limits_from_mids(std::vector<T1,A1> vec)
{
	assert(is_monotonically_increasing(vec));

	ssize_t i;

	for(i=0; i<ssize(vec)-1; ++i)
	{
		vec[i] -= (vec[i+1]-vec[i])/2;
	}

	// Special handling for the final element
	T1 d_last = (vec[i]-vec[i-1])/3;
	vec.push_back(vec[i]+d_last);
	vec[i]-=d_last;

	return vec;
}
template<typename T1, typename A1=std::allocator<T1>>
IceBRG::limit_vector<T1,A1> make_limit_vector_from_mids(std::vector<T1,A1> vec)
{
	if(!is_monotonically_increasing(vec))
		throw std::logic_error("Cannot make limit vector from mids vector which isn't monotonically increasing.\n");
	IceBRG::limit_vector<T1,A1> result;

	result.reconstruct_from_bin_mids(vec);
}
template<typename T1, typename A1=std::allocator<T1>>
IceBRG::limit_vector<T1,A1> make_limit_vector_from_mids(std::vector<T1,A1> && vec)
{
	if(!is_monotonically_increasing(vec))
		throw std::logic_error("Cannot make limit vector from mids vector which isn't monotonically increasing.\n");
	IceBRG::limit_vector<T1,A1> result;

	result.reconstruct_from_bin_mids(std::move(vec));
}


} // namespace IceBRG



#endif // _BRG_LIMIT_VECTOR_OPERATIONS_HPP_INCLUDED_
