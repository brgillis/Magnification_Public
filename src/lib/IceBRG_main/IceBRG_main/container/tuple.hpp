/**********************************************************************\
 @file tuple.hpp
 ------------------

 Element-wise operations for tuples of arbitrary length.

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

#ifndef BRG_CONTAINER_TUPLE_HPP_
#define BRG_CONTAINER_TUPLE_HPP_

#include <type_traits>
#include <utility>

#include <boost/tuple/tuple.hpp>
#include <boost/type_traits/is_convertible.hpp>

#include "IceBRG_main/container/is_container.hpp"
#include "IceBRG_main/units/units.hpp"
#include "IceBRG_main/vector/elementwise_functions.hpp" // So we have primary definition of math funcs


namespace IceBRG {

template <
  class T0 = boost::tuples::null_type, class T1 = boost::tuples::null_type,
  class T2 = boost::tuples::null_type,
  class T3 = boost::tuples::null_type, class T4 = boost::tuples::null_type,
  class T5 = boost::tuples::null_type,
  class T6 = boost::tuples::null_type, class T7 = boost::tuples::null_type,
  class T8 = boost::tuples::null_type,
  class T9 = boost::tuples::null_type>
using tuple = boost::tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>;

namespace tuples { // Various helper functions the end-user won't need to worry about

/// Helper function to construct a cons list with argument deduction
template<typename Th, typename Tt>
boost::tuples::cons<Th,Tt> make_cons(Th && h, Tt && t)
{
	return boost::tuples::cons<Th,Tt>(std::forward<Th>(h),std::forward<Tt>(t));
}

// add_typeof_helper
#if(1)

/**
 * Helper structure to determine the type of adding two values or tuples together.
 * This is needed since the compiler won't fully recurse function decltypes, but it
 * will fully recurse a structure.
 */
template< class T1, class T2, class Enable1 = void, class Enable2 = void>
struct add_typeof_helper
{
};

/**
 * Helper structure to determine the type of adding two values or tuples together.
 * This is needed since the compiler won't fully recurse function decltypes, but it
 * will fully recurse a structure.
 */
template< class T1, class T2>
struct add_typeof_helper<T1,T2,BRG_S_IS_TUPLE(T1), BRG_S_IS_TUPLE(T2)>
{
	typedef typename std::decay<T1>::type T1d;
	typedef typename std::decay<T2>::type T2d;

	typedef boost::tuples::cons<decltype(add(T1d().get_head(),T2d().get_head())),
			typename add_typeof_helper<decltype(T1d().get_tail()),decltype(T2d().get_tail())>::type> type;
};

/**
 * Helper structure to determine the type of adding two values or tuples together.
 * This is needed since the compiler won't fully recurse function decltypes, but it
 * will fully recurse a structure.
 */
template< class T1, class T2>
struct add_typeof_helper<T1,T2,BRG_S_IS_TUPLE(T1), BRG_S_NOT_TUPLE(T2)>
{
	typedef typename std::decay<T1>::type T1d;
	typedef typename std::decay<T2>::type T2d;

	typedef boost::tuples::cons<decltype(add(T1d().get_head(),T2d())),
			typename add_typeof_helper<decltype(T1d().get_tail()),T2>::type> type;
};

/**
 * Helper structure to determine the type of adding two values or tuples together.
 * This is needed since the compiler won't fully recurse function decltypes, but it
 * will fully recurse a structure.
 */
template< class T1, class T2>
struct add_typeof_helper<T1,T2,BRG_S_NOT_TUPLE(T1), BRG_S_IS_TUPLE(T2)>
{
	typedef typename std::decay<T1>::type T1d;
	typedef typename std::decay<T2>::type T2d;

	typedef boost::tuples::cons<decltype(add(T1d(),T2d().get_head())),
			typename add_typeof_helper<T1d,decltype(T2d().get_tail())>::type> type;
};

/**
 * Null type overload to end recursion.
 */
template<typename T>
struct add_typeof_helper<boost::tuples::null_type,T,void>
{
	typedef boost::tuples::null_type type;
};
/**
 * Null type overload to end recursion.
 */
template<typename T>
struct add_typeof_helper<T,boost::tuples::null_type,void>
{
	typedef boost::tuples::null_type type;
};
/**
 * Null type overload to end recursion.
 */
template<>
struct add_typeof_helper<boost::tuples::null_type,boost::tuples::null_type,void>
{
	typedef boost::tuples::null_type type;
};

#endif // add_typeof_helper

// subtract_typeof_helper
#if(1)

/**
 * Helper structure to determine the type of subtracting two values or tuples together.
 * This is needed since the compiler won't fully recurse function decltypes, but it
 * will fully recurse a structure.
 */
template< class T1, class T2, class Enable1 = void, class Enable2 = void>
struct subtract_typeof_helper
{
};

/**
 * Helper structure to determine the type of subtracting two values or tuples together.
 * This is needed since the compiler won't fully recurse function decltypes, but it
 * will fully recurse a structure.
 */
template< class T1, class T2>
struct subtract_typeof_helper<T1,T2,BRG_S_IS_TUPLE(T1), BRG_S_IS_TUPLE(T2)>
{
	typedef typename std::decay<T1>::type T1d;
	typedef typename std::decay<T2>::type T2d;

	typedef boost::tuples::cons<decltype(subtract(T1d().get_head(),T2d().get_head())),
			typename subtract_typeof_helper<decltype(T1d().get_tail()),decltype(T2d().get_tail())>::type> type;
};

/**
 * Helper structure to determine the type of subtracting two values or tuples together.
 * This is needed since the compiler won't fully recurse function decltypes, but it
 * will fully recurse a structure.
 */
template< class T1, class T2>
struct subtract_typeof_helper<T1,T2,BRG_S_IS_TUPLE(T1), BRG_S_NOT_TUPLE(T2)>
{
	typedef typename std::decay<T1>::type T1d;
	typedef typename std::decay<T2>::type T2d;

	typedef boost::tuples::cons<decltype(subtract(T1d().get_head(),T2d())),
			typename subtract_typeof_helper<decltype(T1d().get_tail()),T2>::type> type;
};

/**
 * Helper structure to determine the type of subtracting two values or tuples together.
 * This is needed since the compiler won't fully recurse function decltypes, but it
 * will fully recurse a structure.
 */
template< class T1, class T2>
struct subtract_typeof_helper<T1,T2,BRG_S_NOT_TUPLE(T1), BRG_S_IS_TUPLE(T2)>
{
	typedef typename std::decay<T1>::type T1d;
	typedef typename std::decay<T2>::type T2d;

