/**********************************************************************\
  @file density_profile_functors.cpp

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

#include <iostream>

#include "IceBRG_main/common.h"

#include "IceBRG_main/utility.hpp"

#include "density_profile_functors.h"
#include "IceBRG_main/units/units.hpp"

// IceBRG::accel_functor class methods
#if (1)

void IceBRG::accel_functor::set_host_ptr(
		const density_profile *new_host )
{
	_host_ptr_ = new_host;
}

IceBRG::acceleration_type IceBRG::accel_functor::operator()( const distance_type &  in_param ) const
{
	if ( _host_ptr_ == NULL )
	{
		throw std::runtime_error("ERROR: Host must be assigned to accel_functor before function can be called.\n");
	}
	return _host_ptr_->accel( in_param );
}

IceBRG::accel_functor::accel_functor()
{
	_host_ptr_ = NULL;
}
IceBRG::accel_functor::accel_functor( const density_profile *init_host )
{
	set_host_ptr( init_host );
}

#endif // end IceBRG::accel_functor function implementations

// IceBRG::spherical_density_functor class methods
#if (1)

void IceBRG::spherical_density_functor::set_host_ptr(
		const density_profile *new_host )
{
	_host_ptr_ = new_host;
}

IceBRG::custom_unit_type<-1,0,1,0,0> IceBRG::spherical_density_functor::operator()(
		const distance_type &  in_param ) const
{
	if ( _host_ptr_ == NULL )
	{
		throw std::runtime_error("ERROR: Host must be assigned to spherical_density_functor before function can be called.\n");
	}
	return 4 * pi * square( in_param )
			* _host_ptr_->dens( in_param );
}

IceBRG::spherical_density_functor::spherical_density_functor()
{
	_host_ptr_ = NULL;
}
IceBRG::spherical_density_functor::spherical_density_functor(
		const density_profile *init_host )
{
	set_host_ptr( init_host );
}

#endif // end IceBRG::spherical_density_functor class methods

// IceBRG::solve_rhm_functor class methods
#if (1)
void IceBRG::solve_rhm_functor::set_host_ptr(
		const density_profile *new_host )
{
	_host_ptr_ = new_host;
}

void IceBRG::solve_rhm_functor::set_target_mass(
		const mass_type &new_target_mass )
{
	_target_mass_ = new_target_mass;
}

IceBRG::mass_type IceBRG::solve_rhm_functor::operator()( const distance_type & in_param ) const
{
	if ( _host_ptr_ == NULL )
	{
		throw std::runtime_error("ERROR: Host must be assigned to solve_rhm_functor before function can be called.\n");
	}
	return abs(_target_mass_ - _host_ptr_->enc_mass( abs( in_param ) ) );
}

IceBRG::solve_rhm_functor::solve_rhm_functor()
{
	_host_ptr_ = NULL;
	_target_mass_ = 0;
}
;
IceBRG::solve_rhm_functor::solve_rhm_functor(
		const density_profile *init_host, const mass_type &new_target_mass )
{
	set_host_ptr( init_host );
	set_target_mass( new_target_mass );
}
#endif // end IceBRG::solve_rhm_functor function implementations
