/**********************************************************************\
 @file labeled_array_row_reference.hpp
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

#ifndef _BRG_CONTAINER_LABELED_ARRAY_ROW_REFERENCE_HPP_INCLUDED_
#define _BRG_CONTAINER_LABELED_ARRAY_ROW_REFERENCE_HPP_INCLUDED_

#include <cstdlib>
#include <utility>

#include <boost/type_traits/is_convertible.hpp>

#include "IceBRG_main/container/labeled_array.hpp"
#include "IceBRG_main/utility.hpp"

namespace IceBRG {

template<typename labeled_array_type, typename T_row_type>
class labeled_array_row_reference
{
public:

	typedef typename labeled_array_type::label_type label_type;
	typedef typename labeled_array_type::value_type value_type;
	typedef typename labeled_array_type::const_value_type const_value_type;
	typedef typename labeled_array_type::size_type size_type;

	typedef T_row_type row_type;
	typedef typename labeled_array_type::const_row_type const_row_type;

	typedef typename labeled_array_type::reference reference;
	typedef typename labeled_array_type::const_reference const_reference;

	typedef typename labeled_array_type::row_element_iterator iterator;
	typedef typename labeled_array_type::const_row_element_iterator const_iterator;
	typedef typename labeled_array_type::reverse_row_element_iterator reverse_iterator;
	typedef typename labeled_array_type::const_reverse_row_element_iterator const_reverse_iterator;

	typedef size_t difference_type;

private:

	// Private typedefs
	typedef typename labeled_array_type::map_type map_type;

	// Members
	map_type * _label_map_;
	row_type _row_;
	size_type _num_rows_;

public:

	/// Constructor. Requires a pointer to a labeled_array's label map and row
	template <typename T_init_row_type>
	labeled_array_row_reference(map_type * label_map, T_init_row_type && row, const size_type & num_cols)
	: _label_map_(label_map),
	  _row_(std::forward<T_init_row_type>(row)),
	  _num_rows_(num_cols)
	{
	}

	/// Virtual destructor
	virtual ~labeled_array_row_reference() {}

	// Iterator methods
#if(1)
	/// begin (const)
	template< typename T_iterator = const_iterator,
		typename std::enable_if<!std::is_same<T_iterator,const_value_type *>::value,char>::type = 0>
	T_iterator begin() const noexcept
	{
		return T_iterator(_row_.data(),_num_rows_);
	}
	template< typename T_iterator = const_iterator,
		typename std::enable_if<std::is_same<T_iterator,const_value_type *>::value,char>::type = 0>
	T_iterator begin() const noexcept
	{
		return T_iterator(_row_.data());
	}

	/// begin
	template< typename T_iterator = iterator,
		typename std::enable_if<!std::is_same<T_iterator,value_type *>::value,char>::type = 0>
	T_iterator begin() noexcept
	{
		return T_iterator(_row_.data(),_num_rows_);
	}
	template< typename T_iterator = iterator,
		typename std::enable_if<std::is_same<T_iterator,value_type *>::value,char>::type = 0>
	T_iterator begin() noexcept
	{
		return T_iterator(_row_.data());
	}

	/// end (const)
	template< typename T_iterator = const_iterator,
		typename std::enable_if<!std::is_same<T_iterator,const_value_type *>::value,char>::type = 0>
	T_iterator end() const noexcept
	{
		return T_iterator(_row_.data()+_row_.size()*_num_rows_,_num_rows_);
	}
	template< typename T_iterator = const_iterator,
		typename std::enable_if<std::is_same<T_iterator,const_value_type *>::value,char>::type = 0>
	T_iterator end() const noexcept
	{
		return T_iterator(_row_.data()+_row_.size());
	}

	/// end
	template< typename T_iterator = iterator,
		typename std::enable_if<!std::is_same<T_iterator,value_type *>::value,char>::type = 0>
	T_iterator end() noexcept
	{
		return T_iterator(_row_.data()+_row_.size()*_num_rows_,_num_rows_);
	}
	template< typename T_iterator = iterator,
		typename std::enable_if<std::is_same<T_iterator,value_type *>::value,char>::type = 0>
	T_iterator end() noexcept
	{
		return T_iterator(_row_.data()+_row_.size());
	}

	/// rbegin (const)
	template< typename T_iterator = const_reverse_iterator,
		typename std::enable_if<!std::is_same<T_iterator,const_value_type *>::value,char>::type = 0>
	T_iterator rbegin() const noexcept
	{
		return T_iterator(_row_.data()+_row_.size()*_num_rows_-_num_rows_,_num_rows_);
	}
	template< typename T_iterator = const_reverse_iterator,
		typename std::enable_if<std::is_same<T_iterator,const_value_type *>::value,char>::type = 0>
	T_iterator rbegin() const noexcept
	{
		return T_iterator(_row_.data()+_row_.size()-1);
	}

	/// rbegin
	template< typename T_iterator = reverse_iterator,
		typename std::enable_if<!std::is_same<T_iterator,value_type *>::value,char>::type = 0>
	T_iterator rbegin() noexcept
	{
		return T_iterator(_row_.data()+_row_.size()*_num_rows_-_num_rows_,_num_rows_);
	}
	template< typename T_iterator = reverse_iterator,
		typename std::enable_if<std::is_same<T_iterator,value_type *>::value,char>::type = 0>
	T_iterator rbegin() noexcept
	{
		return T_iterator(_row_.data()+_row_.size()-1);
	}

	/// rend (const)
	template< typename T_iterator = const_reverse_iterator,
		typename std::enable_if<!std::is_same<T_iterator,const_value_type *>::value,char>::type = 0>
	T_iterator rend() const noexcept
	{
		return T_iterator(_row_.data()-_num_rows_,_num_rows_);
	}
	template< typename T_iterator = const_reverse_iterator,
		typename std::enable_if<std::is_same<T_iterator,const_value_type *>::value,char>::type = 0>
	T_iterator rend() const noexcept
	{
		return T_iterator(_row_.data()-1);
	}

	/// rend
	template< typename T_iterator = reverse_iterator,
		typename std::enable_if<!std::is_same<T_iterator,value_type *>::value,char>::type = 0>
	T_iterator rend() const noexcept
	{
		return T_iterator(_row_.data()-_num_rows_,_num_rows_);
	}
	template< typename T_iterator = reverse_iterator,
		typename std::enable_if<std::is_same<T_iterator,value_type *>::value,char>::type = 0>
	T_iterator rend() const noexcept
	{
		return T_iterator(_row_.data()-1);
	}

	/// cbegin
	template< typename T_iterator = const_iterator,
		typename std::enable_if<!std::is_same<T_iterator,const_value_type *>::value,char>::type = 0>
	T_iterator cbegin() const noexcept
	{
		return T_iterator(_row_.data(),_num_rows_);
	}
	template< typename T_iterator = const_iterator,
		typename std::enable_if<std::is_same<T_iterator,const_value_type *>::value,char>::type = 0>
	T_iterator cbegin() const noexcept
	{
		return T_iterator(_row_.data());
	}

	/// cend
	template< typename T_iterator = const_iterator,
		typename std::enable_if<!std::is_same<T_iterator,const_value_type *>::value,char>::type = 0>
	T_iterator cend() const noexcept
	{
		return T_iterator(_row_.data()+_row_.size()*_num_rows_,_num_rows_);
	}
	template< typename T_iterator = const_iterator,
		typename std::enable_if<std::is_same<T_iterator,const_value_type *>::value,char>::type = 0>
	T_iterator cend() const noexcept
	{
		return T_iterator(_row_.data()+_row_.size());
	}

	/// crbegin
	template< typename T_iterator = const_reverse_iterator,
		typename std::enable_if<!std::is_same<T_iterator,const_value_type *>::value,char>::type = 0>
	T_iterator crbegin() const noexcept
	{
		return T_iterator(_row_.data()+_row_.size()*_num_rows_-_num_rows_,_num_rows_);
	}
	template< typename T_iterator = const_reverse_iterator,
		typename std::enable_if<std::is_same<T_iterator,const_value_type *>::value,char>::type = 0>
	T_iterator crbegin() const noexcept
	{
		return T_iterator(_row_.data()+_row_.size()-1);
	}

	/// crend
	template< typename T_iterator = const_reverse_iterator,
		typename std::enable_if<!std::is_same<T_iterator,const_value_type *>::value,char>::type = 0>
	T_iterator crend() const noexcept
	{
		return T_iterator(_row_.data()-_num_rows_,_num_rows_);
	}
	template< typename T_iterator = const_reverse_iterator,
		typename std::enable_if<std::is_same<T_iterator,const_value_type *>::value,char>::type = 0>
	T_iterator crend() const noexcept
	{
		return T_iterator(_row_.data()-1);
	}

#endif // Iterator methods

	// Size methods
#if(1)

	/// size
	size_type size() const noexcept
	{
		return _row_.size();
	}

	/// empty
	bool empty() const noexcept
	{
		return _row_.empty();
	}

#endif // Capacity methods

	// Element access
#if(1)

	/// Element access
	value_type operator[] (const size_type & n) const
	{
		return _row_[n];
	}

	/// Element access
	reference operator[] (const size_type & n)
	{
		return _row_[n];
	}

	/// Range-checked element access
	value_type operator()( const size_type & n ) const
	{
		return _row_(n);
	}

	/// Range-checked element access
	reference operator()( const size_type & n )
	{
		return _row_(n);
	}

	/// Range-checked element access
	value_type at( const size_type & n ) const
	{
		return _row_(n);
	}

	/// Range-checked element access
	reference at( const size_type & n )
	{
		return _row_(n);
	}

	/// Range-checked element access
	value_type col( const size_type & n ) const
	{
		return _row_(n);
	}

	/// Range-checked element access
	reference col( const size_type & n )
	{
		return _row_(n);
	}

	/// Range-checked element access
	value_type at_label( const label_type & label ) const
	{
		return _row_(_label_map_->left.at(label));
	}

	/// Range-checked element access
	reference at_label( const label_type & label )
	{
		return _row_(_label_map_->left.at(label));
	}

	/// Access first element
	value_type front() const
	{
		return _row_(0);
	}

	/// Access first element
	reference front()
	{
		return _row_(0);
	}

	/// Access last element
	value_type back() const
	{
		return _row_(_row_.size()-1);
	}

	/// Access last element
	reference back()
	{
		return _row_(_row_.size()-1);
	}

	/// Access data
	const value_type* data() const noexcept
	{
		return _row_.data();
	}

	/// Access data
	value_type* data() noexcept
	{
		return _row_.data();
	}

#endif // Element access

	// Label access
#if(1)

	label_type label(const size_type & n) const
	{
		return _label_map_->right.at(n);
	}

#endif

	// Casting
#if(1)

	/// Cast to row_type
	operator const_row_type() const noexcept
	{
		return _row_;
	}
	operator row_type() noexcept
	{
		return _row_;
	}
	const_row_type raw() const noexcept
	{
		return _row_;
	}
	row_type raw() noexcept
	{
		return _row_;
	}

	/// Cast non-const version to const version
	template <typename other_row_type,
	typename std::enable_if<std::is_convertible<other_row_type,row_type>::value, other_row_type>::type* = nullptr>
	labeled_array_row_reference( const labeled_array_row_reference<labeled_array_type,other_row_type> & other)
	: _label_map_(other._label_map_), _row_(other._row_), _num_rows_(other._num_rows_) {}

#endif

};

}



#endif // _BRG_CONTAINER_LABELED_ARRAY_ROW_REFERENCE_HPP_INCLUDED_
