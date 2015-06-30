/**********************************************************************\
 @file coerce.hpp
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

#ifndef _BRG_CONTAINER_COERCE_HPP_INCLUDED_
#define _BRG_CONTAINER_COERCE_HPP_INCLUDED_

#include <type_traits>
#include <iterator>

#include "IceBRG_main/container/is_eigen_container.hpp"
#include "IceBRG_main/utility.hpp"
#include "is_stl_container.hpp"

namespace IceBRG {

// Helper containers
#if(1)

// Assignment coercer - uses assignment to coerce
#if(1)

/**
 * assignment_coercer - a structure which performs coercion of one type to
 * another as a side-effect of its constructor.
 */
template<int_type d, typename container>
struct assignment_coercer
{
	assignment_coercer() = delete;
	assignment_coercer(const assignment_coercer &) = delete;
	assignment_coercer(assignment_coercer &&) = delete;
	assignment_coercer & operator=(const assignment_coercer &) = delete;
	assignment_coercer & operator=(assignment_coercer &&) = delete;

	template<typename oc, typename std::enable_if<(IceBRG::is_stl_container<oc>::value &&
	IceBRG::is_stl_container<container>::value),oc>::type* = nullptr>
	assignment_coercer(container & obj, const oc & other_obj)
	{
		obj.resize(other_obj.size());
		auto o_it = begin(other_obj);
		for(auto it=begin(obj); it!=end(obj); ++it, ++o_it)
		{
			assignment_coercer<d-1,decltype(*it)>(*it,*o_it);
		}
		return;
	}

	template<typename oc, typename std::enable_if<!(IceBRG::is_stl_container<oc>::value &&
		IceBRG::is_stl_container<container>::value),oc>::type* = nullptr>
	assignment_coercer(container & obj, const oc & other_obj)
	{
		obj.resize(other_obj.size());
		auto size = ssize(other_obj);
		for(decltype(size) i=0; i<size; ++i)
		{
			assignment_coercer<d-1,decltype(obj[i])>(obj[i],other_obj[i]);
		}
		return;
	}
};

/**
 * d=0 partial specialization of assignment_coercer. Uses simple assignment to
 * coerce.
 */
template<typename container>
struct assignment_coercer<0,container>
{
	assignment_coercer() = delete;
	assignment_coercer(const assignment_coercer &) = delete;
	assignment_coercer(assignment_coercer &&) = delete;
	assignment_coercer & operator=(const assignment_coercer &) = delete;
	assignment_coercer & operator=(assignment_coercer &&) = delete;

	template<typename oc>
	assignment_coercer(container & obj, const oc & other_obj)
	{
		obj = other_obj;
	}
};

#endif

// Range coercer - uses range constructor to coerce
#if(1)

/**
 * range_coercer - a structure which performs coercion of one type to
 * another as a side-effect of its constructor.
 */
template<int_type d, typename container>
struct range_coercer
{
	range_coercer() = delete;
	range_coercer(const range_coercer &) = delete;
	range_coercer(range_coercer &&) = delete;
	range_coercer & operator=(const range_coercer &) = delete;
	range_coercer & operator=(range_coercer &&) = delete;

	template<typename oc, typename std::enable_if<(IceBRG::is_stl_container<oc>::value &&
		IceBRG::is_stl_container<container>::value),oc>::type* = nullptr>
	range_coercer(container & obj, const oc & other_obj)
	{
		obj.resize(other_obj.size());
		auto o_it = begin(other_obj);
		for(auto it=begin(obj); it!=end(obj); ++it, ++o_it)
		{
			range_coercer<d-1,decltype(*it)>(*it,*o_it);
		}
		return;
	}

	template<typename oc, typename std::enable_if<!(IceBRG::is_stl_container<oc>::value &&
		IceBRG::is_stl_container<container>::value),oc>::type* = nullptr>
	range_coercer(container & obj, const oc & other_obj)
	{
		obj.resize(other_obj.size());
		auto size=ssize(other_obj);
		for(decltype(size) i=0; i<size; ++i)
		{
			range_coercer<d-1,decltype(obj[i])>(obj[i],other_obj[i]);
		}
		return;
	}
};

/**
 * d=1 partial specialization of range_coercer. Uses range constructor to
 * coerce.
 */
template<typename container>
struct range_coercer<1,container>
{
	range_coercer() = delete;
	range_coercer(const range_coercer &) = delete;
	range_coercer(range_coercer &&) = delete;
	range_coercer & operator=(const range_coercer &) = delete;
	range_coercer & operator=(range_coercer &&) = delete;

	template<typename oc>
	range_coercer(container & obj, const oc & other_obj)
	{
		obj = container(begin(other_obj),end(other_obj));
	}
};

/**
 * d=0 partial specialization of range_coercer. Uses simple assignment to
 * coerce.
 */
template<typename container>
struct range_coercer<0,container>
{
	range_coercer() = delete;
	range_coercer(const range_coercer &) = delete;
	range_coercer(range_coercer &&) = delete;
	range_coercer & operator=(const range_coercer &) = delete;
	range_coercer & operator=(range_coercer &&) = delete;

	template<typename oc>
	range_coercer(container & obj, const oc & other_obj)
	{
		obj = other_obj;
	}
};

#endif

#endif

/**
 * coerce - helper function which returns a coercion of old into NewType.
 *
 * @param old
 * @return
 */
template<typename NewType, int_type d=1, typename OldType=NewType>
NewType coerce(const OldType & old)
{
	NewType result;
	assignment_coercer<d,NewType>(result,old);
	return result;
}

/**
 * range_coerce - helper function which returns a coercion of old into NewType.
 * Use this version if the lowest level container has a range constructor for
 * a moderate performance gain.
 *
 * @param old
 * @return
 */
template<typename NewType, int_type d=1, typename OldType=NewType>
NewType range_coerce(const OldType & old)
{
	NewType result;
	range_coercer<d,NewType>(result,old);
	return result;
}

} // namespace IceBRG


#endif // _BRG_CONTAINER_COERCE_HPP_INCLUDED_
