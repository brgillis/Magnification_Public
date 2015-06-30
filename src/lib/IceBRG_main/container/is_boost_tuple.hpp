/**********************************************************************\
 @file is_boost_tuple.hpp
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

#ifndef BRG_CONTAINER_IS_BOOST_TUPLE_HPP_
#define BRG_CONTAINER_IS_BOOST_TUPLE_HPP_

#include <type_traits>
#include <utility>

#include <boost/tti/has_member_function.hpp>
#include <boost/tuple/tuple.hpp>

namespace IceBRG {

template<typename T>
struct _is_boost_tuple : std::false_type
{};

template <class T0, class T1, class T2, class T3, class T4,
          class T5, class T6, class T7, class T8, class T9>
struct _is_boost_tuple<boost::tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>>
	: std::true_type
{};

template <class TH, class TT>
struct _is_boost_tuple<boost::tuples::cons<TH,TT>>
	: std::true_type
{};
template <>
struct _is_boost_tuple<boost::tuples::null_type>
	: std::true_type
{};

template<typename T>
struct is_boost_tuple
	: _is_boost_tuple<typename std::decay<T>::type>
{};

static_assert(is_boost_tuple<boost::tuple<double,double>>::value,
		"ERROR: is_boost_tuple isn't working for tuples.");
static_assert(is_boost_tuple<boost::tuple<double,double> &>::value,
		"ERROR: is_boost_tuple isn't working for tuple references.");
static_assert(is_boost_tuple<boost::tuples::cons<double, boost::tuples::null_type>>::value,
		"ERROR: is_boost_tuple isn't cons lists.");
static_assert(is_boost_tuple<boost::tuples::cons<double, boost::tuples::null_type> &>::value,
		"ERROR: is_boost_tuple isn't cons list references.");

template<typename T>
struct _is_null_type : std::false_type
{};

template <>
struct _is_null_type<boost::tuples::null_type>
	: std::true_type
{};

template<typename T>
struct is_null_type
	: _is_null_type<typename std::decay<T>::type>
{};

} // namespace IceBRG

#endif // BRG_CONTAINER_IS_BOOST_TUPLE_HPP_
