/**********************************************************************\
 @file Eigen.hpp
 ------------------

 A file to include the Eigen library, plus set up some basic extensions
 for it - most notably, begin() and end() support

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

#ifndef _BRG_EIGEN_HPP_
#define _BRG_EIGEN_HPP_

#include <Eigen/Core>

#include "common.h"

// Global namespace begin and end definitions
#if(1)
template <typename Derived>
auto begin(const Eigen::EigenBase<Derived> & vec) -> decltype(vec.data())
{
	return vec.data();
}

template <typename Derived>
auto begin(Eigen::EigenBase<Derived> & vec) -> decltype(vec.data())
{
	return vec.data();
}

template <typename Derived>
auto end(const Eigen::EigenBase<Derived> & vec) -> decltype(vec.data())
{
	return vec.data()+vec.size();
}

template <typename Derived>
auto end(Eigen::EigenBase<Derived> & vec) -> decltype(vec.data())
{
	return vec.data()+vec.size();
}

#endif // Global namespace begin and end definitions

namespace IceBRG {

typedef Eigen::Array<flt_type,Eigen::Dynamic,1> flt_array_type;
typedef Eigen::Array<long_flt_type,Eigen::Dynamic,1> long_flt_array_type;

typedef Eigen::Array<int_type,Eigen::Dynamic,1> int_array_type;
typedef Eigen::Array<long_int_type,Eigen::Dynamic,1> long_int_array_type;
typedef Eigen::Array<short_int_type,Eigen::Dynamic,1> short_int_array_type;

typedef Eigen::Array<unsigned_int_type,Eigen::Dynamic,1> unsigned_int_array_type;
typedef Eigen::Array<long_unsigned_int_type,Eigen::Dynamic,1> long_unsigned_int_array_type;
typedef Eigen::Array<short_unsigned_int_type,Eigen::Dynamic,1> short_unsigned_int_array_type;

typedef Eigen::Array<complex_type,Eigen::Dynamic,1> complex_array_type;
typedef Eigen::Array<long_complex_type,Eigen::Dynamic,1> long_complex_array_type;

}

#endif // _BRG_EIGEN_HPP_