	typedef boost::tuples::cons<decltype(subtract(T1d(),T2d().get_head())),
			typename subtract_typeof_helper<T1d,decltype(T2d().get_tail())>::type> type;
};

/**
 * Null type overload to end recursion.
 */
template<typename T>
struct subtract_typeof_helper<boost::tuples::null_type,T,void>
{
	typedef boost::tuples::null_type type;
};
/**
 * Null type overload to end recursion.
 */
template<typename T>
struct subtract_typeof_helper<T,boost::tuples::null_type,void>
{
	typedef boost::tuples::null_type type;
};
/**
 * Null type overload to end recursion.
 */
template<>
struct subtract_typeof_helper<boost::tuples::null_type,boost::tuples::null_type,void>
{
	typedef boost::tuples::null_type type;
};

#endif // subtract_typeof_helper

// multiply_typeof_helper
#if(1)

/**
 * Helper structure to determine the type of multiplying two values or tuples together.
 * This is needed since the compiler won't fully recurse function decltypes, but it
 * will fully recurse a structure.
 */
template< class T1, class T2, class Enable1 = void, class Enable2 = void>
struct multiply_typeof_helper
{
};

/**
 * Helper structure to determine the type of multiplying two values or tuples together.
 * This is needed since the compiler won't fully recurse function decltypes, but it
 * will fully recurse a structure.
 */
template< class T1, class T2>
struct multiply_typeof_helper<T1,T2,BRG_S_IS_TUPLE(T1), BRG_S_IS_TUPLE(T2)>
{
	typedef typename std::decay<T1>::type T1d;
	typedef typename std::decay<T2>::type T2d;

	typedef boost::tuples::cons<decltype(multiply(T1d().get_head(),T2d().get_head())),
			typename multiply_typeof_helper<decltype(T1d().get_tail()),decltype(T2d().get_tail())>::type> type;
};

/**
 * Helper structure to determine the type of multiplying two values or tuples together.
 * This is needed since the compiler won't fully recurse function decltypes, but it
 * will fully recurse a structure.
 */
template< class T1, class T2>
struct multiply_typeof_helper<T1,T2,BRG_S_IS_TUPLE(T1), BRG_S_NOT_TUPLE(T2)>
{
	typedef typename std::decay<T1>::type T1d;
	typedef typename std::decay<T2>::type T2d;

	typedef boost::tuples::cons<decltype(multiply(T1d().get_head(),T2d())),
			typename multiply_typeof_helper<decltype(T1d().get_tail()),T2>::type> type;
};

/**
 * Helper structure to determine the type of multiplying two values or tuples together.
 * This is needed since the compiler won't fully recurse function decltypes, but it
 * will fully recurse a structure.
 */
template< class T1, class T2>
struct multiply_typeof_helper<T1,T2,BRG_S_NOT_TUPLE(T1), BRG_S_IS_TUPLE(T2)>
{
	typedef typename std::decay<T1>::type T1d;
	typedef typename std::decay<T2>::type T2d;

	typedef boost::tuples::cons<decltype(multiply(T1d(),T2d().get_head())),
			typename multiply_typeof_helper<T1d,decltype(T2d().get_tail())>::type> type;
};

/**
 * Null type overload to end recursion.
 */
template<typename T>
struct multiply_typeof_helper<boost::tuples::null_type,T,void>
{
	typedef boost::tuples::null_type type;
};
/**
 * Null type overload to end recursion.
 */
template<typename T>
struct multiply_typeof_helper<T,boost::tuples::null_type,void>
{
	typedef boost::tuples::null_type type;
};
/**
 * Null type overload to end recursion.
 */
template<>
struct multiply_typeof_helper<boost::tuples::null_type,boost::tuples::null_type,void>
{
	typedef boost::tuples::null_type type;
};

#endif // multiply_typeof_helper

// divide_typeof_helper
#if(1)

/**
 * Helper structure to determine the type of divideing two values or tuples together.
 * This is needed since the compiler won't fully recurse function decltypes, but it
 * will fully recurse a structure.
 */
template< class T1, class T2, class Enable1 = void, class Enable2 = void>
struct divide_typeof_helper
{
};

/**
 * Helper structure to determine the type of divideing two values or tuples together.
 * This is needed since the compiler won't fully recurse function decltypes, but it
 * will fully recurse a structure.
 */
template< class T1, class T2>
struct divide_typeof_helper<T1,T2,BRG_S_IS_TUPLE(T1), BRG_S_IS_TUPLE(T2)>
{
	typedef typename std::decay<T1>::type T1d;
	typedef typename std::decay<T2>::type T2d;

	typedef boost::tuples::cons<decltype(divide(T1d().get_head(),T2d().get_head())),
			typename divide_typeof_helper<decltype(T1d().get_tail()),decltype(T2d().get_tail())>::type> type;
};

/**
 * Helper structure to determine the type of divideing two values or tuples together.
 * This is needed since the compiler won't fully recurse function decltypes, but it
 * will fully recurse a structure.
 */
template< class T1, class T2>
struct divide_typeof_helper<T1,T2,BRG_S_IS_TUPLE(T1), BRG_S_NOT_TUPLE(T2)>
{
	typedef typename std::decay<T1>::type T1d;
	typedef typename std::decay<T2>::type T2d;

	typedef boost::tuples::cons<decltype(divide(T1d().get_head(),T2d())),
			typename divide_typeof_helper<decltype(T1d().get_tail()),T2>::type> type;
};

/**
 * Helper structure to determine the type of divideing two values or tuples together.
 * This is needed since the compiler won't fully recurse function decltypes, but it
 * will fully recurse a structure.
 */
template< class T1, class T2>
struct divide_typeof_helper<T1,T2,BRG_S_NOT_TUPLE(T1), BRG_S_IS_TUPLE(T2)>
{
	typedef typename std::decay<T1>::type T1d;
	typedef typename std::decay<T2>::type T2d;

	typedef boost::tuples::cons<decltype(divide(T1d(),T2d().get_head())),
			typename divide_typeof_helper<T1d,decltype(T2d().get_tail())>::type> type;
};

/**
 * Null type overload to end recursion.
 */
template<typename T>
struct divide_typeof_helper<boost::tuples::null_type,T,void>
{
	typedef boost::tuples::null_type type;
};
/**
 * Null type overload to end recursion.
 */
template<typename T>
struct divide_typeof_helper<T,boost::tuples::null_type,void>
{
	typedef boost::tuples::null_type type;
};
/**
 * Null type overload to end recursion.
 */
