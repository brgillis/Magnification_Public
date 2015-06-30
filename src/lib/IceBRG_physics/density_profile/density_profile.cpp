/**********************************************************************\
  @file density_profile.cpp

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

#include "IceBRG_main/error_handling.h"
#include "IceBRG_main/math/calculus/integrate.hpp"
#include "IceBRG_main/math/solvers/solvers.hpp"
#include "IceBRG_main/units/units.hpp"
#include "IceBRG_physics/density_profile/density_profile_functors.h"

#include "density_profile.h"

// IceBRG::density_profile class methods
#if (1)
IceBRG::density_profile::~density_profile()
{
}

IceBRG::custom_unit_type<0,-2,0,0,0> IceBRG::density_profile::Daccel( const distance_type & r ) const
{
	distance_type dr;
	acceleration_type a1, a2;
	// It's simplest, and a ton faster to just manually differentiate here.
	dr = max( r * SMALL_FACTOR, units_cast<distance_type>(SMALL_FACTOR) );
	a1 = accel( r );
	a2 = accel( r + dr );
	return ( a2 - a1 ) / safe_d( dr );
}

IceBRG::distance_type IceBRG::density_profile::rhmtot() const
{
	// If cached, return the cached value
	if ( hmtot_cached )
		return _rhmtot_cache_;

	// Not cached, so we'll have to calculate it
	mass_type target_mass = hmtot();
	solve_rhm_functor func( this, target_mass );

	distance_type max_r( default_tau_factor * rvir() );
	distance_type rhm_test( 0 );

	// First check for zero mass_type/radius/density_type
	if ( ( value_of(mvir()) <= 0. ) || ( value_of(rvir()) <= 0. ) || (value_of( dens( rvir() / 2. ) ) < 0. ) )
	{
		hmtot_cached = true;
		return _rhmtot_cache_ = units_cast<distance_type>(0.);
	}

	try
	{
		rhm_test = solve_grid( func, units_cast<distance_type>(0.), max_r, 10, units_cast<mass_type>(0.) );
	}
	catch(const std::exception &e)
	{
		handle_error("Could not solve half-mass_type radius. Assuming it's zero.");

		_rhmtot_cache_ = units_cast<distance_type>(0.);
		hmtot_cached = true;
		return _rhmtot_cache_;
	}
	_rhmtot_cache_ = abs( rhm_test );
	hmtot_cached = true;
	return _rhmtot_cache_;
}

IceBRG::distance_type IceBRG::density_profile::rhmvir() const
{
	// If cached, return the cached value
	if ( hmvir_cached )
		return _rhmvir_cache_;

	// Not cached, so we'll have to calculate it
	mass_type target_mass = hmvir();
	solve_rhm_functor func( this, target_mass );

	distance_type max_r( default_tau_factor * rvir() );
	distance_type rhm_test( units_cast<distance_type>(0.) );

	// First check for zero mass_type/radius/density_type
	if ( ( value_of(mvir()) <= 0. ) || ( value_of(rvir()) <= 0 ) || ( value_of(dens( rvir() / 2. )) < 0 ) )
	{
		hmvir_cached = true;
		return _rhmvir_cache_ = units_cast<distance_type>(0.);
	}

	try
	{
		rhm_test = solve_grid( func, units_cast<distance_type>(0.), max_r, 10, units_cast<mass_type>(0.));
	}
	catch(const std::exception &e)
	{
		handle_error("Could not solve half-mass_type radius.");
		_rhmvir_cache_ = units_cast<distance_type>(0.);
		return _rhmvir_cache_;
	}
	_rhmvir_cache_ = max(units_cast<distance_type>(0.),abs( rhm_test ));
	hmvir_cached = true;
	return _rhmvir_cache_;
}

IceBRG::mass_type IceBRG::density_profile::enc_mass( const distance_type & r ) const
{
	if ( value_of(r) == 0 )
		return units_cast<mass_type>(0.);
	distance_type r_to_use = abs( r );
	IceBRG::spherical_density_functor func( this );
	distance_type min_in_params( units_cast<distance_type>(0.) ), max_in_params( r_to_use );
	mass_type out_params = IceBRG::integrate_Romberg( func,min_in_params,
			max_in_params, 0.00001, false );
	return out_params;
}

#endif

IceBRG::time_type IceBRG::period( const IceBRG::density_profile *host,
		const distance_type & r, const velocity_type & vr, const velocity_type & vt )
{
	auto mu = host->enc_mass( r ) * Gc;
	velocity_type v = quad_add( vr, vt );
	distance_type a = -mu / 2. / safe_d( v * v / 2. - mu / safe_d( r ) );
	time_type result = (
			value_of(a) > 0. ? 2. * pi * sqrt( cube(a) / mu ) : units_cast<time_type>( 0. ) );
	return result;
}
