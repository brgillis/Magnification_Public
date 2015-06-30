/**********************************************************************\
 @file labeled_array_element_iterator.hpp
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

// body file: labeled_array_element_iterator.cpp

#ifndef _BRG_BRG_CONTAINER_LABELED_ARRAY_ELEMENT_ITERATOR_HPP_INCLUDED_
#define _BRG_BRG_CONTAINER_LABELED_ARRAY_ELEMENT_ITERATOR_HPP_INCLUDED_

#include <boost/iterator/iterator_facade.hpp>

namespace IceBRG {

template< typename value_type >
class labeled_array_element_iterator
	: public boost::iterator_facade<
	  labeled_array_element_iterator<value_type>, // CRTP
		value_type, // Value type
		boost::random_access_traversal_tag> // Traversal type
{
private:
	// Private typedefs
	typedef typename boost::iterator_facade<labeled_array_element_iterator<value_type>,
		value_type, boost::random_access_traversal_tag> base;
public:
	// Public typedefs
	typedef typename base::difference_type difference_type;

private:
	value_type * _ptr_;
	const int_type _multiplier_;

	friend class boost::iterator_core_access;

	// Implementation of necessary operations
#if(1)

    void increment() { _ptr_ += _multiplier_; }
    void decrement() { _ptr_ -= _multiplier_; }
    void advance(const difference_type & n) { _ptr_ += _multiplier_*n; }

    bool equal(const labeled_array_element_iterator<value_type> & other) const
    {
        return (_ptr_ == other._ptr_) &&
        	(_multiplier_ == other._multiplier_);
    }

    difference_type distance_to(
    	const labeled_array_element_iterator<value_type> & other) const
    {
    	return (other._ptr_-_ptr_)/_multiplier_;
    }

    value_type & dereference() const { return *_ptr_; }

#endif

public:
    labeled_array_element_iterator()
	: _ptr_(nullptr), _multiplier_(1)
	{
	}

    labeled_array_element_iterator(value_type * ptr, const int_type & multiplier = 1)
	: _ptr_(ptr), _multiplier_(multiplier)
	{
	}

	template <typename T_o_value_type>
	labeled_array_element_iterator(
		const labeled_array_element_iterator<value_type> & other,
		typename std::enable_if<std::is_convertible<T_o_value_type,value_type>::value, T_o_value_type *>::type = nullptr
	)
	: _ptr_(other._ptr_), _multiplier_(other.multiplier) {}

};

}


#endif // _BRG_BRG_CONTAINER_LABELED_ARRAY_ELEMENT_ITERATOR_HPP_INCLUDED_
