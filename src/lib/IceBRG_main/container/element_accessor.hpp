/**********************************************************************\
 @file element_accessor.hpp
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

#ifndef _BRG_CONTAINER_ELEMENT_ACCESSOR_HPP_INCLUDED_
#define _BRG_CONTAINER_ELEMENT_ACCESSOR_HPP_INCLUDED_

#include "IceBRG_main/utility.hpp"

namespace IceBRG {

template<typename ContainerType>
class element_accessor
{
private:

	ContainerType & _reference_;

public:

	// Some typedefs that we can allow users to access
	typedef typename ContainerType::value_type value_type;
	typedef typename ContainerType::allocator_type allocator_type;
	typedef typename ContainerType::reference reference;
	typedef typename ContainerType::const_reference const_reference;
	typedef typename ContainerType::pointer pointer;
	typedef typename ContainerType::const_pointer const_pointer;
	typedef typename ContainerType::iterator iterator;
	typedef typename ContainerType::const_iterator const_iterator;
	typedef typename ContainerType::reverse_iterator reverse_iterator;
	typedef typename ContainerType::const_reverse_iterator const_reverse_iterator;
	typedef typename ContainerType::difference_type difference_type;
	typedef typename ContainerType::size_type size_type;

	/// Constructor. Requires a non-const reference to a container of the proper type.
	element_accessor(ContainerType & reference)
	: _reference_(reference)
	{
	}

	/// Virtual destructor
	virtual ~element_accessor() {}

	/// Delete default copy and move constructors and assignments
#if(1)
	element_accessor(const element_accessor & other) = delete;
	element_accessor(element_accessor && other) = delete;
	element_accessor & operator=(const element_accessor & other) = delete;
	element_accessor & operator=(element_accessor && other) = delete;
#endif

	// Iterator methods
#if(1)
	/// begin
	const_iterator begin() const noexcept
	{
		return _reference_.begin();
	}
	/// begin
	iterator begin() noexcept
	{
		return _reference_.begin();
	}
	/// end
	const_iterator end() const noexcept
	{
		return _reference_.end();
	}
	/// end
	iterator end() noexcept
	{
		return _reference_.end();
	}
	/// rbegin
	const_iterator rbegin() const noexcept
	{
		return _reference_.rbegin();
	}
	/// rbegin
	iterator rbegin() noexcept
	{
		return _reference_.rbegin();
	}
	/// rend
	const_iterator rend() const noexcept
	{
		return _reference_.rend();
	}
	/// rend
	iterator rend() noexcept
	{
		return _reference_.rend();
	}
	/// cbegin
	const_iterator cbegin() const noexcept
	{
		return _reference_.cbegin();
	}
	/// cend
	const_iterator cend() const noexcept
	{
		return _reference_.cend();
	}
	/// crbegin
	const_iterator crbegin() const noexcept
	{
		return _reference_.crbegin();
	}
	/// crend
	const_iterator crend() const noexcept
	{
		return _reference_.crend();
	}

#endif // Iterator methods

	// Capacity methods
#if(1)

	/// size
	size_type size() const noexcept
	{
		return _reference_.size();
	}

	/// max_size
	size_type max_size() const noexcept
	{
		return _reference_.max_size();
	}

	/// capacity
	size_type capacity() const noexcept
	{
		return _reference_.capacity();
	}

	/// empty
	bool empty() const noexcept
	{
		return _reference_.empty();
	}

	// reserve disallowed - it can't be resized, so no reason to allow the user to increase capacity

	/// shrink_to_fit (allowed since there's a reason to let the user conserve memory)
	void shrink_to_fit() const
	{
		_reference_.shrink_to_fit();
	}

#endif // Capacity methods

	// Element access
#if(1)

	/// Element access
	const_reference operator[] (const size_type & n) const
	{
		return _reference_[n];
	}

	/// Element access
	reference operator[] (const size_type & n)
	{
		return _reference_[n];
	}

	/// Range-checked element access
	const_reference at( const size_type & n ) const
	{
		return _reference_.at(n);
	}

	/// Range-checked element access
	reference at( const size_type & n )
	{
		return _reference_.at(n);
	}

	/// Access first element
	const_reference front() const
	{
		return _reference_.front();
	}

	/// Access first element
	reference front()
	{
		return _reference_.front();
	}

	/// Access last element
	const_reference back() const
	{
		return _reference_.back();
	}

	/// Access last element
	reference back()
	{
		return _reference_.back();
	}

	/// Access data
	const value_type* data() const noexcept
	{
		return _reference_.data();
	}

	/// Access data
	value_type* data() noexcept
	{
		return _reference_.data();
	}

#endif // Element access

	// Casting
#if(1)

	/// Cast to ContainerType (the result will always be const so it can't be resized)
	operator const ContainerType &() const
	{
		return const_cast<const ContainerType &>(_reference_);
	}

#endif

};

}

#endif // _BRG_CONTAINER_ELEMENT_ACCESSOR_HPP_INCLUDED_
