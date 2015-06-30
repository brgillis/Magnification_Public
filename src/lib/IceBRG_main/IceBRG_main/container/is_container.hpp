/**********************************************************************\
 @file is_container.hpp
 ------------------

 General type-checking for if something is a container, tuple, or not.

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

#ifndef BRG_CONTAINER_IS_CONTAINER_HPP_
#define BRG_CONTAINER_IS_CONTAINER_HPP_

#include <type_traits>

#include "IceBRG_main/container/is_boost_tuple.hpp"
#include "IceBRG_main/container/is_eigen_container.hpp"
#include "IceBRG_main/container/is_stl_container.hpp"

namespace IceBRG {

// Structures to test generally if something is a container, boost tuple, or not
#if(1)

template<typename T>
struct is_container : std::integral_constant<bool,
	is_stl_container<T>::value || is_eigen_container<T>::value>
{ };

template<typename T>
struct is_container_or_boost_tuple : std::integral_constant<bool,
is_container<T>::value || is_boost_tuple<T>::value>
{ };

template<typename T>
struct is_scalar : std::integral_constant<bool,
!is_container_or_boost_tuple<T>::value>
{ };

template<typename T>
struct is_scalar_or_eigen : std::integral_constant<bool,
!(is_stl_container<T>::value || is_boost_tuple<T>::value)>
{ };

#endif

// Macros to be used in templates for enable_if, to save text and keep things uniform
#if(1)

// For STL and Eigen
#define BRG_S_BOTH_STL(T1,T2) typename std::enable_if<IceBRG::is_stl_container<T1>::value && \
	IceBRG::is_stl_container<T2>::value>::type
#define BRG_S_FIRST_STL(T1,T2) typename std::enable_if<IceBRG::is_stl_container<T1>::value && \
	IceBRG::is_scalar_or_eigen<T2>::value>::type
#define BRG_S_SECOND_STL(T1,T2) typename std::enable_if<IceBRG::is_stl_container<T2>::value && \
	IceBRG::is_scalar_or_eigen<T1>::value>::type
#define BRG_S_NEITHER_STL(T1,T2) typename std::enable_if<IceBRG::is_scalar_or_eigen<T1>::value && \
	IceBRG::is_scalar_or_eigen<T2>::value>::type

#define BRG_S_IS_EIGEN(T1) typename std::enable_if<IceBRG::is_eigen_container<T1>::value>::type
#define BRG_S_IS_EIGEN_OR_SCALAR(T1) typename std::enable_if<IceBRG::is_scalar_or_eigen<T1>::value>::type

#define BRG_S_IS_STL_OR_EIGEN(T1) typename std::enable_if<IceBRG::is_container<T1>::value>::type

#define BRG_S_IS_STL(T1) typename std::enable_if<IceBRG::is_stl_container<T1>::value>::type

#define BRG_F_BOTH_STL(T1,T2) typename std::enable_if<IceBRG::is_stl_container<T1>::value && \
	IceBRG::is_stl_container<T2>::value,char>::type = 0
#define BRG_F_FIRST_STL(T1,T2) typename std::enable_if<IceBRG::is_stl_container<T1>::value && \
	IceBRG::is_scalar_or_eigen<T2>::value,char>::type = 0
#define BRG_F_SECOND_STL(T1,T2) typename std::enable_if<IceBRG::is_stl_container<T2>::value && \
	IceBRG::is_scalar_or_eigen<T1>::value,char>::type = 0
#define BRG_F_NEITHER_STL(T1,T2) typename std::enable_if<IceBRG::is_scalar_or_eigen<T1>::value && \
	IceBRG::is_scalar_or_eigen<T2>::value,char>::type = 0

#define BRG_F_IS_EIGEN(T1) typename std::enable_if<IceBRG::is_eigen_container<T1>::value,char>::type = 0
#define BRG_F_IS_EIGEN_OR_SCALAR(T1) typename std::enable_if<IceBRG::is_scalar_or_eigen<T1>::value,char>::type = 0

#define BRG_F_IS_STL_OR_EIGEN(T1) typename std::enable_if<IceBRG::is_container<T1>::value,char>::type = 0

#define BRG_F_IS_STL(T1) typename std::enable_if<IceBRG::is_stl_container<T1>::value,char>::type = 0

// For tuples
#define BRG_S_IS_TUPLE(T1) typename std::enable_if<IceBRG::is_boost_tuple<T1>::value && \
	!IceBRG::is_null_type<T1>::value>::type
#define BRG_S_NOT_TUPLE(T1) typename std::enable_if<!IceBRG::is_boost_tuple<T1>::value>::type
#define BRG_S_IS_NULL(T1) typename std::enable_if<IceBRG::is_null_type<T1>::value>::type
#define BRG_S_EITHER_IS_NULL(T1,T2) typename std::enable_if<IceBRG::is_null_type<T1>::value || \
	IceBRG::is_null_type<T2>::value>::type

#define BRG_F_IS_TUPLE(T1) typename std::enable_if<IceBRG::is_boost_tuple<T1>::value && \
	!IceBRG::is_null_type<T1>::value,char>::type = 0
#define BRG_F_NOT_TUPLE(T1) typename std::enable_if<!IceBRG::is_boost_tuple<T1>::value,char>::type = 0
#define BRG_F_IS_NULL(T1) typename std::enable_if<IceBRG::is_null_type<T1>::value,char>::type = 0
#define BRG_F_EITHER_IS_NULL(T1,T2) typename std::enable_if<IceBRG::is_null_type<T1>::value || \
	IceBRG::is_null_type<T2>::value,char>::type = 0

// For multiple checks
#define BRG_S_IS_CONTAINER(T1) typename std::enable_if<IceBRG::is_container<T1>::value>::type
#define BRG_S_IS_CONTAINER_OR_TUPLE(T1) typename std::enable_if< \
	IceBRG::is_container_or_boost_tuple<T1>::value>::type
#define BRG_S_NOT_CONTAINER(T1) typename std::enable_if<IceBRG::is_scalar<T1>::value>::type

#define BRG_F_IS_CONTAINER(T1) typename std::enable_if<IceBRG::is_container<T1>::value,char>::type = 0
#define BRG_F_IS_CONTAINER_OR_TUPLE(T1) typename std::enable_if< \
	IceBRG::is_container_or_boost_tuple<T1>::value,char>::type = 0
#define BRG_F_NOT_CONTAINER(T1) typename std::enable_if<IceBRG::is_scalar<T1>::value,char>::type = 0

#endif

} // namespace IceBRG

#endif // BRG_CONTAINER_IS_CONTAINER_HPP_