template<>
struct divide_typeof_helper<boost::tuples::null_type,boost::tuples::null_type,void>
{
	typedef boost::tuples::null_type type;
};

#endif // divide_typeof_helper

// sqrt_typeof_helper
#if(1)

/**
 * Helper structure to determine the type of sqrting a tuple.
 * This is needed since the compiler won't fully recurse function decltypes, but it
 * will fully recurse a structure.
 */
template< class T1, class Enable1 = void, class Enable2 = void>
struct sqrt_typeof_helper
{
};

/**
 * Helper structure to determine the type of sqrting two values or tuples together.
 * This is needed since the compiler won't fully recurse function decltypes, but it
 * will fully recurse a structure.
 */
template< class T1>
struct sqrt_typeof_helper<T1,BRG_S_IS_TUPLE(T1)>
{
	typedef typename std::decay<T1>::type T1d;

	typedef boost::tuples::cons<decltype(sqrt(T1d().get_head())),
			typename sqrt_typeof_helper<decltype(T1d().get_tail())>::type> type;
};

/**
 * Null type overload to end recursion.
 */
template<>
struct sqrt_typeof_helper<boost::tuples::null_type,void>
{
	typedef boost::tuples::null_type type;
};

#endif // sqrt_typeof_helper

// ipow_typeof_helper
#if(1)

/**
 * Helper structure to determine the type of ipowing a tuple.
 * This is needed since the compiler won't fully recurse function decltypes, but it
 * will fully recurse a structure.
 */
template< int_type p, class T1, class Enable1 = void, class Enable2 = void>
struct ipow_typeof_helper
{
};

/**
 * Helper structure to determine the type of ipowing two values or tuples together.
 * This is needed since the compiler won't fully recurse function decltypes, but it
 * will fully recurse a structure.
 */
template< int_type p, class T1>
struct ipow_typeof_helper<p,T1,BRG_S_IS_TUPLE(T1)>
{
	typedef typename std::decay<T1>::type T1d;

	typedef boost::tuples::cons<decltype(ipow<p>(T1d().get_head())),
			typename ipow_typeof_helper<p,decltype(T1d().get_tail())>::type> type;
};

/**
 * Null type overload to end recursion.
 */
template< int_type p >
struct ipow_typeof_helper<p,boost::tuples::null_type,void>
{
	typedef boost::tuples::null_type type;
};

#endif // ipow_typeof_helper

// sct_typeof_helper - sin, cos, tan typeof helper
#if(1)

/**
 * Helper structure to determine the type of scting a tuple.
 * This is needed since the compiler won't fully recurse function decltypes, but it
 * will fully recurse a structure.
 */
template< class T1, class Enable1 = void, class Enable2 = void>
struct sct_typeof_helper
{
};

/**
 * Helper structure to determine the type of scting two values or tuples together.
 * This is needed since the compiler won't fully recurse function decltypes, but it
 * will fully recurse a structure.
 */
template< class T1>
struct sct_typeof_helper<T1,BRG_S_IS_TUPLE(T1)>
{
	typedef typename std::decay<T1>::type T1d;

	typedef boost::tuples::cons<decltype(sin(T1d().get_head())),
			typename sct_typeof_helper<decltype(T1d().get_tail())>::type> type;
};

/**
 * Null type overload to end recursion.
 */
template<>
struct sct_typeof_helper<boost::tuples::null_type,void>
{
	typedef boost::tuples::null_type type;
};

#endif // sct_typeof_helper

// asct_typeof_helper - asin, acos, atan typeof helper
#if(1)

/**
 * Helper structure to determine the type of scting a tuple.
 * This is needed since the compiler won't fully recurse function decltypes, but it
 * will fully recurse a structure.
 */
template< class T1, class Enable1 = void, class Enable2 = void>
struct asct_typeof_helper
{
};

/**
 * Helper structure to determine the type of scting two values or tuples together.
 * This is needed since the compiler won't fully recurse function decltypes, but it
 * will fully recurse a structure.
 */
template< class T1>
struct asct_typeof_helper<T1,BRG_S_IS_TUPLE(T1)>
{
	typedef typename std::decay<T1>::type T1d;

	typedef boost::tuples::cons<decltype(asin(T1d().get_head())),
			typename asct_typeof_helper<decltype(T1d().get_tail())>::type> type;
};

/**
 * Null type overload to end recursion.
 */
template<>
struct asct_typeof_helper<boost::tuples::null_type,void>
{
	typedef boost::tuples::null_type type;
};

#endif // asct_typeof_helper

// unary_rand_typeof_helper
#if(1)

/**
 * Helper structure to determine the type of generating a random tuple from a function.
 * This is needed since the compiler won't fully recurse function decltypes, but it
 * will fully recurse a structure.
 */
template< class f, class T1, class Enable1 = void>
struct unary_rand_typeof_helper
{
};

/**
 * Helper structure to determine the type of generating a random tuple from a function.
 * This is needed since the compiler won't fully recurse function decltypes, but it
 * will fully recurse a structure.
 */
template< class f, class T1>
struct unary_rand_typeof_helper<f,T1,BRG_S_IS_TUPLE(T1)>
{
	typedef typename std::decay<T1>::type T1d;

	typedef boost::tuples::cons<decltype(std::declval<f>()(T1d().get_head())),
			typename unary_rand_typeof_helper<f,decltype(T1d().get_tail())>::type> type;
};

/**
 * Null type overload to end recursion.
 */
template<class f>
struct unary_rand_typeof_helper<f,boost::tuples::null_type,void>
{
	typedef boost::tuples::null_type type;
};

#endif // unary_rand_typeof_helper

// binary_rand_typeof_helper
#if(1)

/**
 * Helper structure to determine the type of generating a random tuple from a function.
 * This is needed since the compiler won't fully recurse function decltypes, but it
 * will fully recurse a structure.
 */
template< class f, class T1, class T2, class Enable1 = void, class Enable2 = void>
struct binary_rand_typeof_helper
{
};

/**
 * Helper structure to determine the type of generating a random tuple from a function.
 * This is needed since the compiler won't fully recurse function decltypes, but it
 * will fully recurse a structure.
 */
template< class f, class T1, class T2>
struct binary_rand_typeof_helper<f,T1,T2,BRG_S_IS_TUPLE(T1), BRG_S_IS_TUPLE(T2)>
{
	typedef typename std::decay<T1>::type T1d;
	typedef typename std::decay<T2>::type T2d;

	typedef boost::tuples::cons<decltype(std::declval<f>()(T1d().get_head(),T2d().get_head())),
			typename binary_rand_typeof_helper<f,decltype(T1d().get_tail()),decltype(T2d().get_tail())>::type> type;
};

