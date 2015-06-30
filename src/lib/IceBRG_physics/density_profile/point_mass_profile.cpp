/**********************************************************************\
  @file point_mass_profile.cpp

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

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <vector>

#include "IceBRG_main/common.h"

#include "IceBRG_main/error_handling.h"
#include "IceBRG_main/utility.hpp"
#include "IceBRG_main/units/units.hpp"

#include "point_mass_profile.h"

// IceBRG::point_mass_profile class methods
#if (1)

#if (1) // Constructors
IceBRG::point_mass_profile::point_mass_profile()
{
	_mass_ = 0;
}

IceBRG::point_mass_profile::point_mass_profile( const mass_type init_mass,
		const flt_type & init_z )
{
	set_mvir( init_mass );
	set_z( init_z );
}

#endif // End constructors

// Destructor
IceBRG::point_mass_profile::~point_mass_profile()
{
}

#if (1) // Set functions

void IceBRG::point_mass_profile::set_mvir(
		const mass_type & new_halo_mass )
{
	_mass_ = new_halo_mass;
}
void IceBRG::point_mass_profile::set_parameters( const std::vector< any_units_type > & parameters )
{
	assert(parameters.size()==2);
	set_mvir( any_cast<mass_type>(parameters.at( 0 )) );
	set_z( any_cast<flt_type>(parameters.at( 1 )) );
}

#endif // end set functions

IceBRG::mass_type IceBRG::point_mass_profile::mvir() const
{
	return _mass_;
}
IceBRG::mass_type IceBRG::point_mass_profile::mass() const
{
	return _mass_;
}

IceBRG::mass_type IceBRG::point_mass_profile::mtot() const
{
	return _mass_;
}

IceBRG::velocity_type IceBRG::point_mass_profile::vvir() const
{
	return ipow<1,3>( 10. * Gc * H() * mvir() );
}
IceBRG::distance_type IceBRG::point_mass_profile::rvir() const
{
	return vvir() / H() / 10.;
}
IceBRG::distance_type IceBRG::point_mass_profile::rs() const
{
	return 0;
}
IceBRG::distance_type IceBRG::point_mass_profile::rt() const
{
	return units_cast<distance_type>(0);
}

IceBRG::density_type IceBRG::point_mass_profile::dens(
		const distance_type & r ) const
{
	density_type result = units_cast<density_type>(( value_of(r) == 0 ? std::numeric_limits<flt_type>::max() : 0 ));

	return result;
}
IceBRG::density_type IceBRG::point_mass_profile::enc_dens(
		const distance_type & r) const
{
	return enc_mass( r ) / ( 4. / 3. * pi * cube( r ) );
}
IceBRG::mass_type IceBRG::point_mass_profile::enc_mass(
		const distance_type & r) const
{
	return _mass_;
}

std::vector< IceBRG::any_units_type >
	IceBRG::point_mass_profile::get_parameters() const
{
	std::vector< any_units_type > parameters(num_parameters());

	parameters[0] = any_units_cast<any_units_type>(_mass_);
	parameters[1] = z();
	return parameters;
}

std::vector< std::string > IceBRG::point_mass_profile::get_parameter_names() const
{
	std::vector< std::string > parameter_names(num_parameters());

	parameter_names.at( 0 ) = "mass_type";
	parameter_names.at( 1 ) = "z";
	return parameter_names;
}

void IceBRG::point_mass_profile::truncate_to_fraction( const flt_type & f)
{
	if ( ( f <= 0 ) || ( isbad( f ) ) )
	{
		handle_error(std::string("Bad, negative, or zero f passed to truncate_to_fraction.\n")
					+ "Will truncate to zero instead.");
		_mass_ = 0;
	}
	else
	{
		_mass_ *= f;
	}

}

#endif // end point_mass profile functions
