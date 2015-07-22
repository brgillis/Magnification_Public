/**********************************************************************\
 @file labeled_array_col_reference.hpp
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

#ifndef _BRG_CONTAINER_LABELED_ARRAY_COL_REFERENCE_HPP_INCLUDED_
#define _BRG_CONTAINER_LABELED_ARRAY_COL_REFERENCE_HPP_INCLUDED_

#include <utility>

#include <boost/type_traits/is_convertible.hpp>

#include "IceBRG_main/container/labeled_array.hpp"
#include "IceBRG_main/utility.hpp"

namespace IceBRG {

template<typename labeled_array_type, typename T_col_type>
class labeled_array_col_reference
{

public:

	// Public typedefs

	typedef typename labeled_array_type::const_label_type label_type;
	typedef typename labeled_array_type::value_type value_type;
	typedef typename labeled_array_type::const_value_type const_value_type;
	typedef typename labeled_array_type::size_type size_type;

	typedef T_col_type col_type;
	typedef const col_type const_col_type;

	typedef typename labeled_array_type::reference reference;
	typedef typename labeled_array_type::const_reference const_reference;

	typedef typename labeled_array_type::col_element_iterator iterator;
	typedef typename labeled_array_type::const_col_element_iterator const_iterator;
	typedef typename labeled_array_type::reverse_col_element_iterator reverse_iterator;
	typedef typename labeled_array_type::const_reverse_col_element_iterator const_reverse_iterator;


	typedef size_t difference_type;

private:

	// Private Members
	label_type * _label_;
	col_type _col_;
	size_type _num_cols_;

public:

	/// Constructor. Requires a pointer to a labeled_array's label, the column, and the number of rows
	labeled_array_col_reference(labeled_array_type * array, const size_type & col,
			const size_type & num_cols)
	: _label_(&array->_label_map_.right.at(col)),
	  _col_(array->raw_col(col)),
	  _num_cols_(num_cols)
	{
	}

	/// Virtual destructor
	virtual ~labeled_array_col_reference() {}

	// Iterator methods
#if(1)
	/// begin (const)
	template< typename T_iterator = const_iterator,
		typename std::enable_if<!std::is_same<T_iterator,const_value_type *>::value,char>::type = 0>
	T_iterator begin() const noexcept
	{
		return T_iterator(_col_.data(),_num_cols_);
	}
	template< typename T_iterator = const_iterator,
		typename std::enable_if<std::is_same<T_iterator,const_value_type *>::value,char>::type = 0>
	T_iterator begin() const noexcept
	{
		return T_iterator(_col_.data());
	}

	/// begin
	template< typename T_iterator = iterator,
		typename std::enable_if<!std::is_same<T_iterator,value_type *>::value,char>::type = 0>
	T_iterator begin() noexcept
	{
		return T_iterator(_col_.data(),_num_cols_);
	}
	template< typename T_iterator = iterator,
		typename std::enable_if<std::is_same<T_iterator,value_type *>::value,char>::type = 0>
	T_iterator begin() noexcept
	{
		return T_iterator(_col_.data());
	}

	/// end (const)
	template< typename T_iterator = const_iterator,
		typename std::enable_if<!std::is_same<T_iterator,const_value_type *>::value,char>::type = 0>
	T_iterator end() const noexcept
	{
		return T_iterator(_col_.data()+_col_.size()*_num_cols_,_num_cols_);
	}
	template< typename T_iterator = const_iterator,
		typename std::enable_if<std::is_same<T_iterator,const_value_type *>::value,char>::type = 0>
	T_iterator end() const noexcept
	{
		return T_iterator(_col_.data()+_col_.size());
	}

	/// end
	template< typename T_iterator = iterator,
		typename std::enable_if<!std::is_same<T_iterator,value_type *>::value,char>::type = 0>
	T_iterator end() noexcept
	{
		return T_iterator(_col_.data()+_col_.size()*_num_cols_,_num_cols_);
	}
	template< typename T_iterator = iterator,
		typename std::enable_if<std::is_same<T_iterator,value_type *>::value,char>::type = 0>
	T_iterator end() noexcept
	{
		return T_iterator(_col_.data()+_col_.size());
	}

	/// rbegin (const)
	template< typename T_iterator = const_reverse_iterator,
		typename std::enable_if<!std::is_same<T_iterator,const_value_type *>::value,char>::type = 0>
	T_iterator rbegin() const noexcept
	{
		return T_iterator(_col_.data()+_col_.size()*_num_cols_-_num_cols_,_num_cols_);
	}
	template< typename T_iterator = const_reverse_iterator,
		typename std::enable_if<std::is_same<T_iterator,const_value_type *>::value,char>::type = 0>
	T_iterator rbegin() const noexcept
	{
		return T_iterator(_col_.data()+_col_.size()-1);
	}

	/// rbegin
	template< typename T_iterator = reverse_iterator,
		typename std::enable_if<!std::is_same<T_iterator,value_type *>::value,char>::type = 0>
	T_iterator rbegin() noexcept
	{
		return T_iterator(_col_.data()+_col_.size()*_num_cols_-_num_cols_,_num_cols_);
	}
	template< typename T_iterator = reverse_iterator,
		typename std::enable_if<std::is_same<T_iterator,value_type *>::value,char>::type = 0>
	T_iterator rbegin() noexcept
	{
		return T_iterator(_col_.data()+_col_.size()-1);
	}

	/// rend (const)
	template< typename T_iterator = const_reverse_iterator,
		typename std::enable_if<!std::is_same<T_iterator,const_value_type *>::value,char>::type = 0>
	T_iterator rend() const noexcept
	{
		return T_iterator(_col_.data()-_num_cols_,_num_cols_);
	}
	template< typename T_iterator = const_reverse_iterator,
		typename std::enable_if<std::is_same<T_iterator,const_value_type *>::value,char>::type = 0>
	T_iterator rend() const noexcept
	{
		return T_iterator(_col_.data()-1);
	}

	/// rend
	template< typename T_iterator = reverse_iterator,
		typename std::enable_if<!std::is_same<T_iterator,value_type *>::value,char>::type = 0>
	T_iterator rend() const noexcept
	{
		return T_iterator(_col_.data()-_num_cols_,_num_cols_);
	}
	template< typename T_iterator = reverse_iterator,
		typename std::enable_if<std::is_same<T_iterator,value_type *>::value,char>::type = 0>
	T_iterator rend() const noexcept
	{
		return T_iterator(_col_.data()-1);
	}

	/// cbegin
	template< typename T_iterator = const_iterator,
		typename std::enable_if<!std::is_same<T_iterator,const_value_type *>::value,char>::type = 0>
	T_iterator cbegin() const noexcept
	{
		return T_iterator(_col_.data(),_num_cols_);
	}
	template< typename T_iterator = const_iterator,
		typename std::enable_if<std::is_same<T_iterator,const_value_type *>::value,char>::type = 0>
	T_iterator cbegin() const noexcept
	{
		return T_iterator(_col_.data());
	}

	/// cend
	template< typename T_iterator = const_iterator,
		typename std::enable_if<!std::is_same<T_iterator,const_value_type *>::value,char>::type = 0>
	T_iterator cend() const noexcept
	{
		return T_iterator(_col_.data()+_col_.size()*_num_cols_,_num_cols_);
	}
	template< typename T_iterator = const_iterator,
		typename std::enable_if<std::is_same<T_iterator,const_value_type *>::value,char>::type = 0>
	T_iterator cend() const noexcept
	{
		return T_iterator(_col_.data()+_col_.size());
	}

	/// crbegin
	template< typename T_iterator = const_reverse_iterator,
		typename std::enable_if<!std::is_same<T_iterator,const_value_type *>::value,char>::type = 0>
	T_iterator crbegin() const noexcept
	{
		return T_iterator(_col_.data()+_col_.size()*_num_cols_-_num_cols_,_num_cols_);
	}
	template< typename T_iterator = const_reverse_iterator,
		typename std::enable_if<std::is_same<T_iterator,const_value_type *>::value,char>::type = 0>
	T_iterator crbegin() const noexcept
	{
		return T_iterator(_col_.data()+_col_.size()-1);
	}

	/// crend
	template< typename T_iterator = const_reverse_iterator,
		typename std::enable_if<!std::is_same<T_iterator,const_value_type *>::value,char>::type = 0>
	T_iterator crend() const noexcept
	{
		return T_iterator(_col_.data()-_num_cols_,_num_cols_);
	}
	template< typename T_iterator = const_reverse_iterator,
		typename std::enable_if<std::is_same<T_iterator,const_value_type *>::value,char>::type = 0>
	T_iterator crend() const noexcept
	{
		return T_iterator(_col_.data()-1);
	}

#endif // Iterator methods

	// Size methods
#if(1)

	/// size
	size_type size() const noexcept
	{
		return _col_.size();
	}

	/// empty
	bool empty() const noexcept
	{
		return _col_.size()==0;
	}

#endif // Capacity methods

	// Element access
#if(1)

	/// Element access
	const_reference operator[] (const size_type & n) const
	{
		return _col_[n];
	}

	/// Element access
	reference operator[] (const size_type & n)
	{
		return _col_[n];
	}

	/// Range-checked element access
	const_reference row( const size_type & n ) const
	{
		return _col_[n];
	}

	/// Range-checked element access
	reference row( const size_type & n )
	{
		return _col_[n];
	}

	/// Range-checked element access
	const_reference at( const size_type & n ) const
	{
		return _col_[n];
	}

	/// Range-checked element access
	reference at( const size_type & n )
	{
		return _col_[n];
	}

	/// Range-checked element access
	const_reference operator()( const size_type & n ) const
	{
		return _col_[n];
	}

	/// Range-checked element access
	reference operator()( const size_type & n )
	{
		return _col_[n];
	}

	/// Access first element
	const_reference front() const
	{
		return _col_[0];
	}

	/// Access first element
	reference front()
	{
		return _col_[0];
	}

	/// Access last element
	const_reference back() const
	{
		return _col_[_col_.size()-1];
	}

	/// Access last element
	reference back()
	{
		return _col_[_col_.size()-1];
	}

	/// Access data
	const value_type* data() const noexcept
	{
		return _col_.data();
	}

	/// Access data
	value_type* data() noexcept
	{
		return _col_.data();
	}

#endif // Element access

	// Label access
#if(1)
	/// Get the label
	const label_type & label() const noexcept
	{
		return *_label_;
	}
#endif

	// Casting
#if(1)

	/// Cast to col_type
	operator const_col_type() const noexcept
	{
		return _col_;
	}
	operator col_type() noexcept
	{
		return _col_;
	}
	const_col_type raw() const noexcept
	{
		return _col_;
	}
	col_type raw() noexcept
	{
		return _col_;
	}

	/// Cast non-const version to const version
	template <typename other_col_type,
	typename std::enable_if<std::is_convertible<other_col_type,col_type>::value, other_col_type>::type* = nullptr>
	labeled_array_col_reference( const labeled_array_col_reference<labeled_array_type,other_col_type> & other)
	: _label_(other._label_), _col_(other._col_), _num_cols_(other._num_cols_)  {}

#endif

};

}



#endif // _BRG_CONTAINER_LABELED_ARRAY_COL_REFERENCE_HPP_INCLUDED_