/**
 * Helper structure to determine the type of generating a random tuple from a function.
 * This is needed since the compiler won't fully recurse function decltypes, but it
 * will fully recurse a structure.
 */
template< class f, class T1, class T2>
struct binary_rand_typeof_helper<f,T1,T2,BRG_S_IS_TUPLE(T1), BRG_S_NOT_TUPLE(T2)>
{
	typedef typename std::decay<T1>::type T1d;
	typedef typename std::decay<T2>::type T2d;

	typedef boost::tuples::cons<decltype(std::declval<f>()(T1d().get_head(),T2d())),
			typename binary_rand_typeof_helper<f,decltype(T1d().get_tail()),T2>::type> type;
};

/**
 * Helper structure to determine the type of generating a random tuple from a function.
 * This is needed since the compiler won't fully recurse function decltypes, but it
 * will fully recurse a structure.
 */
template< class f, class T1, class T2>
struct binary_rand_typeof_helper<f,T1,T2,BRG_S_NOT_TUPLE(T1), BRG_S_IS_TUPLE(T2)>
{
	typedef typename std::decay<T1>::type T1d;
	typedef typename std::decay<T2>::type T2d;

	typedef boost::tuples::cons<decltype(std::declval<f>()(T1d(),T2d().get_head())),
			typename binary_rand_typeof_helper<f,T1d,decltype(T2d().get_tail())>::type> type;
};

/**
 * Null type overload to end recursion.
 */
template<class f, typename T>
struct binary_rand_typeof_helper<f,boost::tuples::null_type,T,void>
{
	typedef boost::tuples::null_type type;
};
/**
 * Null type overload to end recursion.
 */
template<class f, typename T>
struct binary_rand_typeof_helper<f,T,boost::tuples::null_type,void>
{
	typedef boost::tuples::null_type type;
};
/**
 * Null type overload to end recursion.
 */
template<class f>
struct binary_rand_typeof_helper<f,boost::tuples::null_type,boost::tuples::null_type,void>
{
	typedef boost::tuples::null_type type;
};

#endif // binary_rand_typeof_helper

} // namespace tuples

// for_each
#if(1)

// Unary for_each
#if(1)

template<typename f>
inline void unary_for_each( const f & func, const boost::tuples::null_type &)
{
	return;
}

template <typename f, class Th, class Tt>
inline void unary_for_each( const f & func, const boost::tuples::cons<Th, Tt> & t1)
{
	func(t1.get_head()); // Apply to head member

	unary_for_each(func,t1.get_tail()); // Recurse to next
}

template <typename f, class Th, class Tt>
inline void unary_for_each( const f & func, boost::tuples::cons<Th, Tt> & t1)
{
	func(t1.get_head()); // Apply to head member

	unary_for_each(func,t1.get_tail()); // Recurse to next
}

#endif // Unary for_each

// Binary for_each
#if(1)

template<typename f>
inline void binary_for_each( const f & func, const boost::tuples::null_type &,
		const boost::tuples::null_type &)
{
	return;
}

template <typename f, class Th, class Tt, typename T2>
inline void binary_for_each( const f & func, const boost::tuples::cons<Th, Tt> & t1,
		T2 && t2)
{
	func(t1.get_head(),t2.get_head()); // Apply to head members

	binary_for_each(func,t1.get_tail(),t2.get_tail()); // Recurse to next
}

template <typename f, class Th, class Tt, typename T2>
inline void binary_for_each( const f & func, boost::tuples::cons<Th, Tt> & t1,
		T2 && t2)
{
	func(t1.get_head(),t2.get_head()); // Apply to head members

	binary_for_each(func,t1.get_tail(),t2.get_tail()); // Recurse to next
}

#endif // Binary for_each

// Trinary for_each
#if(1)

template<typename f>
inline void trinary_for_each( const f & func, const boost::tuples::null_type &,
		const boost::tuples::null_type &)
{
	return;
}

template <typename f, class Th, class Tt, typename T2, typename T3>
inline void trinary_for_each( const f & func, const boost::tuples::cons<Th, Tt> & t1,
		T2 && t2, T3 && t3)
{
	func(t1.get_head(),t2.get_head(),t3.get_head()); // Apply to head members

	trinary_for_each(func,t1.get_tail(),t2.get_tail(),t3.get_tail()); // Recurse to next
}

template <typename f, class Th, class Tt, typename T2, typename T3>
inline void trinary_for_each( const f & func, boost::tuples::cons<Th, Tt> & t1,
		T2 && t2, T3 && t3)
{
	func(t1.get_head(),t2.get_head(),t3.get_head()); // Apply to head members

	trinary_for_each(func,t1.get_tail(),t2.get_tail(),t3.get_tail()); // Recurse to next
}

#endif // Trinary for_each

#endif // for_each

// Basic arithmetic
#if(1)

// Addition
#if(1)

template< class T1, class T2,BRG_F_EITHER_IS_NULL(T1,T2)>
inline boost::tuples::null_type add( T1 &&,
		T2 &&)
{
	return boost::tuples::null_type();
}

template <class T1, class T2,
BRG_F_IS_TUPLE(T1),
BRG_F_IS_TUPLE(T2)>
inline typename tuples::add_typeof_helper<T1,T2>::type add( const T1 & t1,
		const T2 & t2)
{
	return tuples::make_cons(add(t1.get_head(),t2.get_head()),add(t1.get_tail(),t2.get_tail()));
}

template <class T1, class T2,
BRG_F_IS_TUPLE(T1),
BRG_F_NOT_TUPLE(T2)>
inline typename tuples::add_typeof_helper<T1,T2>::type add( const T1 & t1,
		const T2 & t2)
{
	return tuples::make_cons(add(t1.get_head(),t2),add(t1.get_tail(),t2));
}

template <class T1, class T2,
BRG_F_NOT_TUPLE(T1),
BRG_F_IS_TUPLE(T2)>
inline typename tuples::add_typeof_helper<T1,T2>::type add( const T1 & t1,
		const T2 & t2)
{
	return tuples::make_cons(add(t1,t2.get_head()),add(t1,t2.get_tail()));
}

#endif // Addition

// Subtraction
#if(1)

template< class T1, class T2,BRG_F_EITHER_IS_NULL(T1,T2)>
inline boost::tuples::null_type subtract( T1 &&,
		T2 &&)
{
	return boost::tuples::null_type();
}

