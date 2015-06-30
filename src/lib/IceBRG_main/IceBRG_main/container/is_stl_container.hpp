/**********************************************************************\
 @file is_stl_container.hpp
 ------------------

 Credit to Nawaz on StackOverflow at:
 http://stackoverflow.com/a/9407521

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


#ifndef _BRG_CONTAINER_IS_STL_CONTAINER_HPP_INCLUDED_
#define _BRG_CONTAINER_IS_STL_CONTAINER_HPP_INCLUDED_

namespace IceBRG
{

template<typename T>
struct has_const_iterator
{
private:
    typedef char                      yes;
    typedef struct { char array[2]; } no;

    template<typename C> static yes test(typename C::const_iterator*);
    template<typename C> static no  test(...);
public:
    static const bool value = sizeof(test<T>(0)) == sizeof(yes);
    typedef T type;
};

template<typename T>
struct has_iterator
{
private:
    typedef char                      yes;
    typedef struct { char array[2]; } no;

    template<typename C> static yes test(typename C::iterator*);
    template<typename C> static no  test(...);
public:
    static const bool value = sizeof(test<T>(0)) == sizeof(yes);
    typedef T type;
};

template <typename T>
struct has_begin_end
{
    template<typename C> static char (&f(typename std::enable_if<
      std::is_same<decltype(static_cast<typename C::const_iterator (C::*)() const>(&C::begin)),
      typename C::const_iterator(C::*)() const>::value, void>::type*))[1];

    template<typename C> static char (&f(...))[2];

    template<typename C> static char (&g(typename std::enable_if<
      std::is_same<decltype(static_cast<typename C::const_iterator (C::*)() const>(&C::end)),
      typename C::const_iterator(C::*)() const>::value, void>::type*))[1];

    template<typename C> static char (&g(...))[2];

    static bool const beg_value = sizeof(f<T>(0)) == 1;
    static bool const end_value = sizeof(g<T>(0)) == 1;
};

template<typename T>
struct is_stl_container : std::integral_constant<bool, has_const_iterator<typename std::decay<T>::type>::value &&
	has_begin_end<typename std::decay<T>::type>::beg_value && has_begin_end<typename std::decay<T>::type>::end_value>
{ };

} // end namespace IceBRG



#endif // _BRG_CONTAINER_IS_STL_CONTAINER_HPP_INCLUDED_
