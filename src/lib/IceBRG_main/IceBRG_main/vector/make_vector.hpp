/**********************************************************************\
 @file make_vector.hpp
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


#ifndef _BRG_MAKE_VECTOR_HPP_INCLUDED_
#define _BRG_MAKE_VECTOR_HPP_INCLUDED_

#include <iterator>
#include <type_traits>
#include <vector>

#include "IceBRG_main/common.h"

#include "IceBRG_main/container/coerce.hpp"
#include "IceBRG_main/utility.hpp"

namespace IceBRG
{

// Make a vector with default-constructed values
#if(1)

// Using specified lengths in each dimension
#if(1)

template<typename container>
void make_vector_default( container & vec, const typename container::size_type & d1)
{
	static_assert(std::is_default_constructible<typename container::value_type>::value,
			"make_default_vector requires that object type is default-constructible.");
	for(auto it=vec.begin(); it!=vec.end(); ++it)
	{
		*it = typename container::value_type();
	}
	vec.resize(d1);
}

template<typename container, typename... Args>
void make_vector_default( container & vec, const typename container::size_type & d1, const typename container::size_type & d2, Args... remaining_dims)
{
	vec.clear();
	vec.resize(d1);
	for(auto it=vec.begin(); it!=vec.end(); ++it)
	{
		make_vector_default(*it, d2, remaining_dims...);
	}
}

#endif // Using specified lengths in each dimension

#if(1) // Using the shape of another vector

template<int_type d, typename container, typename other_container>
struct vector_defaulter
{
	vector_defaulter(container & vec, const other_container & other_vec)
	{
		vec.resize(other_vec.size());
		auto o_it = other_vec.begin();

		for(auto it=vec.begin(); it!=vec.end(); ++it)
		{
			vector_defaulter<d-1,decltype(*it),decltype(*o_it)>(*it,*o_it);
		}
	}
};

template<typename container, typename other_container>
struct vector_defaulter<1,container,other_container>
{
	static_assert(std::is_default_constructible<typename container::value_type>::value,
			"make_default_vector requires that object type is default-constructible.");
	vector_defaulter(container & vec, const other_container & other_vec)
	{
		vec.clear();
		vec.resize(other_vec);
	}
};

template<typename container, typename other_container>
struct vector_defaulter<0,container,other_container>
{
	static_assert(std::is_default_constructible<container>::value,
			"make_default_vector requires that object type is default-constructible.");
	vector_defaulter(container & vec, const other_container & other_vec)
	{
		vec = container();
	}
};

template<int_type d, typename container, typename value_type, typename other_container>
void make_vector_default( container & vec, const value_type & val, const other_container & other_vec)
{
	vector_defaulter<d,container,other_container>(vec,other_vec);
}

#endif // Using the shape of another vector

#endif // Make a vector with default-constructed values

// Make a vector filled with zeroes
#if(1)

// Using specified lengths in each dimension
#if(1)

template<typename container>
void make_vector_zeroes( container & vec, const typename container::size_type & d1)
{
	vec.resize(d1);
	for(auto it=vec.begin(); it!=vec.end(); ++it)
	{
		set_zero(*it);
	}
}

template<typename container, typename... Args>
void make_vector_zeroes( container & vec, const typename container::size_type & d1, const typename container::size_type & d2, Args... remaining_dims)
{
	vec.clear();
	vec.resize(d1);
	for(auto it=vec.begin(); it!=vec.end(); ++it)
	{
		make_vector_zeroes(*it, d2, remaining_dims...);
	}
}

#endif // Using specified lengths in each dimension

#if(1) // Using the shape of another vector

template<int_type d, typename container, typename other_container>
struct vector_zeroer
{
	vector_zeroer(container & vec, const other_container & other_vec)
	{
		vec.resize(other_vec.size());
		auto o_it = other_vec.begin();

		for(auto it=vec.begin(); it!=vec.end(); ++it)
		{
			vector_zeroer<d-1,decltype(*it),decltype(*o_it)>(*it,*o_it);
		}
	}
};

template<typename container, typename other_container>
struct vector_zeroer<1,container,other_container>
{
	vector_zeroer(container & vec, const other_container & other_vec)
	{
		if(!other_vec.empty())
		{
			vec.resize(1);
			set_zero(vec.front());
			vec.resize(other_vec.size(),vec.front());
		}
		else
		{
			vec.resize(0);
		}
	}
};

template<typename container, typename other_container>
struct vector_zeroer<0,container,other_container>
{
	vector_zeroer(container & vec, const other_container & other_vec)
	{
		set_zero(vec);
	}
};

template<int_type d, typename container, typename value_type, typename other_container>
void make_vector_zeroes( container & vec, const value_type & val, const other_container & other_vec)
{
	vector_zeroer<d,container,other_container>(vec,other_vec);
}

#endif // Using the shape of another vector

#endif // Make a vector filled with zeroes

// Make a vector filled with a specified value
#if(1)

// Using specified lengths in each dimension
#if(1)

template<typename container, typename val_type>
void make_vector_value( container & vec, const val_type & val, const typename container::size_type & d1)
{
	vec.clear();
	vec.resize(d1,val);
}

template<typename container, typename val_type, typename... Args>
void make_vector_value( container & vec, const val_type & val, const typename container::size_type & d1, const typename container::size_type & d2, Args... remaining_dims)
{
	vec.clear();
	vec.resize(d1);
	for(auto it=vec.begin(); it!=vec.end(); ++it)
	{
		make_vector_value(*it, val, d2, remaining_dims...);
	}
}

#endif // Using specified lengths in each dimension

#if(1) // Using the shape of another vector

template<int_type d, typename container, typename value_type, typename other_container>
struct vector_valuer
{
	vector_valuer(container & vec, const value_type & val, const other_container & other_vec)
	{
		vec.resize(other_vec.size());
		auto o_it = other_vec.begin();

		for(auto it=vec.begin(); it!=vec.end(); ++it)
		{
			vector_valuer<d-1,decltype(*it),value_type,decltype(*o_it)>(*it,val,*o_it);
		}
	}
};

template<typename container, typename value_type, typename other_container>
struct vector_valuer<1,container,value_type,other_container>
{
	vector_valuer(container & vec, const value_type & val, const other_container & other_vec)
	{
		vec = container(other_vec.size(),val);
	}
};

template<typename container, typename value_type, typename other_container>
struct vector_valuer<0,container,value_type,other_container>
{
	vector_valuer(container & vec, const value_type & val, const other_container & other_vec)
	{
		vec = val;
	}
};

template<int_type d, typename container, typename value_type, typename other_container>
void make_vector_value( container & vec, const value_type & val, const other_container & other_vec)
{
	vector_valuer<d,container,value_type,other_container>(vec,val,other_vec);
}

#endif // Using the shape of another vector

#endif // Make a vector filled with a specified value

// Make a vector from a function
#if(1)

// Using specified lengths in each dimension
#if(1)

template<typename container, typename func_type>
void make_vector_function( container & vec, const func_type & func, const typename container::size_type & d1)
{
	vec.clear();
	vec.reserve(d1);

	typename container::size_type i(0);

	for(i=0; i<d1; ++i)
	{
		vec.push_back(func(i));
	}
}

template<typename container, typename func_type, typename... Args>
void make_vector_function( container & vec, const func_type & func, const typename container::size_type & d1, const typename container::size_type & d2, Args... remaining_dims)
{
	vec.clear();
	vec.resize(d1);

	typename container::size_type i(0);

	auto new_func = [&] (const typename container::size_type & i2, Args... remaining_is)
		{
			return func(i,i2,remaining_is...);
		};

	for(auto it=vec.begin(); it!=vec.end(); ++it)
	{
		make_vector_function(*it, new_func, d2, remaining_dims...);
		++i;
	}
}

#endif // Using specified lengths in each dimension

#if(1) // Using the shape of another vector

template<int_type d, typename container, typename func_type, typename other_container>
struct vector_functioner
{
	template<typename... Args>
	vector_functioner(container & vec, const func_type & func, const other_container & other_vec)
	{
		const typename container::size_type i;
		auto new_func = [&] (Args... args)
		{
			return func(i,args...);
		};

		vec.clear();
		vec.reserve(other_vec.size());
		for(i=0; i<other_vec.size(); ++i)
		{
			vector_functioner<d-1,decltype(vec[i]),decltype(new_func),decltype(other_vec[i])>
				(vec[i],new_func,other_vec[i]);
		}
	}
};

template<typename container, typename func_type, typename other_container>
struct vector_functioner<1,container,func_type,other_container>
{
	vector_functioner(container & vec, const func_type & func, const other_container & other_vec)
	{
		make_vector_function(vec,func,other_vec.size());
	}
};

template<typename container, typename func_type, typename other_container>
struct vector_functioner<0,container,func_type,other_container>
{
	vector_functioner(container & vec, const func_type & func, const other_container & other_vec)
	{
		vec = func();
	}
};

template<int_type d, typename container, typename func_type, typename other_container>
void make_vector_function( container & vec, const func_type & func, const other_container & other_vec)
{
	vector_functioner<d,container,func_type,other_container>(vec,func,other_vec);
}

#endif // Using the shape of another vector

#endif // Make a vector from a function

// Coerce from another container
#if(1)

template<int_type d, typename container, typename other_container>
void make_vector_coerce(container & vec, const other_container & other_vec)
{
	assignment_coercer<d,container>(vec,other_vec);
}

#endif

} // namespace IceBRG

#endif // _BRG_MAKE_VECTOR_HPP_INCLUDED_