template <class T1, class T2,
BRG_F_IS_TUPLE(T1),
BRG_F_IS_TUPLE(T2)>
inline typename tuples::subtract_typeof_helper<T1,T2>::type subtract( const T1 & t1,
		const T2 & t2)
{
	return tuples::make_cons(subtract(t1.get_head(),t2.get_head()),subtract(t1.get_tail(),t2.get_tail()));
}

template <class T1, class T2,
BRG_F_IS_TUPLE(T1),
BRG_F_NOT_TUPLE(T2)>
inline typename tuples::subtract_typeof_helper<T1,T2>::type subtract( const T1 & t1,
		const T2 & t2)
{
	return tuples::make_cons(subtract(t1.get_head(),t2),subtract(t1.get_tail(),t2));
}

template <class T1, class T2,
BRG_F_NOT_TUPLE(T1),
BRG_F_IS_TUPLE(T2)>
inline typename tuples::subtract_typeof_helper<T1,T2>::type subtract( const T1 & t1,
		const T2 & t2)
{
	return tuples::make_cons(subtract(t1,t2.get_head()),subtract(t1,t2.get_tail()));
}

#endif // Subtraction

// Multiplication
#if(1)

template< class T1, class T2,BRG_F_EITHER_IS_NULL(T1,T2)>
inline boost::tuples::null_type multiply( T1 &&,
		T2 &&)
{
	return boost::tuples::null_type();
}

template <class T1, class T2,
BRG_F_IS_TUPLE(T1),
BRG_F_IS_TUPLE(T2)>
inline typename tuples::multiply_typeof_helper<T1,T2>::type multiply( const T1 & t1,
		const T2 & t2)
{
	return tuples::make_cons(multiply(t1.get_head(),t2.get_head()),multiply(t1.get_tail(),t2.get_tail()));
}

template <class T1, class T2,
BRG_F_IS_TUPLE(T1),
BRG_F_NOT_TUPLE(T2)>
inline typename tuples::multiply_typeof_helper<T1,T2>::type multiply( const T1 & t1,
		const T2 & t2)
{
	return tuples::make_cons(multiply(t1.get_head(),t2),multiply(t1.get_tail(),t2));
}

template <class T1, class T2,
BRG_F_NOT_TUPLE(T1),
BRG_F_IS_TUPLE(T2)>
inline typename tuples::multiply_typeof_helper<T1,T2>::type multiply( const T1 & t1,
		const T2 & t2)
{
	return tuples::make_cons(multiply(t1,t2.get_head()),multiply(t1,t2.get_tail()));
}

#endif // Multiplication

// Division
#if(1)

template< class T1, class T2,BRG_F_EITHER_IS_NULL(T1,T2)>
inline boost::tuples::null_type divide( T1 &&,
		T2 &&)
{
	return boost::tuples::null_type();
}

template <class T1, class T2,
BRG_F_IS_TUPLE(T1),
BRG_F_IS_TUPLE(T2)>
inline typename tuples::divide_typeof_helper<T1,T2>::type divide( const T1 & t1,
		const T2 & t2)
{
	return tuples::make_cons(divide(t1.get_head(),t2.get_head()),divide(t1.get_tail(),t2.get_tail()));
}

template <class T1, class T2,
BRG_F_IS_TUPLE(T1),
BRG_F_NOT_TUPLE(T2)>
inline typename tuples::divide_typeof_helper<T1,T2>::type divide( const T1 & t1,
		const T2 & t2)
{
	return tuples::make_cons(divide(t1.get_head(),t2),divide(t1.get_tail(),t2));
}

template <class T1, class T2,
BRG_F_NOT_TUPLE(T1),
BRG_F_IS_TUPLE(T2)>
inline typename tuples::divide_typeof_helper<T1,T2>::type divide( const T1 & t1,
		const T2 & t2)
{
	return tuples::make_cons(divide(t1,t2.get_head()),divide(t1,t2.get_tail()));
}

#endif // Division

#endif // Basic arithmetic

// Compound assignment-arithmetic
#if(1)

// Addition
#if(1)

template< class T1, class T2, BRG_F_IS_NULL(T1)>
inline const T1 & add_equal( const T1 & t1,
		T2 &&)
{
	return t1;
}

template <class T1, class T2,
BRG_F_IS_TUPLE(T1),
BRG_F_IS_TUPLE(T2)>
inline T1 & add_equal( T1 & t1,
		const T2 & t2)
{
	static_assert(boost::is_convertible<typename tuples::add_typeof_helper<T1,T2>::type,
			typename std::decay<T1>::type>::value,
			"add_equal cannot be compiled due to incompatible types.");

	add_equal(t1.get_head(),t2.get_head()); // Add heads
	add_equal(t1.get_tail(),t2.get_tail()); // Recursively add tails

	return t1;
}

template <class T1, class T2,
BRG_F_IS_TUPLE(T1),
BRG_F_NOT_TUPLE(T2)>
inline T1 & add_equal( T1 & t1,
		const T2 & t2)
{
	static_assert(boost::is_convertible<typename tuples::add_typeof_helper<T1,T2>::type,
			typename std::decay<T1>::type>::value,
			"add_equal cannot be compiled due to incompatible types.");

	add_equal(t1.get_head(),t2); // Add to head
	add_equal(t1.get_tail(),t2); // Recursively add to tail

	return t1;
}

#endif // Addition

// Subtraction
#if(1)

template< class T1, class T2, BRG_F_IS_NULL(T1)>
inline const T1 & subtract_equal( const T1 & t1,
		T2 &&)
{
	return t1;
}

template <class T1, class T2,
BRG_F_IS_TUPLE(T1),
BRG_F_IS_TUPLE(T2)>
inline T1 & subtract_equal( T1 & t1,
		const T2 & t2)
{
	static_assert(boost::is_convertible<typename tuples::subtract_typeof_helper<T1,T2>::type,
			typename std::decay<T1>::type>::value,
			"subtract_equal cannot be compiled due to incompatible types.");

	subtract_equal(t1.get_head(),t2.get_head()); // subtract heads
	subtract_equal(t1.get_tail(),t2.get_tail()); // Recursively subtract tails

	return t1;
}

template <class T1, class T2,
BRG_F_IS_TUPLE(T1),
BRG_F_NOT_TUPLE(T2)>
inline T1 & subtract_equal( T1 & t1,
		const T2 & t2)
{
	static_assert(boost::is_convertible<typename tuples::subtract_typeof_helper<T1,T2>::type,
			typename std::decay<T1>::type>::value,
			"subtract_equal cannot be compiled due to incompatible types.");

	subtract_equal(t1.get_head(),t2); // subtract from head
	subtract_equal(t1.get_tail(),t2); // Recursively subtract from tail

	return t1;
}

