/**********************************************************************\
 @file ipow.hpp
 ------------------

 TODO <Insert file description here>

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

#ifndef BRG_MATH_IPOW_HPP_
#define BRG_MATH_IPOW_HPP_

#include <type_traits>

#include "../container/is_stl_container.hpp"

namespace IceBRG {

// Integer power - use only when you know it'll be an integer, but not the specific value,
// and when it isn't likely to be too high

#if (1)
// This version is optimized so that it won't check for the p==0 and p<0 cases, and generally
// shouldn't be called directly
template< typename T,
typename std::enable_if<!IceBRG::is_stl_container<T>::value,T>::type* = nullptr >
T _runtime_ipow( T v, int_type p )
{
	if(p==1) return v;
	T tmp = _runtime_ipow(v,p/2);
	if(p%2==0) return tmp*tmp;
	return v*tmp*tmp;
}
#endif

template< typename T,
typename std::enable_if<!IceBRG::is_stl_container<T>::value,T>::type* = nullptr >
T runtime_ipow( T v, int_type p )
{
	if(p<0) return 1/_runtime_ipow(v,-p);
	if(p==0) return 1;
	if(p==1) return v;
	T tmp = _runtime_ipow(v,p/2);
	if(p%2==0) return tmp*tmp;
	return v*tmp*tmp;
}

template< int_type p >
struct _ipow_s {
	const flt_type & value;

	_ipow_s( const flt_type & v )
	: value(v*_ipow_s<p-1>(v).value) // Using a lambda for a multi-line initializer
	{
	}
};

template<>
struct _ipow_s<1> {
	const flt_type & value;

	_ipow_s( const flt_type & v )
	: value(v)
	{
	}
};

template<>
struct _ipow_s<0> {
	const flt_type & value;

	_ipow_s( const flt_type & v )
	: value(1.)
	{
	}
};

template< int_type p >
flt_type ipow( const flt_type & v)
{
	if(p>0)	return _ipow_s<abs(p)>(v).value;
	if(p<0) return 1./_ipow_s<abs(p)>(v).value;
	return 1.;
}

} // namespace IceBRG



#endif // BRG_MATH_IPOW_HPP_
