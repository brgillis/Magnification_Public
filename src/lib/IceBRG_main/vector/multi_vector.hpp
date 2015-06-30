/**********************************************************************\
  @file vector.hpp

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

#ifndef _BRG_VECTOR_HPP_INCLUDED_
#define _BRG_VECTOR_HPP_INCLUDED_

#include <vector>
#include <iterator>
#include <stdexcept>
#include <memory>

#include "IceBRG_main/utility.hpp"
#include "IceBRG_main/vector/elementwise_functions.hpp"
#include "IceBRG_main/vector/summary_functions.hpp"

namespace IceBRG {

template < class T, class A = std::allocator<T> >
class multi_vector: private std::vector<T,A>
{
public:
	// Some typedefs that we can allow users to access
	typedef typename std::vector<T,A>::size_type vsize_t;
	typedef int_type dsize_t;
	typedef int_type ssize_t;
	typedef std::vector< int_type > shape_t;
private:
	typedef std::vector<T,A> vec;

	dsize_t _num_dim_;
	shape_t _shape_;

	vsize_t _get_p(const shape_t & position) const
	{
		vsize_t p = 0;
		vsize_t m = 1;

		for(ssize_t i=0; i<position.size(); i++ )
		{
			p += m * position[i];
			m *= _shape_[i];
		}
		return p;
	}

	vsize_t _get_p(const multi_vector<int_type> & position) const
	{
		vsize_t p = 0;
		vsize_t m = 1;

		for(ssize_t i=0; i<position.size(); i++ )
		{
			p += m * position[i];
			m *= _shape_[i];
		}
		return p;
	}
	// Soft resize - only affects internal variables to this class, not the data
	void _resize (vsize_t n)
	{
		_num_dim_ = 1;
		_shape_.resize(_num_dim_);
		_shape_[0] = n;
	}
	// Soft reshape
	void _reshape(const shape_t & new_shape, const T & new_val = T())
	{
		_shape_ = new_shape;
		_num_dim_ = _shape_.size();
	}
public:

    // Using all vector functions except operator=, resize, assign, insert, erase, clear,
	// push_back, and pop_back
#if (1)
	using vec::begin;
	using vec::end;
	using vec::rbegin;
	using vec::rend;

	using vec::size;
	using vec::max_size;
	using vec::capacity;
	using vec::empty;
	using vec::reserve;

	using vec::operator[];
	using vec::at;
	using vec::front;
	using vec::back;

	using vec::assign;
	using vec::insert;
	using vec::erase;
	using vec::clear;

	using vec::get_allocator;

#ifdef _BRG_USE_CPP_11_STD_
	using vec::cbegin;
	using vec::cend;
	using vec::crbegin;
	using vec::crend;

	using vec::shrink_to_fit;

	using vec::data;
#endif

#endif

	// Specialized versions of vector functions
#if (1)
	template <class InputIterator>
	void assign (InputIterator first, InputIterator last)
	{
		vec::assign(first,last);
		reshape(shape_t(_num_dim_,size()));
	}
	void assign (typename vec::size_type n, const T & val)
	{
		vec::assign(n,val);
		reshape(shape_t(_num_dim_,n));
	}
	multi_vector<T,A> & push_back (const T & val)
	{
		if(_num_dim_ > 1)
			throw std::out_of_range("Can't push_back to num_dim>1 vector.");
		_shape_[0] += 1;
		try
		{
			vec::push_back(val);
		}
		catch(const std::exception &e)
		{
			_shape_[0] -= 1;
			throw;
		}
		return *this;
	}
	typename vec::iterator insert (typename vec::iterator position, const T & val)
	{
		if(_num_dim_ != 1)
			throw std::out_of_range("Can't insert to num_dim!=1 vector.");
		_shape_[0] += 1;
		try
		{
			return vec::insert(position,val);
		}
		catch(const std::exception &e)
		{
			_shape_[0] -= 1;
			throw;
		}
	}
	void insert (typename vec::iterator position, typename vec::size_type n,
			const typename vec::value_type& val)
	{
		if(_num_dim_ != 1)
			throw std::out_of_range("Can't insert to num_dim!=1 vector.");
		try
		{
			vec::insert(position,n,val);
			_shape_[0] += n;
		}
		catch(const std::exception &e)
		{
			throw;
		}
	}
	template <class InputIterator>
	void insert (typename vec::iterator position, InputIterator first, InputIterator last)
	{
		if(_num_dim_ != 1)
			throw std::out_of_range("Can't insert to num_dim!=1 vector.");
		try
		{
			vec::insert(position,first,last);
			_shape_[0] = size();
		}
		catch(const std::exception &e)
		{
			throw;
		}
	}
	typename vec::iterator erase (typename vec::iterator position)
	{
		if(_num_dim_ != 1)
			throw std::out_of_range("Can't erase with num_dim!=1 vector.");
		_shape_[0] -= 1;
		try
		{
			return vec::erase(position);
		}
		catch(const std::exception &e)
		{
			_shape_[0] += 1;
			throw;
		}
	}
	typename vec::iterator erase (typename vec::iterator first, typename vec::iterator last)
	{
		if(_num_dim_ != 1)
			throw std::out_of_range("Can't erase with num_dim!=1 vector.");
		try
		{
			typename vec::iterator result = vec::erase(first,last);
			_shape_[0] = size();
			return result;
		}
		catch(const std::exception &e)
		{
			throw;
		}
	}
	void resize (vsize_t n, T val = T())
	{
		vec::resize(n,val);
		_resize(n);
	}
	const T & pop_back()
	{
		T result = vec::back();
		vec::pop_back();
		return result;
	}
	void del_back()
	{
		vec::pop_back();
	}
	void clear()
	{
		vec::clear();
		reshape(shape_t(0,0));
	}

#ifdef _BRG_USE_CPP_11_STD_
	template <class... Args>
	typename vec::iterator emplace (typename vec::const_iterator position, Args&&... args)
	{
		if(_num_dim_ != 1)
			throw std::out_of_range("Cannot emplace in vector of num_dim!=1.");
		_shape_[0] += 1;
		try
		{
			vec::emplace(position,args...);
		}
		catch(const std::exception &e)
		{
			_shape_[0] -= 1;
			throw;
		}
	}
	template <class... Args>
	typename vec::iterator emplace_back (Args&&... args)
	{
		if(_num_dim_ != 1)
			throw std::out_of_range("Cannot emplace_back in vector of num_dim!=1.");
		_shape_[0] += 1;
		try
		{
			vec::emplace_back(args...);
		}
		catch(const std::exception &e)
		{
			_shape_[0] -= 1;
			throw;
		}
	}
#endif

#endif // Specialized versions of vector functions

	// Overloaded assignment(-related) operators
#if (1)
	multi_vector<T, A> & operator=(multi_vector<T, A> other)
	{
		this->swap(other);
		return *this;
	}

	multi_vector<T, A> & copy(multi_vector<T, A> other)
	{
		return *this = other;
	}

	multi_vector<T, A> & swap(multi_vector<T, A> & other)
	{
		vec::swap(other);
		std::swap(_num_dim_,other._num_dim_);
		std::swap(_shape_,other._shape_);
		return *this;
	}

	// Cast to scalar
	operator T() const
	{
		return vec::at(0);
	}

	template<typename T_o, typename A_o>
	multi_vector<T, A> & operator+=( const multi_vector<T_o, A_o> & other )
	{
		if(_shape_ != other.shape())
			throw std::out_of_range("Cannot add vectors of different shapes.");
		for(vsize_t i=0; i<size(); i++)
			vec::operator[](i) += other[i];
		return *this;
	}
	template<typename T_o, typename A_o>
	multi_vector<T, A> & operator+=( const std::vector<T_o, A_o> & other )
	{
		if(size() != other.size())
			throw std::out_of_range("Cannot add vectors of different sizes.");
		for(vsize_t i=0; i<size(); i++)
			vec::operator[](i) += other[i];
		return *this;
	}
	template<typename T_o>
	multi_vector<T, A> & operator+=( T_o other )
	{
		// Special handling for scalars
		for(vsize_t i=0; i<size(); i++)
			vec::operator[](i) += other;
		return *this;
	}
	template<typename T_o, typename A_o>
	multi_vector<T, A> & operator-=( const multi_vector<T_o, A_o> & other )
	{
		if(_shape_ != other.shape())
			throw std::out_of_range("Cannot subtract vectors of different shapes.");
		for(vsize_t i=0; i<size(); i++)
			vec::operator[](i) -= other[i];
		return *this;
	}
	template<typename T_o, typename A_o>
	multi_vector<T, A> & operator-=( const std::vector<T_o, A_o> & other )
	{
		if(size() != other.size())
			throw std::out_of_range("Cannot subtrace vectors of different sizes.");
		for(vsize_t i=0; i<size(); i++)
			vec::operator[](i) -= other[i];
		return *this;
	}
	template<typename T_o>
	multi_vector<T, A> & operator-=( T_o other )
	{
		// Special handling for scalars
		for(vsize_t i=0; i<size(); i++)
			vec::operator[](i) -= other;
		return *this;
	}
	template<typename T_o, typename A_o>
	multi_vector<T, A> & operator*=( const multi_vector<T_o, A_o> & other )
	{
		if(_shape_ != other.shape())
			throw std::out_of_range("Cannot multiply vectors of different shapes.");
		for(vsize_t i=0; i<size(); i++)
			vec::operator[](i) *= other[i];
		return *this;
	}
	template<typename T_o, typename A_o>
	multi_vector<T, A> & operator*=( const std::vector<T_o, A_o> & other )
	{
		if(size() != other.size())
			throw std::out_of_range("Cannot multiply vectors of different sizes.");
		for(vsize_t i=0; i<size(); i++)
			vec::operator[](i) *= other[i];
		return *this;
	}
	template<typename T_o>
	multi_vector<T, A> & operator*=( T_o other )
	{
		// Special handling for scalars
		for(vsize_t i=0; i<size(); i++)
			vec::operator[](i) *= other;
		return *this;
	}
	template<typename T_o, typename A_o>
	multi_vector<T, A> & operator/=( const multi_vector<T_o, A_o> & other )
	{
		if(_shape_ != other.shape())
			throw std::out_of_range("Cannot divide vectors of different shapes.");
		for(vsize_t i=0; i<size(); i++)
			vec::operator[](i) /= other[i];
		return *this;
	}
	template<typename T_o, typename A_o>
	multi_vector<T, A> & operator/=( const std::vector<T_o, A_o> & other )
	{
		if(size() != other.size())
			throw std::out_of_range("Cannot divide vectors of different sizes.");
		for(vsize_t i=0; i<size(); i++)
			vec::operator[](i) /= other[i];
		return *this;
	}
	template<typename T_o>
	multi_vector<T, A> & operator/=( T_o other )
	{
		// Special handling for scalars
		for(vsize_t i=0; i<size(); i++)
			vec::operator[](i) /= other;
		return *this;
	}

	multi_vector<T, A> & operator++()
	{
		for(vsize_t i=0; i<size(); i++)
			vec::operator[](i) += 1;
		return *this;
	}
	const multi_vector<T, A> & operator++(int_type)
	{
		multi_vector<T,A> old(*this);
		for(vsize_t i=0; i<size(); i++)
			vec::operator[](i) += 1;
		return old;
	}

#endif

	// New methods and accessors
#if (1)

	// Accessors
#if (1)

	std::vector<T,A> v() const
	{
		std::vector<T,A> v;
		v.reserve(size());
		for(vsize_t i=0;i<size();++i) v.push_back(vec::operator[](i));
		return v;
	}

	const shape_t & shape() const
	{
		return _shape_;
	}

	const dsize_t & num_dim() const
	{
		return _num_dim_;
	}

	// Unsafe element-access by position vector
	T & operator() (const shape_t & position)
	{
		return vec::operator[](_get_p(position));
	}
	const T & operator() (const shape_t & position) const
	{
		return vec::operator[](_get_p(position));
	}
	T & operator() (const multi_vector<int_type> & position)
	{
		return vec::operator[](_get_p(position));
	}
	const T & operator() (const multi_vector<int_type> & position) const
	{
		return vec::operator[](_get_p(position));
	}

	// Safe element-access by position vector
	T & at(const shape_t & position)
	{
		if(position.size() != _num_dim_)
			throw std::out_of_range("Attempt to access vector with invalid position vector size.");
		if(not_all_true(position<_shape_))
			throw std::out_of_range("Attempt to access vector with position outside bounds.");

		return (*this)(position);
	}
	const T & at(const shape_t & position) const
	{
		if(position.size() != _num_dim_)
			throw std::out_of_range("Attempt to access vector with invalid position vector size.");
		if(not_all_true(position<_shape_))
			throw std::out_of_range("Attempt to access vector with position outside bounds.");

		return (*this)(position);
	}
	T & at(const multi_vector<int_type> & position)
	{
		if(position.size() != _num_dim_)
			throw std::out_of_range("Attempt to access vector with invalid position vector size.");
		if(not_all_true(position.v()<_shape_))
			throw std::out_of_range("Attempt to access vector with position outside bounds.");

		return (*this)(position);
	}
	const T & at(const multi_vector<int_type> &position) const
	{
		if(position.size() != _num_dim_)
			throw std::out_of_range("Attempt to access vector with invalid position vector size.");
		if(not_all_true(position.v()<_shape_))
			throw std::out_of_range("Attempt to access vector with position outside bounds.");

		return (*this)(position);
	}
#endif

	// Reshaping
	void reshape(const shape_t & new_shape, const T & new_val = T())
	{
		// Check if no reshaping is necessary
		if(_shape_==new_shape) return;

		if(new_shape.size() > std::numeric_limits<dsize_t>::max())
			throw std::out_of_range("Cannot reshape vector: Dimensions array too large.");

		vsize_t new_size = 1;
		for(shape_t::const_iterator itr=new_shape.begin(); itr!=new_shape.end();++itr)
			new_size *= *itr;

		if(new_size >= vec::max_size())
			throw std::out_of_range("Cannot reshape vector: Too large to be indexed by this implementation.");
		    // There's a slim chance this will throw on a valid size, but unlikely

		vec::resize(new_size,new_val);
		_shape_ = new_shape;
		_num_dim_ = _shape_.size();
	}

#endif // New methods

	// Constructors and destructors
#if (1)
	multi_vector(const A& alloc = A())
	: vec(alloc), _num_dim_(0)
	{
	}
	multi_vector( vsize_t init_size, T init_val, const A& alloc = A() )
	: vec(init_size, init_val, alloc)
	{
		_resize(init_size);
	}
	multi_vector( shape_t init_shape, T init_val, const A& alloc = A() )
	: vec(alloc)
	{
		reshape(init_shape,init_val);
	}
	multi_vector(const multi_vector<T, A> & other)
	: vec(other)
	{
		_reshape(other.shape());
	}
	template<typename T_o, typename A_o>
	multi_vector(const multi_vector<T_o, A_o> & other)
	: vec(other)
	{
		reshape(other.shape());
		for(vsize_t i=0; i<size(); i++) vec::operator[](i) = other[i];
	}
	template<typename T_o, typename A_o>
	multi_vector(const std::vector<T_o, A_o> & other)
	: vec(other)
	{
		resize(other.size());
		for(vsize_t i=0; i<size(); i++) vec::operator[](i) = other[i];
	}
	template<typename T_o, size_t N>
	multi_vector( const T_o (&array)[N], const A& alloc = A() )
	: vec(alloc)
	{
		resize(N);
		for(vsize_t i=0; i<N; i++) vec::operator[](i) = array[i];
	}
	template<typename T_o>
	multi_vector(const T_o & other, const A& alloc = A())
	: vec(alloc)
	{
		resize(1);
		vec::operator[](0) = other;
	}
	template<typename iT_o>
	multi_vector(iT_o first, iT_o last, const A& alloc = A())
	: vec(first, last, alloc)
	{
		_resize(vec::size());
	}
	virtual ~multi_vector()
	{
		// Everything will be handled automatically by _shape_ and the data's destructors
	}

	// Move Constructors
#ifdef _BRG_USE_CPP_11_
	template<typename T_o, typename A_o>
	multi_vector(std::multi_vector<T_o, A_o> && other)
	: vec()
	{
		swap(*this,other);
	}
	template<typename T_o, typename A_o>
	multi_vector(multi_vector<T_o, A_o> && other)
	: vec()
	{
		swap(*this,other);
	}
#endif // _BRG_USE_CPP_11_

#endif

}; // class vector

// Global brgastro functions
#if (1)

template<typename T, typename A>
void swap(multi_vector<T, A> & v1, multi_vector<T, A> & v2)
{
	v1.swap(v2);
}

#endif // Global brgastro functions

// Overloaded operators
#if (1)

// Arithmetic operators
#if (1)

template<typename T1, typename A1, typename T2, typename A2>
const multi_vector<T1, A1> operator+( multi_vector<T1, A1> v1, const multi_vector<T2, A2> & v2 )
{
	v1 += v2;
	return v1;
}
template<typename T1, typename A1, typename T2, typename A2>
const multi_vector<T1, A1> operator+( multi_vector<T1, A1> v1, const std::vector<T2, A2> & v2 )
{
	v1 += v2;
	return v1;
}
template<typename T1, typename A1, typename T2, typename A2>
const multi_vector<T1, A1> operator+( const std::vector<T1, A1> & v1, multi_vector<T2, A2> v2 )
{
	v2 += v1;
	return v2;
}
template<typename T1, typename A1, typename T2>
const multi_vector<T1, A1> operator+( multi_vector<T1, A1> v1, T2 v2 )
{
	v1 += v2;
	return v1;
}
template<typename T1, typename T2, typename A2>
const multi_vector<T2, A2> operator+( T1 v1, multi_vector<T2, A2> v2 )
{
	v2 += v1;
	return v2;
}
template<typename T1, typename A1, typename T2, typename A2>
const multi_vector<T1, A1> operator-( multi_vector<T1, A1> v1, const multi_vector<T2, A2> & v2 )
{
	v1 -= v2;
	return v1;
}
template<typename T1, typename A1, typename T2, typename A2>
const multi_vector<T1, A1> operator-( multi_vector<T1, A1> v1, const std::vector<T2, A2> & v2 )
{
	v1 -= v2;
	return v1;
}
template<typename T1, typename A1, typename T2, typename A2>
const multi_vector<T1, A1> operator-( const std::vector<T1, A1> &v1, multi_vector<T2, A2> v2 )
{
	if(v1.size() != v2.size())
		throw std::out_of_range("Cannot subtract vectors of different sizes.");
	for(typename multi_vector<T2,A2>::size_t i=0; i<v2.size(); ++i)
		v2[i] = v1[i]-v2[i];
	return v2;
}
template<typename T1, typename A1, typename T2>
const multi_vector<T1, A1> operator-( multi_vector<T1, A1> v1, T2 v2 )
{
	v1 -= v2;
	return v1;
}
template<typename T1, typename T2, typename A2>
const multi_vector<T2, A2> operator-( T1 v1, multi_vector<T2, A2> v2 )
{
	for(typename multi_vector<T2, A2>::vsize_t i=0; i<v2.size(); i++)
		v2[i] = v1-v2[i];
	return v2;
}
template<typename T1, typename A1, typename T2, typename A2>
const multi_vector<T1, A1> operator*( multi_vector<T1, A1> v1, const multi_vector<T2, A2> & v2 )
{
	v1 *= v2;
	return v1;
}
template<typename T1, typename A1, typename T2, typename A2>
const multi_vector<T1, A1> operator*( multi_vector<T1, A1> v1, const std::vector<T2, A2> & v2 )
{
	v1 *= v2;
	return v1;
}
template<typename T1, typename A1, typename T2, typename A2>
const multi_vector<T1, A1> operator*( const std::vector<T1, A1> & v1, multi_vector<T2, A2> v2 )
{
	v2 *= v1;
	return v2;
}
template<typename T1, typename A1, typename T2>
const multi_vector<T1, A1> operator*( multi_vector<T1, A1> v1, T2 v2 )
{
	v1 *= v2;
	return v1;
}
template<typename T1, typename T2, typename A2>
const multi_vector<T2, A2> operator*( T1 v1, multi_vector<T2, A2> v2 )
{
	v2 *= v1;
	return v2;
}
template<typename T1, typename A1, typename T2, typename A2>
const multi_vector<T1, A1> operator/( multi_vector<T1, A1> v1, const multi_vector<T2, A2> & v2 )
{
	v1 /= v2;
	return v1;
}
template<typename T1, typename A1, typename T2, typename A2>
const multi_vector<T1, A1> operator/( multi_vector<T1, A1> v1, const std::vector<T2, A2> & v2 )
{
	v1 /= v2;
	return v1;
}
template<typename T1, typename A1, typename T2, typename A2>
const multi_vector<T1, A1> operator/( const std::vector<T1, A1> &v1, multi_vector<T2, A2> v2 )
{
	if(v1.size() != v2.size())
		throw std::out_of_range("Cannot divide vectors of different sizes.");
	for(typename multi_vector<T2,A2>::size_t i=0; i<v2.size(); ++i)
		v2[i] = v1[i]/v2[i];
	return v2;
}
template<typename T1, typename A1, typename T2>
const multi_vector<T1, A1> operator/( multi_vector<T1, A1> v1, T2 v2 )
{
	v1 /= v2;
	return v1;
}
template<typename T1, typename T2, typename A2>
const multi_vector<T2, A2> operator/( T1 v1, multi_vector<T2, A2> v2 )
{
	for(typename multi_vector<T2, A2>::vsize_t i=0; i<v2.size(); i++)
		v2[i] = v1/v2[i];
	return v2;
}

#endif // Arithmetic operators

// Comparison operators
#if (1)

// Element-wise equal
template<typename T1, typename A1, typename T2, typename A2>
const std::vector<bool> & operator==( const multi_vector<T1, A1> & v1, const multi_vector<T2, A2> & v2 )
{
	if(v1.shape()!= v2.shape())
		throw std::out_of_range("Cannot compare vectors of different shapes element-wise.");
	std::vector<bool> result(v1.size());
	for(typename IceBRG::multi_vector<T1,A1>::vsize_t i=0; i<v1.size(); i++)
		result[i] = (v1[i] == v2[i]);
	return result;
}
template<typename T1, typename A1, typename T2>
const std::vector<bool> & operator==( const multi_vector<T1, A1> & v1, const T2 v2 )
{
	std::vector<bool> result(v1.size());
	for(typename IceBRG::multi_vector<T1,A1>::vsize_t i=0; i<v1.size(); i++)
		result[i] = (v1[i] == v2);
	return result;
}
template<typename T1, typename T2, typename A2>
const std::vector<bool> & operator==( const T1 v1, const multi_vector<T2, A2> & v2 )
{
	std::vector<bool> result(v2.size());
	// Special handling for scalars
	for(typename IceBRG::multi_vector<T2,A2>::vsize_t i=0; i<v2.size(); i++)
		result[i] = (v2[i] == v1);
	return result;
}

// Element-wise not equal
template<typename T1, typename A1, typename T2, typename A2>
const std::vector<bool> & operator!=( const multi_vector<T1, A1> & v1, const multi_vector<T2, A2> & v2 )
{
	return !operator==(v1,v2);
}
template<typename T1, typename A1, typename T2>
const std::vector<bool> & operator!=( const multi_vector<T1, A1> & v1, const T2 v2 )
{
	return !operator==(v1,v2);
}
template<typename T1, typename T2, typename A2>
const std::vector<bool> & operator!=( const T1 v1, const multi_vector<T2, A2> & v2 )
{
	return !operator==(v1,v2);
}

// Element-wise less-than
template<typename T1, typename A1, typename T2, typename A2>
const std::vector<bool> & operator<( const multi_vector<T1, A1> & v1, const multi_vector<T2, A2> & v2 )
{
	if(v1.shape()!= v2.shape())
		throw std::out_of_range("Cannot compare vectors of different shapes element-wise.");
	std::vector<bool> result(v1.size());
	for(typename IceBRG::multi_vector<T1,A1>::vsize_t i=0; i<v1.size(); i++)
		result[i] = (v1[i] < v2[i]);
	return result;
}
template<typename T1, typename A1, typename T2>
const std::vector<bool> & operator<( const multi_vector<T1, A1> & v1, const T2 v2 )
{
	std::vector<bool> result(v1.size());
	for(typename IceBRG::multi_vector<T1,A1>::vsize_t i=0; i<v1.size(); i++)
		result[i] = (v1[i] < v2);
	return result;
}
template<typename T1, typename T2, typename A2>
const std::vector<bool> & operator<( const T1 v1, const multi_vector<T2, A2> & v2 )
{
	std::vector<bool> result(v2.size());
	// Special handling for scalars
	for(typename IceBRG::multi_vector<T2,A2>::vsize_t i=0; i<v2.size(); i++)
		result[i] = (v2 < v1[i]);
	return result;
}

// Element-wise greater-than
template<typename T1, typename A1, typename T2, typename A2>
const std::vector<bool> & operator>( const multi_vector<T1, A1> & v1, const multi_vector<T2, A2> & v2 )
{
	return operator<(v2,v1);
}
template<typename T1, typename A1, typename T2>
const std::vector<bool> & operator>( const multi_vector<T1, A1> & v1, const T2 v2 )
{
	return operator<(v2,v1);
}
template<typename T1, typename T2, typename A2>
const std::vector<bool> & operator>( const T1 v1, const multi_vector<T2, A2> & v2 )
{
	return operator<(v2,v1);
}

// Element-wise less-than or equal to
template<typename T1, typename A1, typename T2, typename A2>
const std::vector<bool> & operator<=( const multi_vector<T1, A1> & v1, const multi_vector<T2, A2> & v2 )
{
	return v_not(operator>(v1,v2));
}
template<typename T1, typename A1, typename T2>
const std::vector<bool> & operator<=( const multi_vector<T1, A1> & v1, T2 v2 )
{
	return v_not(operator>(v1,v2));
}
template<typename T1, typename T2, typename A2>
const std::vector<bool> & operator<=( const T1 v1, const multi_vector<T2, A2> & v2 )
{
	return v_not(operator>(v1,v2));
}

// Element-wise greater-than or equal to
template<typename T1, typename A1, typename T2, typename A2>
const std::vector<bool> & operator>=( const multi_vector<T1, A1> & v1, const multi_vector<T2, A2> & v2 )
{
	return v_not(operator<(v1,v2));
}
template<typename T1, typename A1, typename T2>
const std::vector<bool> & operator>=( const multi_vector<T1, A1> & v1, const T2 v2 )
{
	return v_not(operator<(v1,v2));
}
template<typename T1, typename T2, typename A2>
const std::vector<bool> & operator>=( const T1 v1, const multi_vector<T2, A2> & v2 )
{
	return v_not(operator<(v1,v2));
}

#endif // Comparison operators

#endif // Overloaded operators

// Element-wise min/max/bound
#if (1)

template<typename T, typename A, typename T_o, typename A_o>
multi_vector<T,A> min( multi_vector<T,A> v1, const multi_vector<T_o,A_o> & v2 )
{
	if(v1.shape()!= v2.shape())
		throw std::out_of_range("Cannot compare vectors of different shapes element-wise.");
	for(typename IceBRG::multi_vector<T,A>::vsize_t i=0; i<v1.size(); i++)
		v1[i] = min(v1[i],v2[i]);
	return v1;
}
template<typename T, typename A, typename T_o, typename A_o>
multi_vector<T,A> min( multi_vector<T,A> v1, const std::vector<T_o,A_o> & v2 )
{
	if(v1.size()!= v2.size())
		throw std::out_of_range("Cannot compare vectors of different sizes element-wise.");
	for(typename IceBRG::multi_vector<T,A>::vsize_t i=0; i<v1.size(); i++)
		v1[i] = min(v1[i],v2[i]);
	return v1;
}
template<typename T, typename A, typename T_o, typename A_o>
multi_vector<T,A> min( const std::vector<T,A> & v1, const multi_vector<T_o,A_o> & v2 )
{
	if(v1.shape()!= v2.shape())
		throw std::out_of_range("Cannot compare vectors of different sizes element-wise.");
	multi_vector<T,A> result(v1);
	for(typename IceBRG::multi_vector<T,A>::vsize_t i=0; i<v1.size(); i++)
		result[i] = min(result[i],v2[i]);
	return result;
}
template<typename T, typename T_o, typename A_o>
multi_vector<T_o,A_o> min( T v1, multi_vector<T_o,A_o> v2 )
{
	// Special handling for scalars
	for(typename IceBRG::multi_vector<T_o,A_o>::vsize_t i=0; i<v2.size(); i++)
		v2[i] = min(v1,v2[i]);
	return v2;
}
template<typename T, typename A, typename T_o>
multi_vector<T,A> min( multi_vector<T,A> v1, const T_o v2 )
{
	// Special handling for scalars
	for(typename IceBRG::multi_vector<T,A>::vsize_t i=0; i<v1.size(); i++)
		v1[i] = min(v1[i],v2);
	return v1;
}

template<typename T, typename A, typename T_o, typename A_o>
multi_vector<T,A> max( multi_vector<T,A> v1, const multi_vector<T_o,A_o> & v2 )
{
	if(v1.shape()!= v2.shape())
		throw std::out_of_range("Cannot compare vectors of different shapes element-wise.");
	for(typename IceBRG::multi_vector<T,A>::vsize_t i=0; i<v1.size(); i++)
		v1[i] = max(v1[i],v2[i]);
	return v1;
}
template<typename T, typename A, typename T_o, typename A_o>
multi_vector<T,A> max( multi_vector<T,A> v1, const std::vector<T_o,A_o> & v2 )
{
	if(v1.size()!= v2.size())
		throw std::out_of_range("Cannot compare vectors of different sizes element-wise.");
	for(typename IceBRG::multi_vector<T,A>::vsize_t i=0; i<v1.size(); i++)
		v1[i] = max(v1[i],v2[i]);
	return v1;
}
template<typename T, typename A, typename T_o, typename A_o>
multi_vector<T,A> max( const std::vector<T,A> & v1, const multi_vector<T_o,A_o> & v2 )
{
	if(v1.shape()!= v2.shape())
		throw std::out_of_range("Cannot compare vectors of different sizes element-wise.");
	multi_vector<T,A> result(v1);
	for(typename IceBRG::multi_vector<T,A>::vsize_t i=0; i<v1.size(); i++)
		result[i] = max(result[i],v2[i]);
	return result;
}
template<typename T, typename T_o, typename A_o>
multi_vector<T_o,A_o> max( const T v1, multi_vector<T_o,A_o> v2 )
{
		// Special handling for scalars
		for(typename IceBRG::multi_vector<T_o,A_o>::vsize_t i=0; i<v2.size(); i++)
			v2[i] = max(v1,v2[i]);
		return v2;
}
template<typename T, typename A, typename T_o>
multi_vector<T,A> max( multi_vector<T,A> v1, const T_o v2 )
{
	// Special handling for scalars
	for(typename IceBRG::multi_vector<T,A>::vsize_t i=0; i<v1.size(); i++)
		v1[i] = max(v1[i],v2);
	return v1;
}

#endif // Element-wise min/max/bound

// Math and safe functions
#if (1)

// Element-wise power
#if (1)

template< typename T1, typename A1, typename T2, typename A2 >
const multi_vector<T1,A1> pow( multi_vector<T1,A1> v1, const multi_vector<T2,A2> &v2 )
{
	if(v1.shape()!= v2.shape())
		throw std::out_of_range("Cannot raise vector to power vector of different shape element-wise.");
	for(int_type i = 0; i < v1.size(); i++)
	{
		v1[i] = std::pow(v1[i], v2[i]);
	}

	return v1;
}

template< typename T1, typename A1, typename T2 >
const multi_vector<T1,A1> pow( const multi_vector<T1,A1> & v1, const T2 &v2 )
{
	for(int_type i = 0; i < v1.size(); i++)
	{
		v1[i] = std::pow(v1[i], v2);
	}

	return v1;
}

template< typename T1, typename T2, typename A2 >
const multi_vector<T2,A2> pow( const T1 & v1, multi_vector<T2,A2> v2 )
{
	for(int_type i = 0; i < v2.size(); i++)
	{
		v2[i] = std::pow(v1, v2[i]);
	}

	return v2;
}

#endif // Element-wise power

// Element-wise safe power
#if (1)

template< typename T1, typename A1, typename T2, typename A2 >
const multi_vector<T1,A1> safe_pow( multi_vector<T1,A1> v1, const multi_vector<T2,A2> &v2 )
{
	if(v1.shape()!= v2.shape())
		throw std::out_of_range("Cannot raise vector to power vector of different shape element-wise.");
	for(int_type i = 0; i < v1.size(); i++)
	{
		v1[i] = safe_pow(v1[i], v2[i]);
	}

	return v1;
}

template< typename T1, typename A1, typename T2 >
const multi_vector<T1,A1> safe_pow( const multi_vector<T1,A1> & v1, const T2 &v2 )
{
	for(int_type i = 0; i < v1.size(); i++)
	{
		v1[i] = safe_pow(v1[i], v2);
	}

	return v1;
}

template< typename T1, typename T2, typename A2 >
const multi_vector<T2,A2> safe_pow( const T1 & v1, multi_vector<T2,A2> v2 )
{
	for(int_type i = 0; i < v2.size(); i++)
	{
		v2[i] = safe_pow(v1, v2[i]);
	}

	return v2;
}

#endif // Element-wise safe power

// Element-wise negate
#if (1)

template< typename T, typename A >
const multi_vector<T,A> operator-( multi_vector<T,A> v )
{
	for(int_type i = 0; i < v.size(); i++)
	{
		v[i] = -v[i];
	}
	return v;
}

#endif // Element-wise negate

// Element-wise abs
#if (1)

template< typename T, typename A >
const multi_vector<T,A> abs( multi_vector<T,A> v )
{
	using std::abs;
	using IceBRG::abs;

	for(int_type i = 0; i < v.size(); i++)
	{
		v[i] = abs(v[i]);
	}

	return v;
}

#endif // Element-wise abs

// Element-wise square root
#if (1)

template< typename T, typename A >
const multi_vector<T,A> sqrt( multi_vector<T,A> v )
{
	using std::sqrt;
	using IceBRG::sqrt;

	for(int_type i = 0; i < v.size(); i++)
	{
		v[i] = sqrt(v[i]);
	}

	return v;
}

#endif // Element-wise square root

// Element-wise safe square root
#if (1)

template< typename T, typename A >
const multi_vector<T,A> safe_sqrt( multi_vector<T,A> v )
{
	for(int_type i = 0; i < v.size(); i++)
	{
		v[i] = safe_sqrt(v[i]);
	}

	return v;
}

#endif // Element-wise safe square root

// Element-wise exponential
#if (1)

template< typename T, typename A >
const multi_vector<T,A> exp( multi_vector<T,A> v )
{
	using std::exp;
	using IceBRG::exp;

	for(int_type i = 0; i < v.size(); i++)
		v[i] = exp(v[i]);

	return v;
}

#endif // Element-wise exponential

// Element-wise square
#if (1)

template< typename T, typename A >
const multi_vector<T,A> square( multi_vector<T,A> v )
{
	for(int_type i = 0; i < v.size(); i++)
	{
		v[i] = square(v[i]);
	}

	return v;
}

#endif // Element-wise square

// Element-wise cube
#if (1)

template< typename T, typename A >
const multi_vector<T,A> cube( multi_vector<T,A> v )
{
	for(int_type i = 0; i < v.size(); i++)
	{
		v[i] = cube(v[i]);
	}

	return v;
}

#endif // Element-wise cube

// Element-wise quart
#if (1)

template< typename T, typename A >
const multi_vector<T,A> quart( multi_vector<T,A> v )
{
	for(int_type i = 0; i < v.size(); i++)
	{
		v[i] = quart(v[i]);
	}

	return v;
}

#endif // Element-wise quart

// Element-wise inverse
#if (1)

template< typename T, typename A >
const multi_vector<T,A> inverse( multi_vector<T,A> v )
{
	for(int_type i = 0; i < v.size(); i++)
	{
		v[i] = inverse(v[i]);
	}

	return v;
}

#endif // Element-wise inverse

// Element-wise inv_square
#if (1)

template< typename T, typename A >
const multi_vector<T,A> inv_square( multi_vector<T,A> v )
{
	for(int_type i = 0; i < v.size(); i++)
	{
		v[i] = inv_square(v[i]);
	}

	return v;
}

#endif // Element-wise inv_square

// Element-wise inv_cube
#if (1)

template< typename T, typename A >
const multi_vector<T,A> inv_cube( multi_vector<T,A> v )
{
	for(int_type i = 0; i < v.size(); i++)
	{
		v[i] = inv_cube(v[i]);
	}

	return v;
}

#endif // Element-wise inv_cube

// Element-wise inv_quart
#if (1)

template< typename T, typename A >
const multi_vector<T,A> inv_quart( multi_vector<T,A> v )
{
	for(int_type i = 0; i < v.size(); i++)
	{
		v[i] = inv_quart(v[i]);
	}

	return v;
}

#endif // Element-wise inv_quart

// Element-wise safe_d
#if (1)

template< typename T, typename A >
const multi_vector<T,A> safe_d( multi_vector<T,A> v )
{
	for(int_type i = 0; i < v.size(); i++)
		v[i] = safe_d(v[i]);

	return v;
}

#endif // Element-wise safe_d

#endif // Math and safe functions

// Summary functions
#if (1)

// Sum
#if (1)

template< typename T, typename A >
const T sum(const IceBRG::multi_vector<T,A> &v)
{
	T result = 0;
	for(int_type i = 0; i < v.size(); i++)
	{
		result += v[i];
	}
	return result;
}

#endif // Sum

// Product
#if (1)

template< typename T, typename A >
const T product(const IceBRG::multi_vector<T,A> &v)
{
	T result = 1;
	for(int_type i = 0; i < v.size(); i++)
	{
		result *= v[i];
	}
	return result;
}

#endif // Product

// Mean
#if (1)

template< typename T, typename A >
const T mean(const IceBRG::multi_vector<T,A> &v)
{
	if(v.size()==0) return 0;
	return sum(v)/v.size();
}

#endif // Mean

// Standard Deviation
#if (1)

template< typename T, typename A >
const T std(const IceBRG::multi_vector<T,A> &v)
{
	if(v.size()<=1) return 0;

	return sqrt( ((sum( square(v) ) - square(sum(v)) ) / v.size() ) );
}
template< typename T, typename A >
const T stddev(const IceBRG::multi_vector<T,A> &v)
{
	return std(v);
}

#endif // Standard Deviation

#endif // Summary Functions

} // namespace IceBRG

#endif // __BRG_VECTOR_HPP_INCLUDED__