#endif // Subtraction

// Multiplication
#if(1)

template< class T1, class T2, BRG_F_IS_NULL(T1)>
inline const T1 & multiply_equal( const T1 & t1,
		T2 &&)
{
	return t1;
}

template <class T1, class T2,
BRG_F_IS_TUPLE(T1),
BRG_F_IS_TUPLE(T2)>
inline T1 & multiply_equal( T1 & t1,
		const T2 & t2)
{
	static_assert(boost::is_convertible<typename tuples::multiply_typeof_helper<T1,T2>::type,
			typename std::decay<T1>::type>::value,
			"multiply_equal cannot be compiled due to incompatible types.");

	multiply_equal(t1.get_head(),t2.get_head()); // multiply heads
	multiply_equal(t1.get_tail(),t2.get_tail()); // Recursively multiply tails

	return t1;
}

template <class T1, class T2,
BRG_F_IS_TUPLE(T1),
BRG_F_NOT_TUPLE(T2)>
inline T1 & multiply_equal( T1 & t1,
		const T2 & t2)
{
	static_assert(boost::is_convertible<typename tuples::multiply_typeof_helper<T1,T2>::type,
			typename std::decay<T1>::type>::value,
			"multiply_equal cannot be compiled due to incompatible types.");

	multiply_equal(t1.get_head(),t2); // multiply with head
	multiply_equal(t1.get_tail(),t2); // Recursively multiply with tail

	return t1;
}

#endif // Multiplication

// Division
#if(1)

template< class T1, class T2, BRG_F_IS_NULL(T1)>
inline const T1 & divide_equal( const T1 & t1,
		T2 &&)
{
	return t1;
}

template <class T1, class T2,
BRG_F_IS_TUPLE(T1),
BRG_F_IS_TUPLE(T2)>
inline T1 & divide_equal( T1 & t1,
		const T2 & t2)
{
	static_assert(boost::is_convertible<typename tuples::divide_typeof_helper<T1,T2>::type,
			typename std::decay<T1>::type>::value,
			"divide_equal cannot be compiled due to incompatible types.");

	divide_equal(t1.get_head(),t2.get_head()); // divide heads
	divide_equal(t1.get_tail(),t2.get_tail()); // Recursively divide tails

	return t1;
}

template <class T1, class T2,
BRG_F_IS_TUPLE(T1),
BRG_F_NOT_TUPLE(T2)>
inline T1 & divide_equal( T1 & t1,
		const T2 & t2)
{
	static_assert(boost::is_convertible<typename tuples::divide_typeof_helper<T1,T2>::type,
			typename std::decay<T1>::type>::value,
			"divide_equal cannot be compiled due to incompatible types.");

	divide_equal(t1.get_head(),t2); // divide from head
	divide_equal(t1.get_tail(),t2); // Recursively divide from tail

	return t1;
}

#endif // Division

#endif // Compound assignment-arithmetic

// Advanced and misc operations
#if(1)

// abs
#if(1)

template<class T1,
BRG_F_IS_NULL(T1)>
inline boost::tuples::null_type abs( const T1 & t1)
{
	return boost::tuples::null_type();
}

template <class T1,
BRG_F_IS_TUPLE(T1)>
inline T1 abs( const T1 & t1 )
{
	return tuples::make_cons(abs(t1.get_head()),abs(t1.get_tail()));
}

#endif // abs

// sqrt
#if(1)

template<class T1,
BRG_F_IS_NULL(T1)>
inline boost::tuples::null_type sqrt( const T1 & t1)
{
	return boost::tuples::null_type();
}

template <class T1,
BRG_F_IS_TUPLE(T1)>
inline typename tuples::sqrt_typeof_helper<T1>::type sqrt( const T1 & t1 )
{
	return tuples::make_cons(sqrt(t1.get_head()),sqrt(t1.get_tail()));
}

#endif // sqrt

// pow
#if(1)

template<class T1, class T2,
BRG_F_IS_NULL(T1),
BRG_F_IS_NULL(T2)>
inline boost::tuples::null_type pow( const T1 & t1,
		const T2 & t2)
{
	return boost::tuples::null_type();
}

template <class T1, class T2,
BRG_F_IS_TUPLE(T1),
BRG_F_IS_TUPLE(T2)>
inline T1 pow( const T1 & t1,
		const T2 & t2)
{
	return tuples::make_cons(pow(t1.get_head(),t2.get_head()),pow(t1.get_tail(),t2.get_tail()));
}

template <class T1, class T2,
BRG_F_IS_TUPLE(T1),
BRG_F_NOT_TUPLE(T2)>
inline T1 pow( const T1 & t1,
		const T2 & t2)
{
	return tuples::make_cons(pow(t1.get_head(),t2),pow(t1.get_tail(),t2));
}

template <class T1, class T2,
BRG_F_NOT_TUPLE(T1),
BRG_F_IS_TUPLE(T2)>
inline T1 pow( const T1 & t1,
		const T2 & t2)
{
	return tuples::make_cons(pow(t1,t2.get_head()),pow(t1,t2.get_tail()));
}

#endif // pow

// runtime_ipow
#if(1)

template< class T1, class T2,BRG_F_EITHER_IS_NULL(T1,T2)>
inline boost::tuples::null_type runtime_ipow( const T1 & t1,
		T2 &&)
{
	return boost::tuples::null_type();
}

template <class T1, class T2,
BRG_F_IS_TUPLE(T1),
BRG_F_IS_TUPLE(T2)>
inline T1 runtime_ipow( const T1 & t1,
		const T2 & t2)
{
	return tuples::make_cons(runtime_ipow(t1.get_head(),t2.get_head()),runtime_ipow(t1.get_tail(),t2.get_tail()));
}

template <class T1, class T2,
BRG_F_IS_TUPLE(T1),
BRG_F_NOT_TUPLE(T2)>
inline T1 runtime_ipow( const T1 & t1,
		const T2 & t2)
{
	return tuples::make_cons(runtime_ipow(t1.get_head(),t2),runtime_ipow(t1.get_tail(),t2));
}

template <class T1, class T2,
BRG_F_NOT_TUPLE(T1),
BRG_F_IS_TUPLE(T2)>
inline T1 runtime_ipow( const T1 & t1,
		const T2 & t2)
{
	return tuples::make_cons(runtime_ipow(t1,t2.get_head()),runtime_ipow(t1,t2.get_tail()));
}

#endif // runtime_ipow

// ipow
#if(1)

template< int_type p >
inline boost::tuples::null_type ipow( const boost::tuples::null_type & t1)
{
	return boost::tuples::null_type();
}

