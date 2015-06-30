/**********************************************************************\
 @file generic_functions.hpp
 ------------------

 Functions which will operate in the same way on a container or
 boost-style tuple.

 **********************************************************************

 Copyright (C) 2015 brg

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

#ifndef BRG_CONTAINER_GENERIC_FUNCTIONS_HPP_
#define BRG_CONTAINER_GENERIC_FUNCTIONS_HPP_

#include <cassert>
#include <utility>

#include "IceBRG_main/container/is_container.hpp"
#include "IceBRG_main/units/units.hpp"
#include "IceBRG_main/utility.hpp"

namespace IceBRG {

// Generic structs
#if(1)

template<typename T, typename Enable = void>
struct all_flt_type
{ };

template<typename T>
struct all_flt_type<T,BRG_S_IS_STL(T)>
{
	typedef std::vector<flt_type> type;
};

template<typename T>
struct all_flt_type<T,BRG_S_IS_EIGEN(T)>
{
	typedef flt_array_type type;
};

template<typename T>
struct all_flt_type<T,BRG_S_IS_NULL(T)>
{
	typedef boost::tuples::null_type type;
};

template<typename T>
struct all_flt_type<T,BRG_S_IS_TUPLE(T)>
{
	typedef typename std::decay<T>::type Td;

	typedef boost::tuples::cons<flt_type,
			typename all_flt_type<decltype(Td().get_tail())>::type> type;
};

#endif // Generic structs

// Normal versions
#if(1)

// Get a reference to an element

template< typename T, BRG_F_IS_CONTAINER(T) >
auto get( T & v, const int_type & i ) -> decltype(v[i])
{
	assert(i>=0);
	assert(i<ssize(v));

	return v[i];
}

// Note - the tuple version of this may not work if the tuple includes
// incompatible types.
template< typename T, BRG_F_IS_TUPLE(T) >
auto get( T & v, const int_type & i ) -> decltype(v.get_head())
{
	assert(i>=0);
	assert(i<ssize(v));

	if(i==0)
		return v.get_head();
	else
		return get(v.get_tail(),i-1);
}

// Fix min and max - if an element of the second is lower than the corresponding element in the
// first, the two elements are swapped
template< typename T, BRG_F_IS_CONTAINER(T) >
void fix_min_and_max( T & v1, T & v2 )
{
	assert(ssize(v1)==ssize(v2));

	for(int_type i=0; i<ssize(v1); ++i)
	{
		if(v1[i]>v2[i]) std::swap(v1[i],v2[i]);
	}

	return;
}

// Note - the tuple version of this may not work if the tuple includes
// incompatible types.
template< typename T, BRG_F_IS_NULL(T) >
void fix_min_and_max( const T & v1, const T & v2 )
{
	return;
}
template< typename T, BRG_F_IS_TUPLE(T) >
void fix_min_and_max( T & v1, T & v2 )
{
	if(v1.get_head()>v2.get_head()) std::swap(v1.get_head(),v2.get_head());

	if(ssize(v1)==1) return;

	fix_min_and_max(v1.get_tail(),v2.get_tail());

	return;
}

// Fix step sigmas - make sure it's not larger than max-min, and make sure it's positive
template< typename T, BRG_F_IS_CONTAINER(T) >
void fix_step_sigmas( T & v_sigmas, const T & v_min, const T & v_max )
{
	assert(ssize(v_min)==ssize(v_max));

	typedef typename std::decay<decltype(v_sigmas[0])>::type value_type;

	value_type zero;
	set_zero(zero);

	if(ssize(v_sigmas) != ssize(v_min))
	{
		v_sigmas.resize(ssize(v_min));
		for(auto & v : v_sigmas)
			set_zero(v);
	}

	for(int_type i=0; i<ssize(v_sigmas); ++i)
	{
		if( (v_sigmas[i]<=zero) || (v_sigmas[i] >= v_max[i]-v_min[i]))
			v_sigmas[i] = (v_max[i]-v_min[i])/10.;
	}

	return;
}

// Note - the tuple version of this may not work if the tuple includes
// incompatible types.
template< typename T, BRG_F_IS_NULL(T) >
void fix_step_sigmas( const T & v_sigmas, const T & v_min, const T & v_max )
{
	return;
}
template< typename T, BRG_F_IS_TUPLE(T) >
void fix_step_sigmas( T & v_sigmas, const T & v_min, const T & v_max )
{
	typedef typename std::decay<decltype(v_sigmas.get_head())>::type value_type;

	value_type zero;
	set_zero(zero);

	if((v_sigmas.get_head() <= zero) || (v_sigmas.get_head() <= v_max.get_head() - v_min.get_head() ))
		v_sigmas.get_head() = (v_max.get_head() - v_min.get_head())/10.;

	fix_step_sigmas(v_sigmas.get_tail(),v_min.get_tail(),v_max.get_tail());

	return;
}

// Fix step sigmas - make sure it's not larger than max-min, and make sure it's positive
template< typename T, BRG_F_IS_CONTAINER(T) >
void fix_step_sigmas( const T & v_sigmas )
{
	typedef typename std::decay<decltype(v_sigmas[0])>::type value_type;

	value_type zero;
	set_zero(zero);

	for(int_type i=0; i<ssize(v_sigmas); ++i)
	{
		if( v_sigmas[i]<=zero )
			v_sigmas[i] = units_cast<value_type>(1.);
	}

	return;
}

// Note - the tuple version of this may not work if the tuple includes
// incompatible types.
template< typename T, BRG_F_IS_NULL(T) >
void fix_step_sigmas( const T & v_sigmas )
{
	return;
}
template< typename T, BRG_F_IS_TUPLE(T) >
void fix_step_sigmas( T & v_sigmas )
{
	typedef typename std::decay<decltype(v_sigmas.get_head())>::type value_type;

	value_type zero;
	set_zero(zero);

	if(v_sigmas.get_head() <= zero)
		v_sigmas.get_head() = units_cast<value_type>(1.);

	fix_step_sigmas(v_sigmas.get_tail());

	return;
}

#endif // Normal versions

// Attempt versions - will compile even if the type can't handle it, but will hit an assert if called
#if(1)

// Get a reference to an element

template< typename T, BRG_F_IS_CONTAINER(T) >
auto attempt_get( T & v, const int_type & i ) -> decltype(v[i])
{
	assert(i>=0);
	assert(i<ssize(v));

	return v[i];
}

// Note - the tuple version of this may not work if the tuple includes
// incompatible types.
template< typename T, BRG_F_IS_TUPLE(T) >
auto attempt_get( T & v, const int_type & i ) -> decltype(v.get_head())
{
	assert(i>=0);
	assert(i<ssize(v));

	if(i==0)
	{
		return v.get_head();
	}
	else
	{
		assert(false);
	}
}

#endif // Attempt versions

} // namespace IceBRG

#endif // BRG_CONTAINER_GENERIC_FUNCTIONS_HPP_
