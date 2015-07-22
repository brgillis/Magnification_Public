/**********************************************************************\
 @file labeled_array_vecs.hpp
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

#ifndef _BRG_CONTAINER_LABELED_ARRAY_VECS_HPP_INCLUDED_
#define _BRG_CONTAINER_LABELED_ARRAY_VECS_HPP_INCLUDED_

#include <utility>

#include <boost/iterator.hpp>

#include "IceBRG_main/container/labeled_array.hpp"
#include "IceBRG_main/utility.hpp"

namespace IceBRG {

enum { forward_tag = false, reverse_tag = true };

template<typename labeled_array_type,
typename T_vec_type, typename T_const_vec_type,
typename T_reference, typename T_const_reference,
typename T_iterator, typename T_const_iterator,
int_type T_direction_tag=forward_tag>
class labeled_array_vecs
{
private:

	// Private typedefs
	typedef typename labeled_array_type::map_type map_type;

	// Members
	labeled_array_type * const _array_;
	typename labeled_array_type::size_type _size_;
	typename labeled_array_type::size_type _num_ovecs_;

public:

	// Public typedefs
	typedef typename labeled_array_type::label_type label_type;
	typedef typename labeled_array_type::value_type value_type;
	typedef typename labeled_array_type::size_type size_type;

	static_assert(std::is_signed<size_type>::value,"size_type for labeled_array must be signed.");

	typedef T_vec_type vec_type;
	typedef T_const_vec_type const_vec_type;

	typedef T_reference reference;
	typedef T_const_reference const_reference;

	typedef T_iterator iterator;
	typedef T_const_iterator const_iterator;
	typedef boost::reverse_iterator<iterator> reverse_iterator;
	typedef boost::reverse_iterator<const_iterator> const_reverse_iterator;

	typedef typename labeled_array_type::difference_type difference_type;

private:

	// Private iterator methods
#if(1)

	/// begin
	iterator _begin() noexcept
	{
		return iterator(_array_,0);
	}
	/// end
	iterator _end() noexcept
	{
		return iterator(_array_,_size_);
	}
	/// rbegin
	reverse_iterator _rbegin() noexcept
	{
		return reverse_iterator(_end());
	}
	/// rend
	reverse_iterator _rend() noexcept
	{
		return reverse_iterator(_begin());
	}
	/// cbegin
	const_iterator _cbegin() const noexcept
	{
		return const_iterator(_array_,0);
	}
	/// cend
	const_iterator _cend() const noexcept
	{
		return const_iterator(_array_,_size_);
	}
	/// crbegin
	const_reverse_iterator _crbegin() const noexcept
	{
		return const_reverse_iterator(_cend());
	}
	/// crend
	const_reverse_iterator _crend() const noexcept
	{
		return const_reverse_iterator(_cbegin());
	}
#endif

public:

	/// Constructor. Requires a pointer to a labeled array
	labeled_array_vecs(labeled_array_type * init_array, const size_type & init_size,
			const size_type & num_ovecs=1)
	: _array_(init_array), _size_(init_size), _num_ovecs_(num_ovecs)
	{
	}

	/// Virtual destructor
	virtual ~labeled_array_vecs() {}

	// Iterator methods (forward tag version)
#if(1)
	/// begin (const)
	template<int_type direction_tag=T_direction_tag>
	typename std::enable_if<direction_tag==forward_tag, const_iterator>::type begin() const noexcept
	{
		return _cbegin();
	}
	/// begin
	template<int_type direction_tag=T_direction_tag>
	typename std::enable_if<direction_tag==forward_tag, iterator>::type begin() noexcept
	{
		return _begin();
	}
	/// end (const)
	template<int_type direction_tag=T_direction_tag>
	typename std::enable_if<direction_tag==forward_tag, const_iterator>::type end() const noexcept
	{
		return _cend();
	}
	/// end
	template<int_type direction_tag=T_direction_tag>
	typename std::enable_if<direction_tag==forward_tag, iterator>::type end() noexcept
	{
		return _end();
	}
	/// rbegin (const)
	template<int_type direction_tag=T_direction_tag>
	typename std::enable_if<direction_tag==forward_tag, const_reverse_iterator>::type rbegin() const noexcept
	{
		return _crbegin();
	}
	/// rbegin
	template<int_type direction_tag=T_direction_tag>
	typename std::enable_if<direction_tag==forward_tag, reverse_iterator>::type rbegin() noexcept
	{
		return _rbegin();
	}
	/// rend (const)
	template<int_type direction_tag=T_direction_tag>
	typename std::enable_if<direction_tag==forward_tag, const_reverse_iterator>::type rend() const noexcept
	{
		return _crend();
	}
	/// rend
	template<int_type direction_tag=T_direction_tag>
	typename std::enable_if<direction_tag==forward_tag, reverse_iterator>::type rend() noexcept
	{
		return _rend();
	}
	/// cbegin
	template<int_type direction_tag=T_direction_tag>
	typename std::enable_if<direction_tag==forward_tag, const_iterator>::type cbegin() const noexcept
	{
		return _cbegin();
	}
	/// cend
	template<int_type direction_tag=T_direction_tag>
	typename std::enable_if<direction_tag==forward_tag, const_iterator>::type cend() const noexcept
	{
		return _cend();
	}
	/// crbegin
	template<int_type direction_tag=T_direction_tag>
	typename std::enable_if<direction_tag==forward_tag, const_reverse_iterator>::type crbegin() const noexcept
	{
		return _crbegin();
	}
	/// crend
	template<int_type direction_tag=T_direction_tag>
	typename std::enable_if<direction_tag==forward_tag, const_reverse_iterator>::type crend() const noexcept
	{
		return _crend();
	}

#endif // Iterator methods (forward tag version)

	// Iterator methods (reverse tag version)
#if(1)
	/// begin (const)
	template<int_type direction_tag=T_direction_tag>
	typename std::enable_if<direction_tag==reverse_tag, const_reverse_iterator>::type begin() const noexcept
	{
		return _crbegin();
	}
	/// begin
	template<int_type direction_tag=T_direction_tag>
	typename std::enable_if<direction_tag==reverse_tag, reverse_iterator>::type begin() noexcept
	{
		return _rbegin();
	}
	/// end (const)
	template<int_type direction_tag=T_direction_tag>
	typename std::enable_if<direction_tag==reverse_tag, const_reverse_iterator>::type end() const noexcept
	{
		return _crend();
	}
	/// end
	template<int_type direction_tag=T_direction_tag>
	typename std::enable_if<direction_tag==reverse_tag, reverse_iterator>::type end() noexcept
	{
		return _rend();
	}
	/// rbegin (const)
	template<int_type direction_tag=T_direction_tag>
	typename std::enable_if<direction_tag==reverse_tag, const_iterator>::type rbegin() const noexcept
	{
		return _cbegin();
	}
	/// rbegin
	template<int_type direction_tag=T_direction_tag>
	typename std::enable_if<direction_tag==reverse_tag, iterator>::type rbegin() noexcept
	{
		return _begin();
	}
	/// rend (const)
	template<int_type direction_tag=T_direction_tag>
	typename std::enable_if<direction_tag==reverse_tag, const_iterator>::type rend() const noexcept
	{
		return _cend();
	}
	/// rend
	template<int_type direction_tag=T_direction_tag>
	typename std::enable_if<direction_tag==reverse_tag, iterator>::type rend() noexcept
	{
		return _end();
	}
	/// cbegin
	template<int_type direction_tag=T_direction_tag>
	typename std::enable_if<direction_tag==reverse_tag, const_reverse_iterator>::type cbegin() const noexcept
	{
		return _crbegin();
	}
	/// cend
	template<int_type direction_tag=T_direction_tag>
	typename std::enable_if<direction_tag==reverse_tag, const_reverse_iterator>::type cend() const noexcept
	{
		return _crend();
	}
	/// crbegin
	template<int_type direction_tag=T_direction_tag>
	typename std::enable_if<direction_tag==reverse_tag, const_iterator>::type crbegin() const noexcept
	{
		return _cbegin();
	}
	/// crend
	template<int_type direction_tag=T_direction_tag>
	typename std::enable_if<direction_tag==reverse_tag, const_iterator>::type crend() const noexcept
	{
		return _cend();
	}

#endif // Iterator methods (reverse tag version)

	// Size methods
#if(1)

	/// size
	const size_type & size() const noexcept
	{
		return _size_;
	}

	/// empty
	bool empty() const
	{
		return _size_==0;
	}

	/// size
	const size_type & num_ovecs() const noexcept
	{
		return _num_ovecs_;
	}

#endif // Size methods

	// Row access
#if(1)

	/// Element access
	const_reference operator[] (const size_type & n) const
	{
		return const_reference(_array_,n,_num_ovecs_);
	}

	/// Element access
	reference operator[] (const size_type & n)
	{
		return reference(_array_,n,_num_ovecs_);
	}

	/// Range-checked element access
	const_reference at( const size_type & n ) const
	{
		if((n<0)||(n>_size_)) throw std::out_of_range();
		return const_reference(_array_,n,_num_ovecs_);
	}

	/// Range-checked element access
	reference at( const size_type & n )
	{
		if((n<0)||n>_size_) throw std::out_of_range();
		return reference(_array_,n,_num_ovecs_);
	}

	/// Access first element
	const_reference front() const
	{
		return const_reference(_array_,0,_num_ovecs_);
	}

	/// Access first element
	reference front()
	{
		return reference(_array_,0,_num_ovecs_);
	}

	/// Access last element
	const_reference back() const
	{
		return const_reference(_array_,_size_-1,_num_ovecs_);
	}

	/// Access last element
	reference back()
	{
		return reference(_array_,_size_-1,_num_ovecs_);
	}

	/// Access base array
	const labeled_array_type & base() const
	{
		return *_array_;
	}

	/// Access base array
	labeled_array_type & base()
	{
		return *_array_;
	}

	/// Access data
	const value_type* data() const
	{
		return _array_->data();
	}

	/// Access data
	value_type* data()
	{
		return _array_->data();
	}

#endif // Row access

	// Casting
#if (1)

	/// Cast non-const version to const version
	template <typename other_vec_type, typename other_reference, typename other_iterator, int_type other_direction_tag,
	typename std::enable_if<std::is_convertible<other_vec_type,vec_type>::value, other_vec_type *>::type = nullptr,
	typename std::enable_if<std::is_convertible<other_reference,reference>::value, other_reference *>::type = nullptr,
	typename std::enable_if<std::is_convertible<other_iterator,iterator>::value, other_iterator *>::type = nullptr>
	labeled_array_vecs( const labeled_array_vecs<labeled_array_type,other_vec_type,const_vec_type,other_reference,const_reference,
						other_iterator,const_iterator,other_direction_tag> & other)
	: _array_(&other.base()), _size_(other.size()), _num_ovecs_(other.num_ovecs()) {}

#endif

};

}



#endif // _BRG_CONTAINER_LABELED_ARRAY_VECS_HPP_INCLUDED_