template < int_type p, class T1,
BRG_F_IS_TUPLE(T1)>
inline typename tuples::ipow_typeof_helper<p,T1>::type ipow( const T1 & t1 )
{
	return tuples::make_cons(ipow<p>(t1.get_head()),ipow<p>(t1.get_tail()));
}

#endif // ipow

#endif // Advanced math functions

// Trigonometric functions
#if(1)

// sin
#if(1)

template< class T1, BRG_F_IS_NULL(T1)>
inline boost::tuples::null_type sin( const T1 & t1)
{
	return boost::tuples::null_type();
}

template <class T1,
BRG_F_IS_TUPLE(T1)>
inline typename tuples::sct_typeof_helper<T1>::type sin( const T1 & t1 )
{
	return tuples::make_cons(sin(t1.get_head()),sin(t1.get_tail()));
}

#endif // sin

// cos
#if(1)

template< class T1, BRG_F_IS_NULL(T1)>
inline boost::tuples::null_type cos( const T1 & t1)
{
	return boost::tuples::null_type();
}

template <class T1,
BRG_F_IS_TUPLE(T1)>
inline typename tuples::sct_typeof_helper<T1>::type cos( const T1 & t1 )
{
	return tuples::make_cons(cos(t1.get_head()),cos(t1.get_tail()));
}

#endif // cos

// tan
#if(1)

template< class T1, BRG_F_IS_NULL(T1)>
inline boost::tuples::null_type tan( const T1 & t1)
{
	return boost::tuples::null_type();
}

template <class T1,
BRG_F_IS_TUPLE(T1)>
inline typename tuples::sct_typeof_helper<T1>::type tan( const T1 & t1 )
{
	return tuples::make_cons(tan(t1.get_head()),tan(t1.get_tail()));
}

#endif // tan

// asin
#if(1)

template< class T1, BRG_F_IS_NULL(T1)>
inline boost::tuples::null_type asin( const T1 & t1)
{
	return boost::tuples::null_type();
}

template <class T1,
BRG_F_IS_TUPLE(T1)>
inline typename tuples::asct_typeof_helper<T1>::type asin( const T1 & t1 )
{
	return tuples::make_cons(asin(t1.get_head()),asin(t1.get_tail()));
}

#endif // asin

// acos
#if(1)

template< class T1, BRG_F_IS_NULL(T1)>
inline boost::tuples::null_type acos( const T1 & t1)
{
	return boost::tuples::null_type();
}

template <class T1,
BRG_F_IS_TUPLE(T1)>
inline typename tuples::asct_typeof_helper<T1>::type acos( const T1 & t1 )
{
	return tuples::make_cons(acos(t1.get_head()),acos(t1.get_tail()));
}

#endif // acos

// atan
#if(1)

template< class T1, BRG_F_IS_NULL(T1)>
inline boost::tuples::null_type atan( const T1 & t1)
{
	return boost::tuples::null_type();
}

template <class T1,
BRG_F_IS_TUPLE(T1)>
inline typename tuples::asct_typeof_helper<T1>::type atan( const T1 & t1 )
{
	return tuples::make_cons(atan(t1.get_head()),atan(t1.get_tail()));
}

#endif // atan

#endif // Trig functions

// Min/max
#if(1)

template<class T1, class T2,
BRG_F_IS_NULL(T1),
BRG_F_IS_NULL(T2)>
inline boost::tuples::null_type min( const T1 & t1,
		const T2 & t2)
{
	return boost::tuples::null_type();
}

template <class T1, class T2,
BRG_F_IS_TUPLE(T1),
BRG_F_IS_TUPLE(T2)>
inline T1 min( const T1 & t1,
		const T2 & t2)
{
	return tuples::make_cons(min(t1.get_head(),t2.get_head()),min(t1.get_tail(),t2.get_tail()));
}

template <class T1, class T2,
BRG_F_IS_TUPLE(T1),
BRG_F_NOT_TUPLE(T2)>
inline T1 min( const T1 & t1,
		const T2 & t2)
{
	return tuples::make_cons(min(t1.get_head(),t2),min(t1.get_tail(),t2));
}

template <class T1, class T2,
BRG_F_NOT_TUPLE(T1),
BRG_F_IS_TUPLE(T2)>
inline T1 min( const T1 & t1,
		const T2 & t2)
{
	return tuples::make_cons(min(t1,t2.get_head()),min(t1,t2.get_tail()));
}

template<class T1, class T2,
BRG_F_IS_NULL(T1),
BRG_F_IS_NULL(T2)>
inline boost::tuples::null_type max( const T1 & t1,
		const T2 & t2)
{
	return boost::tuples::null_type();
}

template <class T1, class T2,
BRG_F_IS_TUPLE(T1),
BRG_F_IS_TUPLE(T2)>
inline T1 max( const T1 & t1,
		const T2 & t2)
{
	return tuples::make_cons(max(t1.get_head(),t2.get_head()),max(t1.get_tail(),t2.get_tail()));
}

template <class T1, class T2,
BRG_F_IS_TUPLE(T1),
BRG_F_NOT_TUPLE(T2)>
inline T1 max( const T1 & t1,
		const T2 & t2)
{
	return tuples::make_cons(max(t1.get_head(),t2),max(t1.get_tail(),t2));
}

template <class T1, class T2,
BRG_F_NOT_TUPLE(T1),
BRG_F_IS_TUPLE(T2)>
inline T1 max( const T1 & t1,
		const T2 & t2)
{
	return tuples::make_cons(max(t1,t2.get_head()),max(t1,t2.get_tail()));
}

#endif

// Random generation
#if(1)

template<typename f, typename T1, BRG_F_IS_NULL(T1) >
boost::tuples::null_type rand_container(const f func, const T1 & v1)
{
	return boost::tuples::null_type();
}
template<typename f, typename T1, BRG_F_IS_TUPLE(T1) >
typename tuples::unary_rand_typeof_helper<f,T1>::type rand_container(const f func, const T1 & v1)
{
	typename tuples::unary_rand_typeof_helper<f,T1>::type vo;

	vo.get_head() = func(v1.get_head());

	vo.get_tail() = rand_container(func,v1.get_tail());

	return vo;
}

