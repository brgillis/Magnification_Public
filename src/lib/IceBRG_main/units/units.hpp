/**********************************************************************\
 @file units.hpp
 ------------------

 TODO <Insert file description here>

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

#ifndef BRG_UNITS_UNITS_HPP_
#define BRG_UNITS_UNITS_HPP_

#include <cmath>

#include "IceBRG_main/common.h"

#include "IceBRG_main/container/is_boost_tuple.hpp"
#include "IceBRG_main/math/ipow.hpp"

// Enable dimensional analysis unless the NDEBUG flag is defined
#ifndef NDEBUG
#define _BRG_USE_UNITS_
#endif // #ifndef NDEBUG

#ifdef _BRG_USE_UNITS_

#include <boost/units/cmath.hpp>
#include <boost/units/derived_dimension.hpp>
#include <boost/units/is_quantity.hpp>
#include <boost/units/systems/si.hpp>
#include <boost/any.hpp>
#include <boost/serialization/split_free.hpp>

namespace IceBRG {

// Quantity types
#if(1)

typedef boost::any any_units_type;

typedef boost::units::quantity<boost::units::si::dimensionless> dimensionless_type;

typedef boost::units::quantity<boost::units::si::length> distance_type;
typedef boost::units::quantity<boost::units::si::area> area_type;
typedef boost::units::quantity<boost::units::si::wavenumber> inverse_distance_type;

typedef boost::units::quantity<boost::units::si::time> time_type;
typedef boost::units::quantity<boost::units::si::frequency> inverse_time_type;

typedef boost::units::quantity<boost::units::si::mass> mass_type;

typedef boost::units::quantity<boost::units::si::plane_angle> angle_type;
typedef decltype(angle_type()*angle_type()) square_angle_type;

typedef boost::units::quantity<boost::units::si::temperature> temperature_type;

typedef boost::units::quantity<boost::units::si::velocity> velocity_type;
typedef boost::units::quantity<boost::units::si::acceleration> acceleration_type;

typedef boost::units::quantity<boost::units::si::mass_density> density_type;
typedef boost::units::quantity<boost::units::si::surface_density> surface_density_type;

// Gotta be a bit hacky here to handle zeros - When defining a dimension, passing 0 as the exponent
// isn't treated the same as not passing it at all. We can get around that with decltype of a division
// by a power zero of that unit
template< short int d_p,
			short int t_p=0,
			short int m_p=0,
			short int a_p=0,
			short int T_p=0>
using custom_unit_type =
  decltype(boost::units::pow<(d_p)>(distance_type())/boost::units::pow<(0)>(distance_type()) *
		   boost::units::pow<(t_p)>(time_type())/boost::units::pow<(0)>(time_type()) *
		   boost::units::pow<(m_p)>(mass_type())/boost::units::pow<(0)>(mass_type()) *
		   boost::units::pow<(a_p)>(angle_type())/boost::units::pow<(0)>(angle_type()) *
		   boost::units::pow<(T_p)>(temperature_type())/boost::units::pow<(0)>(temperature_type()));

#endif // Quantity types

// Base unit values
#if(1)

const auto meter = boost::units::si::meter;
const auto second = boost::units::si::second;
const auto kilogram = boost::units::si::kilogram;
const auto radian = boost::units::si::radian;
const auto meter_per_second(boost::units::si::meter/boost::units::si::second);

const auto m = meter;
const auto s = second;
const auto kg = kilogram;
const auto rad = radian;
const auto mps = meter_per_second;

#endif // Base unit values

// Utility functions
#if(1)

using boost::any_cast;

template<typename T,
	typename std::enable_if<!std::is_same<T,any_units_type>::value &&
		boost::units::is_quantity<T>::value,char>::type = 0>
flt_type value_of( const T & x)
{
	return static_cast<flt_type>(x.value());
}
template<typename T,
typename std::enable_if<!std::is_same<T,any_units_type>::value &&
!boost::units::is_quantity<T>::value,char>::type = 0>
flt_type value_of( const T & x)
{
	return static_cast<flt_type>(x);
}
template<typename Tstored, typename T,
	typename std::enable_if<std::is_same<T,any_units_type>::value,char>::type = 0>
flt_type value_of( const T & x)
{
	return value_of(any_cast<Tstored>(x));
}

template<typename Tout, typename Tin, typename Tout_copy=Tout,
		typename std::enable_if<boost::units::is_quantity<Tout_copy>::value,char>::type = 0>
Tout units_cast( const Tin & x)
{
	return static_cast<Tout>(value_of(x) * typename Tout::unit_type());
}

template<typename Tout, typename Tin, typename Tout_copy=Tout,
		typename std::enable_if<!boost::units::is_quantity<Tout_copy>::value,char>::type = 0>
Tout units_cast( const Tin & x)
{
	return static_cast<Tout>(value_of(x));
}

template<typename Tout, typename Tin>
any_units_type any_units_cast( const Tin & x)
{
	return units_cast<Tout>(x);
}

#endif // Utility functions

// Math functions
#if(1)

template<typename T,
typename std::enable_if<boost::units::is_quantity<T>::value,char>::type = 0,
typename std::enable_if<!is_boost_tuple<T>::value,char>::type = 0>
T abs( const T & x)
{
	return boost::units::abs(x);
}
template<typename T,
typename std::enable_if<!boost::units::is_quantity<T>::value,char>::type = 0,
typename std::enable_if<!is_boost_tuple<T>::value,char>::type = 0>
T abs( const T & x)
{
	return std::fabs(x);
}

template<typename T,
typename std::enable_if<boost::units::is_quantity<T>::value,char>::type = 0,
typename std::enable_if<!is_boost_tuple<T>::value,char>::type = 0>
auto sqrt( const T & x) -> decltype(boost::units::sqrt(x))
{
	return boost::units::sqrt(x);
}
template<typename T,
typename std::enable_if<!boost::units::is_quantity<T>::value,char>::type = 0,
typename std::enable_if<!is_boost_tuple<T>::value,char>::type = 0>
auto sqrt( const T & x) -> decltype(std::sqrt(x))
{
	return std::sqrt(x);
}

template< int_type e, typename T,
typename std::enable_if<boost::units::is_quantity<T>::value,char>::type = 0,
typename std::enable_if<!is_boost_tuple<T>::value,char>::type = 0>
auto ipow(const T & x) -> decltype(boost::units::pow<e>(x))
{
	return boost::units::pow<e>(x);
}

template< int_type en, int_type ed, typename T,
typename std::enable_if<boost::units::is_quantity<T>::value,char>::type = 0,
typename std::enable_if<!is_boost_tuple<T>::value,char>::type = 0>
auto ipow( const T & x) -> decltype(boost::units::pow<boost::units::static_rational<en,ed>>(x))
{
	return boost::units::pow<boost::units::static_rational<en,ed>>(x);
}
template< int_type en, int_type ed, typename T,
typename std::enable_if<!boost::units::is_quantity<T>::value,char>::type = 0,
typename std::enable_if<!is_boost_tuple<T>::value,char>::type = 0>
auto ipow( const T & x) -> decltype(boost::units::pow<boost::units::static_rational<en,ed>>(x))
{
	return std::pow(x,static_cast<flt_type>(en)/ed);
}

template<typename T,
typename std::enable_if<boost::units::is_quantity<T>::value,char>::type = 0,
typename std::enable_if<!is_boost_tuple<T>::value,char>::type = 0>
flt_type sin( const T & x)
{
	return boost::units::sin(x);
}
template<typename T,
typename std::enable_if<!boost::units::is_quantity<T>::value,char>::type = 0,
typename std::enable_if<!is_boost_tuple<T>::value,char>::type = 0>
flt_type sin( const T & x)
{
	return std::sin(x);
}

template<typename T,
typename std::enable_if<boost::units::is_quantity<T>::value,char>::type = 0,
typename std::enable_if<!is_boost_tuple<T>::value,char>::type = 0>
flt_type cos( const T & x)
{
	return boost::units::cos(x);
}
template<typename T,
typename std::enable_if<!boost::units::is_quantity<T>::value,char>::type = 0,
typename std::enable_if<!is_boost_tuple<T>::value,char>::type = 0>
flt_type cos( const T & x)
{
	return std::cos(x);
}

template<typename T,
typename std::enable_if<boost::units::is_quantity<T>::value,char>::type = 0,
typename std::enable_if<!is_boost_tuple<T>::value,char>::type = 0>
flt_type tan( const T & x)
{
	return boost::units::tan(x);
}
template<typename T,
typename std::enable_if<!boost::units::is_quantity<T>::value,char>::type = 0,
typename std::enable_if<!is_boost_tuple<T>::value,char>::type = 0>
flt_type tan( const T & x)
{
	return std::tan(x);
}

template<typename T,
typename std::enable_if<boost::units::is_quantity<T>::value,char>::type = 0,
typename std::enable_if<!is_boost_tuple<T>::value,char>::type = 0>
angle_type asin( const T & x)
{
	return boost::units::asin(x);
}
template<typename T,
typename std::enable_if<!boost::units::is_quantity<T>::value,char>::type = 0,
typename std::enable_if<!is_boost_tuple<T>::value,char>::type = 0>
T asin( const T & x)
{
	return std::asin(x);
}

template<typename T,
typename std::enable_if<boost::units::is_quantity<T>::value,char>::type = 0,
typename std::enable_if<!is_boost_tuple<T>::value,char>::type = 0>
angle_type acos( const T & x)
{
	return boost::units::acos(x);
}
template<typename T,
typename std::enable_if<!boost::units::is_quantity<T>::value,char>::type = 0,
typename std::enable_if<!is_boost_tuple<T>::value,char>::type = 0>
T acos( const T & x)
{
	return std::acos(x);
}

template<typename T,
typename std::enable_if<boost::units::is_quantity<T>::value,char>::type = 0,
typename std::enable_if<!is_boost_tuple<T>::value,char>::type = 0>
angle_type atan( const T & y)
{
	return boost::units::atan(y);
}
template<typename T,
typename std::enable_if<!boost::units::is_quantity<T>::value,char>::type = 0,
typename std::enable_if<!is_boost_tuple<T>::value,char>::type = 0>
flt_type atan( const T & y)
{
	return std::atan(y);
}

template<typename T,
typename std::enable_if<boost::units::is_quantity<T>::value,char>::type = 0,
typename std::enable_if<!is_boost_tuple<T>::value,char>::type = 0>
angle_type atan2( const T & y, const T & x=1.)
{
	return boost::units::atan2(y,x);
}
template<typename T,
typename std::enable_if<!boost::units::is_quantity<T>::value,char>::type = 0,
typename std::enable_if<!is_boost_tuple<T>::value,char>::type = 0>
flt_type atan2( const T & y, const T & x=1.)
{
	return std::atan2(y,x);
}

#endif // Math functions

} // namespace IceBRG

// Serialization

namespace boost {
namespace serialization {

template<class Archive, class T>
void save(Archive & ar, const boost::units::quantity<T> & g, const unsigned int version)
{
	flt_type val = g.value();
    ar & val;
}
template<class Archive, class T>
void load(Archive & ar, boost::units::quantity<T> & g, const unsigned int version)
{
	flt_type val;
	ar & val;
    g = IceBRG::units_cast<boost::units::quantity<T>>(std::move(val));
}

template<class Archive, class T>
inline void serialize(Archive & ar, boost::units::quantity<T> & g, const unsigned int file_version)
{
        split_free(ar, g, file_version);
}

} // namespace serialization
} // namespace boost

#else // #ifdef _BRG_USE_UNITS_

namespace IceBRG {

template< short int d_p,
			short int t_p=0,
			short int m_p=0,
			short int a_p=0,
			short int T_p=0>
using custom_unit_type = flt_type;

typedef flt_type dimensionless_type;

typedef flt_type distance_type;
typedef flt_type area_type;
typedef flt_type inverse_distance_type;

typedef flt_type time_type;
typedef flt_type inverse_time_type;

typedef flt_type mass_type;

typedef flt_type angle_type;
typedef flt_type square_angle_type;

typedef flt_type temperature_type;

typedef flt_type velocity_type;
typedef flt_type acceleration_type;

typedef flt_type density_type;
typedef flt_type surface_density_type;

typedef flt_type any_units_type;

constexpr flt_type meter(1.);
constexpr flt_type second(1.);
constexpr flt_type kilogram(1.);
constexpr flt_type radian(1.);
constexpr flt_type meter_per_second(1.);

constexpr flt_type m = meter;
constexpr flt_type s = second;
constexpr flt_type kg = kilogram;
constexpr flt_type rad = radian;
constexpr flt_type mps = meter_per_second;

template<typename T>
inline flt_type value_of( const T & x)
{
	return static_cast<flt_type>(x);
}

template<typename Tout, typename Tin>
inline Tout units_cast( const Tin & x)
{
	return static_cast<Tout>(x);
}

template<typename Tout, typename Tin>
inline Tout any_cast( const Tin & x)
{
	return static_cast<Tout>(x);
}

template<typename Tout, typename Tin>
inline any_units_type any_units_cast( const Tin & x)
{
	return static_cast<Tout>(x);
}

// Math functions
#if(1)

template<typename T,
typename std::enable_if<!is_boost_tuple<T>::value,char>::type = 0>
inline T abs( const T & x)
{
	return std::abs(x);
}

using std::sqrt;

//template< int_type e>
//flt_type ipow( const flt_type & x)
//{
//	return ipow<e>(x);
//}

template< int_type en, int_type ed>
inline flt_type ipow( const flt_type & x)
{
	return std::pow(x,static_cast<flt_type>(en)/ed);
}

using std::sin;
using std::cos;
using std::tan;
using std::asin;
using std::acos;
using std::atan;
using std::atan2;

#endif // Math functions

} // namespace IceBRG


#endif // #ifdef _BRG_USE_UNITS_ // #else

#endif // BRG_UNITS_UNITS_HPP_
