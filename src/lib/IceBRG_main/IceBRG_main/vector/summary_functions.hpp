/**********************************************************************\
  @file summary_functions.hpp

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

#ifndef SUMMARY_FUNCTIONS_HPP_
#define SUMMARY_FUNCTIONS_HPP_

#include <vector>

#include "../container/is_stl_container.hpp"
#include "IceBRG_main/common.h"

#include "IceBRG_main/container/is_eigen_container.hpp"
#include "IceBRG_main/utility.hpp"
#include "IceBRG_main/vector/elementwise_functions.hpp"

namespace IceBRG {

// Recursive size
#if(1)

template< typename T, typename std::enable_if<!IceBRG::is_stl_container<T>::value, char>::type = 0,
	typename std::enable_if<IceBRG::is_eigen_container<T>::value, char>::type = 0 >
typename T::Index rsize(const T & v)
{
	return v.size();
}

template< typename T, typename std::enable_if<!IceBRG::is_stl_container<T>::value, char>::type = 0,
	typename std::enable_if<!IceBRG::is_eigen_container<T>::value, char>::type = 0 >
ssize_t rsize(const T & v)
{
	return 1;
}

template< typename T, typename std::enable_if<IceBRG::is_stl_container<T>::value, char>::type = 0 >
ssize_t rsize(const T &v)
{
	ssize_t result = 0;
	for(const auto & val : v)
	{
		result += rsize(val);
	}
	return result;
}
#endif

// Sum
#if (1)

template< typename T, typename std::enable_if<!IceBRG::is_stl_container<T>::value, char>::type = 0,
	typename std::enable_if<!IceBRG::is_eigen_container<T>::value, char>::type = 0 >
T sum(const T & v)
{
	return v;
}

template< typename T, typename std::enable_if<!IceBRG::is_stl_container<T>::value, char>::type = 0,
	typename std::enable_if<IceBRG::is_eigen_container<T>::value, char>::type = 0 >
typename T::Scalar sum(const T & v)
{
	return v.sum();
}

template< typename T, typename std::enable_if<IceBRG::is_stl_container<T>::value, char>::type = 0 >
typename T::value_type sum(const T & v)
{
	typename T::value_type result = 0;
	for(const auto & val : v)
	{
		result += sum(val);
	}
	return result;
}

#endif // Sum

// Product
#if (1)

template< typename T, typename std::enable_if<!IceBRG::is_stl_container<T>::value, char>::type = 0,
	typename std::enable_if<IceBRG::is_eigen_container<T>::value, char>::type = 0 >
typename T::Scalar product(const T & v)
{
	return v.prod();
}

template< typename T, typename std::enable_if<!IceBRG::is_stl_container<T>::value, char>::type = 0,
	typename std::enable_if<!IceBRG::is_eigen_container<T>::value, char>::type = 0 >
T product(const T v)
{
	return v;
}

template< typename T, typename std::enable_if<IceBRG::is_stl_container<T>::value, char>::type = 0 >
typename T::value_type product(const T &v)
{
	typename T::value_type result = 1;
	for(int_type i = 0; i < v.size(); i++)
	{
		result *= v[i];
	}
	return result;
}

#endif // Product

// Mean
#if (1)

template< typename T, typename std::enable_if<IceBRG::is_stl_container<T>::value, char>::type = 0 >
typename T::value_type mean(const std::vector<T> &v)
{
	if(rsize(v)==0) return 0;
	return sum(v)/rsize(v);
}

template< typename T, typename std::enable_if<!IceBRG::is_stl_container<T>::value, char>::type = 0,
	typename std::enable_if<IceBRG::is_eigen_container<T>::value, char>::type = 0 >
typename T::Scalar mean(const T & v)
{
	return v.mean();
}

template< typename T, typename std::enable_if<!IceBRG::is_stl_container<T>::value, char>::type = 0,
	typename std::enable_if<!IceBRG::is_eigen_container<T>::value, char>::type = 0 >
T mean(const T & v)
{
	return v;
}

#endif // Mean

// Standard Deviation
#if (1)

template< typename T, typename std::enable_if<IceBRG::is_stl_container<T>::value, char>::type = 0 >
typename T::value_type std(const T & v)
{
	if(rsize(v)<=1) return 0;

	return std::sqrt( subtract(sum( square(v) )/rsize(v), square(sum(v)/rsize(v)) ) );
}

template< typename T, typename std::enable_if<IceBRG::is_stl_container<T>::value, char>::type = 0 >
typename T::value_type stddev(const T & v)
{
	return std(v);
}

template< typename T, typename std::enable_if<!IceBRG::is_stl_container<T>::value, char>::type = 0,
	typename std::enable_if<IceBRG::is_eigen_container<T>::value, char>::type = 0 >
typename T::Scalar std(const T & v)
{
	if(v.size()<=1) return 0;

	return std::sqrt( sum(v.square())/v.size() - square(v.sum()/v.size()) );
}

template< typename T, typename std::enable_if<!IceBRG::is_stl_container<T>::value, char>::type = 0,
	typename std::enable_if<IceBRG::is_eigen_container<T>::value, char>::type = 0 >
typename T::Scalar stddev(const T & v)
{
	return std(v);
}

template< typename T, typename std::enable_if<!IceBRG::is_stl_container<T>::value, char>::type = 0,
	typename std::enable_if<!IceBRG::is_eigen_container<T>::value, char>::type = 0 >
T std(const T & v)
{
	return 0;
}

template< typename T, typename std::enable_if<!IceBRG::is_stl_container<T>::value, char>::type = 0,
	typename std::enable_if<!IceBRG::is_eigen_container<T>::value, char>::type = 0 >
T stddev(const T & v)
{
	return 0;
}

#endif // Standard Deviation

// Sample standard error
#if(1)

template< typename T, typename std::enable_if<IceBRG::is_stl_container<T>::value, char>::type = 0 >
typename T::value_type stderr(const T &v)
{
	auto size = rsize(v);
	if(size<=1) return std::numeric_limits<T>::max();

	return std(v)/std::sqrt(size-1);
}

template< typename T, typename std::enable_if<!IceBRG::is_stl_container<T>::value, char>::type = 0,
	typename std::enable_if<IceBRG::is_eigen_container<T>::value, char>::type = 0 >
typename T::Scalar stderr(const T & v)
{
	if(v.size()<=1) return std::numeric_limits<T>::max();

	return std(v)/std::sqrt(v.size()-1);
}

template< typename T, typename std::enable_if<!IceBRG::is_stl_container<T>::value, char>::type = 0,
	typename std::enable_if<!IceBRG::is_eigen_container<T>::value, char>::type = 0 >
T stderr(const T &v)
{
	return std::numeric_limits<T>::max();
}

#endif

// Monotonically increasing
#if(1)

template<typename InputIterator>
bool is_monotonically_increasing(const InputIterator &first, const InputIterator &last)
{

	// Check if container is empty
	if(first==last) return false;

	auto last_value = *first;
	InputIterator it=first;

	// Test the first step, to make sure the container is large enough
	++it;
	if(it==last)
	{
		// If we're here, the container only has one element
		return false;
	}
	if(*it<=last_value) return false;
	last_value = *it;

	// Now do remainder of steps
	for(++it; it!=last; ++it)
	{
		if(*it<=last_value) return false;
		last_value = *it;
	}
	return true;
}

template< typename T, typename std::enable_if<IceBRG::is_stl_container<T>::value ||
	IceBRG::is_eigen_container<T>::value, char>::type = 0 >
bool is_monotonically_increasing(const T &v)
{
	return is_monotonically_increasing(begin(v),end(v));
}

template<typename T, typename std::enable_if<!(IceBRG::is_stl_container<T>::value ||
IceBRG::is_eigen_container<T>::value), char>::type = 0 >
bool is_monotonically_increasing(const T & v)
{
	return false;
}

#endif // Monotonically increasing

// Monotonically decreasing
#if(1)

template<typename InputIterator>
bool is_monotonically_decreasing(const InputIterator &first, const InputIterator &last)
{
	typedef decltype(*first) T;

	// Check if container is empty
	if(first==last) return false;

	T last_value = *first;
	InputIterator it=first;

	// Test the first step, to make sure the container is large enough
	++it;
	if(it==last)
	{
		// If we're here, the container only has one element
		return false;
	}
	if(*it>=last_value) return false;
	last_value = *it;

	// Now do remainder of steps
	for(++it; it!=last; ++it)
	{
		if(*it>=last_value) return false;
		last_value = *it;
	}
	return true;
}

template< typename T, typename std::enable_if<IceBRG::is_stl_container<T>::value ||
	IceBRG::is_eigen_container<T>::value, char>::type = 0 >
bool is_monotonically_decreasing(const T &v)
{
	return is_monotonically_decreasing(begin(v),end(v));
}

template<typename T, typename std::enable_if<!(IceBRG::is_stl_container<T>::value ||
	IceBRG::is_eigen_container<T>::value), char>::type = 0 >
bool is_monotonically_decreasing(const T & v)
{
	return false;
}

#endif // Monotonically decreasing

// all_true
#if (1)
inline bool all_true(const std::vector<bool> & vec)
{
	for(const auto & v : vec)
		if(!v) return false;

	return true;
}

constexpr inline bool all_true(bool v)
{
	return v;
}
#endif // all_true

// all_false
#if (1)
inline bool all_false(const std::vector<bool> & vec)
{
	for(const auto & v : vec)
		if(v) return false;
	return true;
}

constexpr inline bool all_false(const bool & v)
{
	return (!v);
}
#endif // all_false

// not_all_true
#if (1)
inline bool not_all_true(const std::vector<bool> & v)
{
	return !all_true(v);
}

constexpr inline bool not_all_true(const bool & v)
{
	return !v;
}
#endif // not_all_true

// not_all_false
#if (1)
inline bool not_all_false(const std::vector<bool> & v)
{
	return !all_false(v);
}

constexpr inline bool not_all_false(bool v)
{
	return v;
}
#endif // all_false

// some_true_some_false
#if (1)
inline bool some_true_some_false(const std::vector<bool> & v)
{
	return ( (!all_true(v)) && (!all_false(v)) );
}

constexpr inline bool some_true_some_false(const bool & v)
{
	return false;
}
#endif // some_true_some_false

} // namespace IceBRG



#endif /* SUMMARY_FUNCTIONS_HPP_ */