template<typename f, typename T1, typename T2, BRG_F_IS_NULL(T1), BRG_F_IS_NULL(T2) >
boost::tuples::null_type rand_container(const f func, const T1 & v1, const T2 & v2)
{
	return boost::tuples::null_type();
}
template<typename f, typename T1, typename T2, BRG_F_IS_TUPLE(T1), BRG_F_IS_TUPLE(T2) >
typename tuples::binary_rand_typeof_helper<f,T1,T2>::type rand_container(const f func, const T1 & v1, const T2 & v2)
{
	assert(ssize(v1)==ssize(v2));

	typename tuples::binary_rand_typeof_helper<f,T1,T2>::type vo;

	vo.get_head() = func(v1.get_head(),v2.get_head());

	vo.get_tail() = rand_container(func,v1.get_tail(),v2.get_tail());

	return vo;
}

template<typename f, typename T1, typename T2, BRG_F_IS_NULL(T1), BRG_F_NOT_CONTAINER(T2) >
boost::tuples::null_type rand_container(const f func, const T1 & v1, const T2 & v2)
{
	return boost::tuples::null_type();
}
template<typename f, typename T1, typename T2, BRG_F_IS_TUPLE(T1), BRG_F_NOT_CONTAINER(T2) >
typename tuples::binary_rand_typeof_helper<f,T1,T2>::type rand_container(const f func, const T1 & v1, const T2 & v2)
{
	typename tuples::binary_rand_typeof_helper<f,T1,T2>::type vo;

	vo.get_head() = func(v1.get_head(),v2);

	vo.get_tail() = rand_container(func,v1.get_tail(),v2);

	return vo;
}

template<typename f, typename T1, typename T2, BRG_F_NOT_CONTAINER(T1), BRG_F_IS_NULL(T2) >
boost::tuples::null_type rand_container(const f func, const T1 & v1, const T2 & v2)
{
	return boost::tuples::null_type();
}
template<typename f, typename T1, typename T2, BRG_F_NOT_CONTAINER(T1), BRG_F_IS_TUPLE(T2) >
typename tuples::binary_rand_typeof_helper<f,T1,T2>::type rand_container(const f func, const T1 & v1, const T2 & v2)
{
	typename tuples::binary_rand_typeof_helper<f,T1,T2>::type vo;

	vo.get_head() = func(v1,v2.get_head());

	vo.get_tail() = rand_container(func,v1,v2.get_tail());

	return vo;
}

#endif

// Random generation for a given result type
#if(1)

template<typename T, typename f, typename Tout=typename std::decay<T>::type, BRG_F_IS_NULL(Tout) >
boost::tuples::null_type rand_container_of_size(const f func,
		const int_type &)
{
	return boost::tuples::null_type();
}
template<typename T, typename f, typename Tout=typename std::decay<T>::type, BRG_F_IS_TUPLE(Tout) >
Tout rand_container_of_size(const f func,
		const int_type &)
{
	Tout vo;

	vo.get_head() = func();

	vo.get_tail() = rand_container_of_size<decltype(vo.get_tail())>(func,
			ssize(vo)-1);

	return vo;
}

template<typename T, typename f, typename T1, typename Tout=typename std::decay<T>::type,
		BRG_F_IS_NULL(Tout) >
boost::tuples::null_type rand_container_of_size(const f func, const T1 & v1,
		const int_type &)
{
	return boost::tuples::null_type();
}
template<typename T, typename f, typename T1, typename Tout=typename std::decay<T>::type,
		BRG_F_IS_TUPLE(Tout),
		BRG_F_IS_TUPLE(T1) >
Tout rand_container_of_size(const f func, const T1 & v1,
		const int_type &)
{
	Tout vo;

	assert(ssize(vo)==ssize(v1));

	vo.get_head() = func(v1.get_head());

	vo.get_tail() = rand_container_of_size<decltype(vo.get_tail())>(func,v1.get_tail(),
			ssize(vo)-1);

	return vo;
}

template<typename T, typename f, typename T1, typename T2, typename Tout=typename std::decay<T>::type,
		BRG_F_IS_NULL(Tout) >
boost::tuples::null_type rand_container_of_size(const f func, const T1 & v1, const T2 & v2,
		const int_type &)
{
	return boost::tuples::null_type();
}
template<typename T, typename f, typename T1, typename T2, typename Tout=typename std::decay<T>::type,
		BRG_F_IS_TUPLE(Tout),
		BRG_F_IS_TUPLE(T1), BRG_F_IS_TUPLE(T2) >
Tout rand_container_of_size(const f func, const T1 & v1,
		const T2 & v2, const int_type &)
{
	assert(ssize(v1)==ssize(v2));

	Tout vo;

	assert(ssize(vo)==ssize(v1));

	vo.get_head() = func(v1.get_head(),v2.get_head());

	vo.get_tail() = rand_container_of_size<decltype(vo.get_tail())>(func,v1.get_tail(),v2.get_tail(),
			ssize(vo)-1);

	return vo;
}

template<typename T, typename f, typename T1, typename T2, typename Tout=typename std::decay<T>::type,
		BRG_F_IS_TUPLE(Tout),
		BRG_F_IS_TUPLE(T1), BRG_F_NOT_CONTAINER(T2) >
Tout rand_container_of_size(const f func, const T1 & v1,
		const T2 & v2, const int_type &)
{
	Tout vo;

	assert(ssize(vo)==ssize(v1));

	vo.get_head() = func(v1.get_head(),v2);

	vo.get_tail() = rand_container_of_size<decltype(vo.get_tail())>(func,v1.get_tail(),v2,
			ssize(vo)-1);

	return vo;
}

template<typename T, typename f, typename T1, typename T2, typename Tout=typename std::decay<T>::type,
		BRG_F_IS_TUPLE(Tout),
		BRG_F_NOT_CONTAINER(T1), BRG_F_IS_TUPLE(T2) >
Tout rand_container_of_size(const f func, const T1 & v1,
		const T2 & v2, const int_type &)
{
	Tout vo;

	assert(ssize(vo)==ssize(v2));

	vo.get_head() = func(v1,v2.get_head());

	vo.get_tail() = rand_container_of_size<decltype(vo.get_tail())>(func,v1,v2.get_head(),
			ssize(vo)-1);

	return vo;
}

template<typename T, typename f, typename T1, typename T2, typename Tout=typename std::decay<T>::type,
		BRG_F_IS_TUPLE(Tout),
		BRG_F_NOT_CONTAINER(T1), BRG_F_NOT_CONTAINER(T2) >
Tout rand_container_of_size(const f func, const T1 & v1,
		const T2 & v2, const int_type &)
{
	Tout vo;

	vo.get_head() = func(v1,v2);

	vo.get_tail() = rand_container_of_size<decltype(vo.get_tail())>(func,v1,v2,
			ssize(vo)-1);

	return vo;
}

#endif

}

#endif // BRG_CONTAINER_TUPLE_HPP_
