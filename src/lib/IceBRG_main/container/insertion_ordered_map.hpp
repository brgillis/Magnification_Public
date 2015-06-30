/**********************************************************************\
 @file insertion_ordered_map.hpp
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


#ifndef _BRG_INSERTION_ORDERED_MAP_HPP_INCLUDED_
#define _BRG_INSERTION_ORDERED_MAP_HPP_INCLUDED_

#include <stdexcept>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <type_traits>
#include <vector>

#include "IceBRG_main/utility.hpp"

namespace IceBRG {

template<typename T_key_type, typename T_mapped_type>
class insertion_ordered_map
{
public:
	// Public typedefs
	typedef T_key_type key_type;
	typedef T_mapped_type mapped_type;
	typedef std::pair<key_type,mapped_type> value_type;
	typedef std::vector<value_type> base_type;
	typedef std::unordered_map<key_type,size_t> map_type;

	typedef typename base_type::allocator_type allocator_type;
	typedef typename base_type::reference reference;
	typedef typename base_type::const_reference const_reference;
	typedef typename base_type::pointer pointer;
	typedef typename base_type::const_pointer const_pointer;
	typedef typename base_type::iterator iterator;
	typedef typename base_type::const_iterator const_iterator;
	typedef typename base_type::reverse_iterator reverse_iterator;
	typedef typename base_type::const_reverse_iterator const_reverse_iterator;
	typedef typename base_type::difference_type difference_type;
	typedef typename base_type::size_type size_type;
private:

	base_type _val_vector_;
	map_type _key_map_;

	template< typename new_val_type >
	value_type & _insert(new_val_type && val)
	{
		// This function is only called if we already know we need it, so we
		// don't check if the key already exists here
		_key_map_[val.first] = ssize(_val_vector_); // Once we add the value to the vector, this will
													// point to the new element
		_val_vector_.push_back(std::forward<new_val_type>(val));

		return _val_vector_.back();
	}

	void _build_key_map()
	{
		for( size_t i=0; i<ssize(_val_vector_); ++i)
		{
			_key_map_[_val_vector_[i].first] = i;
		}
	}
	void _rebuild_key_map()
	{
		_key_map_.clear();
		_build_key_map();
	}

	// _new_key(...) - Generates a new key of the templated type, which is not already in the map
	template< typename new_key_type, typename other_map_type=insertion_ordered_map<key_type,mapped_type>,
	typename std::enable_if<std::is_arithmetic<new_key_type>::value,new_key_type>::type* = nullptr,
	typename std::enable_if<std::is_convertible<key_type,typename other_map_type::key_type>::value,new_key_type>::type* = nullptr>
	new_key_type _new_key(const other_map_type & other_map = other_map_type())
	{
		// For arithmetic types, we'll start by setting the new key value equal to the number of
		// elements in the map. If that's in, we'll add 1. Repeat till we find an unused value.
		// If we try more values than are in the key map, something's gone wrong, so throw an
		// exception.
		new_key_type test_key = ssize(_key_map_);
		size_type counter=0;
		while((_key_map_.find(test_key) != _key_map_.end()) &&
			  (other_map.find(test_key) != other_map.end()) &&
			  (counter<=ssize(_key_map_)))
		{
			test_key += 1;
			++counter;
		}
		if(counter>ssize(_key_map_)) throw std::runtime_error("Cannot generate new arithmetic key for insertion_ordered_map.");
		return test_key;
	}

	template< typename new_key_type, typename other_map_type=insertion_ordered_map<key_type,mapped_type>,
	typename std::enable_if<!std::is_arithmetic<new_key_type>::value,new_key_type>::type* = nullptr,
	typename std::enable_if<std::is_convertible<key_type,typename other_map_type::key_type>::value,new_key_type>::type* = nullptr>
	new_key_type _new_key(const other_map_type & other_map = other_map_type())
	{
		// For strings, we'll start by setting the new key value to "col_N", where N is the number of
		// elements in the map. If that's in, we'll add 1 to N. Repeat till we find an unused value.
		// If we try more values than are in the key map, something's gone wrong, so throw an
		// exception.
		size_type N = ssize(_key_map_);

		std::stringstream ss;

		ss.str();
		ss << "col_" << N;
		std::string test_key = ss.str();

		size_type counter=0;
		while((_key_map_.count(test_key) + other_map.count(test_key) > 0) &&
			  (counter<=ssize(_key_map_)+other_map.size()))
		{
			ss.str();
			ss << "col_" << ++N;
			test_key = ss.str();

			++counter;
		}
		if(counter>ssize(_key_map_)+other_map.size()) throw std::runtime_error("Cannot generate new string key for insertion_ordered_map.");
		return test_key;
	}

public:

	// Constructor overloads
#if(1)
	insertion_ordered_map()
	{
	}

	explicit insertion_ordered_map(const typename base_type::allocator_type & alloc)
	: _val_vector_(alloc)
	{
	}

	template <class InputIterator>
	insertion_ordered_map (InputIterator first, InputIterator last,
		const typename base_type::allocator_type & alloc = typename base_type::allocator_type())
	: _val_vector_(first,last,alloc)
	{
		_build_key_map();
    }
#endif

	// Destructor
	virtual ~insertion_ordered_map() { }

	// Member swap, plus overloads of copy, assignment, and move
#if(1)
	void swap(insertion_ordered_map& other)
	{
		using std::swap;
		swap(_val_vector_,other._val_vector_);
		swap(_key_map_,other._key_map_);
	}
	void swap(insertion_ordered_map&& other)
	{
		using std::swap;
		swap(_val_vector_,other._val_vector_);
		swap(_key_map_,other._key_map_);
	}

	insertion_ordered_map(const insertion_ordered_map & other)
	: _val_vector_(other._val_vector_),
	  _key_map_(other._key_map_)
	{
	}
	insertion_ordered_map(insertion_ordered_map&& other)
	: insertion_ordered_map()
	{
		swap(other);
	}
	insertion_ordered_map & operator=(const insertion_ordered_map & other)
	{
		swap(insertion_ordered_map(other));
		return *this;
	}
	insertion_ordered_map & operator=(insertion_ordered_map&& other)
	{
		swap(other);
		return *this;
	}
#endif



#if(1) // Overloads of accessing map functions

	mapped_type& at (const key_type& k)
	{
		return _val_vector_.at(_key_map_.at(k)).second;
	}
	const mapped_type& at (const key_type& k) const
	{
		return _val_vector_.at(_key_map_.at(k)).second;
	}

	typename base_type::iterator find (const key_type& k)
    {
		auto it = _key_map_.find(k);
    	if(it==_key_map_.end()) return _val_vector_.end();
		return static_cast<typename base_type::iterator>(&_val_vector_[it->second]);
    }
	typename base_type::const_iterator find (const key_type& k) const
    {
		auto it = _key_map_.find(k);
    	if(it==_key_map_.end) return _val_vector_.end();
		return static_cast<typename base_type::iterator>(&_val_vector_[it->second]);
    }

    typename base_type::size_type count (const key_type& k) const
    {
		return _key_map_.count(k);
    }

    std::pair<typename base_type::const_iterator,typename base_type::const_iterator> equal_range
    	(const key_type& k) const
	{
		auto it = _key_map_.find(k);
    	if(it==_key_map_.end) return std::make_pair(_val_vector_.end(),_val_vector_.end());
    	auto vec_begin = static_cast<typename base_type::const_iterator>(&_val_vector_[it->second]);
    	auto vec_end = vec_begin;
    	++vec_end;
    	return std::make_pair(vec_begin,vec_end);
	}
    std::pair<typename base_type::iterator,typename base_type::iterator> equal_range
		(const key_type& k)
	{
		auto it = _key_map_.find(k);
    	if(it==_key_map_.end) return std::make_pair(_val_vector_.end(),_val_vector_.end());
    	auto vec_begin = static_cast<typename base_type::iterator>(&_val_vector_[it->second]);
    	auto vec_end = vec_begin;
    	++vec_end;
    	return std::make_pair(vec_begin,vec_end);
	}

#endif

    // More complicated overloads
#if(1)

    template< typename new_key_type >
    mapped_type & operator[](new_key_type && key)
	{
    	if(_key_map_.count(key)==0)
    	{
    		return _insert(std::make_pair(std::forward<new_key_type>(key),mapped_type())).second;
    	}
		return _val_vector_[_key_map_[std::forward<new_key_type>(key)]].second;
	}

    template< typename new_val_type,
	typename std::enable_if<std::is_convertible<new_val_type,value_type>::value,new_val_type>::type* = nullptr>
	std::pair<typename base_type::iterator,bool> insert(new_val_type && val)
	{
		if(_key_map_.count(val.first)==0)
		{
			_insert(std::forward<new_val_type>(val));
			return std::make_pair(static_cast<typename base_type::iterator>(&_val_vector_[_key_map_[val.first]]),
					true);
		}
		else
		{
			return std::make_pair(static_cast<typename base_type::iterator>(&_val_vector_[_key_map_[val.first]]),
					false);
		}
	}

    template< typename new_mapped_type, typename other_map_type = insertion_ordered_map<key_type,mapped_type>,
	typename std::enable_if<!std::is_convertible<new_mapped_type,value_type>::value,char>::type = 0,
	typename std::enable_if<std::is_convertible<new_mapped_type,mapped_type>::value,char>::type = 0,
	typename std::enable_if<std::is_convertible<key_type,typename other_map_type::key_type>::value,char>::type = 0>
	std::pair<typename base_type::iterator,bool> insert(new_mapped_type && val, const other_map_type & other_map = other_map_type())
	{
		_insert(std::make_pair(_new_key<key_type>(other_map),std::forward<new_mapped_type>(val)));
		return std::make_pair(_val_vector_.end(),true);
	}

	template <class... Args>
	std::pair<typename base_type::iterator,bool> emplace (Args&&... args)
	{
		value_type val(args...);
		key_type & key = val.first;
		return insert(val);
	}

	void erase (typename base_type::iterator position)
	{
		_val_vector_.erase(position);
		_rebuild_key_map();
	}
	typename base_type::size_type erase (const key_type& k)
	{
		if(_key_map_.count(k)==0) return ssize(_val_vector_);

		_val_vector_.erase(static_cast<typename base_type::iterator>(&_val_vector_[_key_map_.at(k)]));
		auto result=ssize(_val_vector_);
		_rebuild_key_map();
		return result;
	}
	void erase (typename base_type::iterator first, typename base_type::iterator last)
	{
		for(auto it=first;it!=last;++it)
		{
			_val_vector_.erase(it);
		}
		_rebuild_key_map();
	}

	void clear()
	{
		_val_vector_.clear();
		_key_map_.clear();
	}

#endif

    // Map functions we're using as-is
#if(1)
    typename base_type::iterator begin()
    {
    	return _val_vector_.begin();
    }
    typename base_type::const_iterator begin() const
    {
    	return _val_vector_.begin();
    }
    typename base_type::iterator end()
    {
    	return _val_vector_.end();
    }
    typename base_type::const_iterator end() const
    {
    	return _val_vector_.end();
    }
    typename base_type::reverse_iterator rbegin()
    {
    	return _val_vector_.rbegin();
    }
    typename base_type::const_reverse_iterator rbegin() const
    {
    	return _val_vector_.rbegin();
    }
    typename base_type::reverse_iterator rend()
    {
    	return _val_vector_.rend();
    }
    typename base_type::const_reverse_iterator rend() const
    {
    	return _val_vector_.rend();
    }
    typename base_type::const_iterator cbegin() const noexcept
    {
    	return _val_vector_.cbegin();
    }
    typename base_type::const_iterator cend() const noexcept
    {
    	return _val_vector_.cend();
    }
    typename base_type::const_reverse_iterator crbegin() const noexcept
    {
    	return _val_vector_.crbegin();
    }
    typename base_type::const_reverse_iterator crend() const noexcept
    {
    	return _val_vector_.crend();
    }

    bool empty() const
    {
    	return _val_vector_.empty();
    }
    typename base_type::size_type size() const
    {
    	return ssize(_val_vector_);
    }
    typename base_type::size_type max_size() const
    {
    	return _val_vector_.max_size();
    }
#endif

    // New methods
#if(1)

    /**
     * Changes a key to another, without altering its mapped data or position.
     *
     * @param init_key
     * @param new_key
     * @return char - 0 if successful
     *                1 if init_key doesn't exist in map
     *                2 if new_key already exists in map
     */
    template< typename new_key_type >
    char change_key(const key_type & init_key, new_key_type && new_key)
    {
    	// Get the position of the value we're going to be altering
    	auto it = _key_map_.find(init_key);

    	if(it==_key_map_.end()) return 1;
    	if(_key_map_.count(new_key)>0) return 2;

    	size_t pos = it->second;

    	// Alter the value in the key map by erasing old entry and adding new
    	_key_map_.erase(it);
    	_key_map_[std::forward<new_key_type>(new_key)] = pos;

    	// Alter the value in the vector
    	_val_vector_[pos].first = std::forward<new_key_type>(new_key);

    	return 0;
    }

#endif

};

} // namespace IceBRG

// Overloads of swap
template<typename key_type, typename mapped_type>
void swap (IceBRG::insertion_ordered_map<key_type, mapped_type> & same,
		IceBRG::insertion_ordered_map<key_type, mapped_type> & other)
{
	same.swap(other);
}

namespace std {
template<typename key_type, typename mapped_type>
void swap(IceBRG::insertion_ordered_map<key_type, mapped_type> & same,
		IceBRG::insertion_ordered_map<key_type, mapped_type> & other)
{
	same.swap(other);
}
}


#endif // _BRG_INSERTION_ORDERED_MAP_HPP_INCLUDED_
