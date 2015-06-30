/**********************************************************************\
  @file density_profile_functors.h

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

// body file: brg_physics/astro/density_profile/density_profile_functors.cpp

#ifndef _BRG_DENSITY_PROFILE_FUNCTORS_H_
#define _BRG_DENSITY_PROFILE_FUNCTORS_H_

#include "IceBRG_main/units/units.hpp"
#include "IceBRG_main/common.h"

#include "IceBRG_physics/density_profile/density_profile.h"

namespace IceBRG {

class accel_functor
{
	/**********************************
	 accel_functor class
	 -----------------------------

	 Function class for acceleration_type within a density_type profile

	 Parent class: function_class (from brg_functors)

	 **********************************/
private:
	const density_profile *_host_ptr_;
public:

	void set_host_ptr( const density_profile *new_host_ptr );
	const density_profile * host_ptr()
	{
		return _host_ptr_;
	}

	acceleration_type operator()( const distance_type &  in_param ) const;

	accel_functor();
	accel_functor( const density_profile *init_host_ptr );
	virtual ~accel_functor()
	{
	}
};
// class accel_functor

class solve_rhm_functor
{
	/**********************************
	 solve_rhm_functor class
	 -----------------------------

	 Function class for solving the half-mass_type
	 radius of a halo.

	 Parent class: function_class (from brg_functors)

	 **********************************/
private:
	const density_profile *_host_ptr_;mass_type _target_mass_;

public:

	void set_host_ptr( const density_profile *new_host_ptr );
	const density_profile * host_ptr()
	{
		return _host_ptr_;
	}
	void set_target_mass( const mass_type &new_target_mass );
	const mass_type & target_mass()
	{
		return _target_mass_;
	}

	mass_type operator ()( const distance_type & in_param ) const;

	solve_rhm_functor();
	solve_rhm_functor( const density_profile *init_host,
			const mass_type &init_target_mass );

};
// end class unitless_solve_rhm_functor

class spherical_density_functor
{
	/**********************************
	 spherical_density_functor class
	 -----------------------------

	 Function class integrating density_type in a sphere

	 Parent class: function_class (from brg_functors)

	 **********************************/
private:
	const density_profile *_host_ptr_;

public:

	void set_host_ptr( const density_profile *new_host_ptr );
	const density_profile * host_ptr()
	{
		return _host_ptr_;
	}

	custom_unit_type<-1,0,1,0,0> operator()( const distance_type & in_param ) const;

	spherical_density_functor();
	spherical_density_functor( const density_profile *init_host );
	virtual ~spherical_density_functor()
	{
	}
};

} // namespace IceBRG

#endif /* _BRG_DENSITY_PROFILE_FUNCTORS_H_ */
