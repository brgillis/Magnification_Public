/**********************************************************************\
 @file DE.hpp
 ------------------

 Functions to be used with differential equations (DEs).

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

#ifndef _BRG_DE_HPP_INCLUDED_
#define _BRG_DE_HPP_INCLUDED_

#include <type_traits>

#include "IceBRG_main/common.h"

#include "IceBRG_main/math/misc_math.hpp"

namespace IceBRG {

// Leapfrog method for solving a DE. Note that this implementation assumes that the positions and velocities passed to it are already spaced
// out by half a timestep, with velocity_type at t+t_step/2 (though it does allow phase classes to be passed to it). This method takes a single step,
// using the passed acceleration_type function. The passed function for this implementation must take in one parameter (the magnitude of distance_type from
// a centre point) and return one parameter (the magnitude of the acceleration_type toward this centre point).
template< typename f, typename Td=flt_type, typename Tv=flt_type, typename Tt=flt_type >
inline const int_type leapfrog_step( const Td & x, const Td & y,
								const Td & z, const Tv & vx, const Tv & vy,
								const Tv & vz,
								Td & new_x, Td & new_y, Td & new_z,
								Tv & new_vx, Tv & new_vy, Tv & new_vz,
								const Tt &t_step, const f *accel_func )
{
	static_assert(std::is_same<decltype(Td()/Tt()),decltype(Tv())>::value,"Invalid types passed to parameters of leapfrog_step");

	typedef decltype(Td()/(Tt()*Tt())) Ta;

	Td d;
	Ta a;

	d = 0;
	a = 0;

	// Adjust position
	new_x = x + vx * t_step;
	new_y = y + vy * t_step;
	new_z = z + vz * t_step;

	// Calculate acceleration_type at this new position
	d = dist3d( new_x, new_y, new_z );
	a = (*accel_func)( d );

	// Adjust velocities
	new_vx = vx + a * new_x / d * t_step;
	new_vy = vy + a * new_y / d * t_step;
	new_vz = vz + a * new_z / d * t_step;

	return 0;
}

template< typename f, typename Td=flt_type, typename Tv=flt_type, typename Tt=flt_type >
inline const int_type leapfrog_step( Td & x, Td & y, Td & z,
		Tv & vx, Tv & vy, Tv & vz,
		const Tt & t_step, const f *accel_func )
{
	static_assert(std::is_same<decltype(Td()/Tt()),decltype(Tv())>::value,"Invalid types passed to parameters of leapfrog_step");

	Td new_x, new_y, new_z;
	Tv new_vx, new_vy, new_vz;

	int_type result;
	result = leapfrog_step( x, y, z, vx, vy, vz, new_x, new_y, new_z, new_vx,
			new_vy, new_vz, t_step, accel_func );
	x = new_x;
	y = new_y;
	z = new_z;
	vx = new_vx;
	vy = new_vy;
	vz = new_vz;
	return result;
}

template< typename f, typename Tp, typename Tt=flt_type >
inline const int_type leapfrog_step( const Tp &p, Tp & new_p,
		const Tt & t_step, const f *accel_func )
{
	static_assert(std::is_same<decltype(p.x/Tt()),decltype(p.vx)>::value,"Invalid types passed to parameters of leapfrog_step");

	return leapfrog_step( p.x, p.y, p.z, p.vx, p.vy, p.vz, new_p.x, new_p.y,
			new_p.z, new_p.vx, new_p.vy, new_p.vz, t_step, accel_func );
}

template< typename f, typename Tp, typename Tt=flt_type >
inline const int_type leapfrog_step( Tp & p, const Tt & t_step,
		const f *accel_func )
{
	static_assert(std::is_same<decltype(p.x/Tt()),decltype(p.vx)>::value,"Invalid types passed to parameters of leapfrog_step");

	int_type result;
	Tp new_p(p);
	result = leapfrog_step( p, new_p, t_step, accel_func );
	p = new_p;
	return result;
}

}

#endif // _BRG_DE_HPP_INCLUDED_
