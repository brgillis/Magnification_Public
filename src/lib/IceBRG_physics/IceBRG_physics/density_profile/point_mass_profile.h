/**********************************************************************\
  @file point_mass_profile.h

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

// body file: brg_physics/astro/density_profile/point_mass_profile.cpp

#ifndef _BRG_POINT_MASS_PROFILE_H_
#define _BRG_POINT_MASS_PROFILE_H_

#include <vector>

#include "IceBRG_main/common.h"

#include "IceBRG_physics/density_profile/density_profile.h"
#include "IceBRG_main/units/units.hpp"

namespace IceBRG {

class point_mass_profile: public density_profile
{
	/**********************************
	 point_mass_profile class
	 ------------------------

	 A virtual class for a point mass_type

	 Defined by two parameters:
	 mass_type and z

	 Parent class:
	 density_type profile

	 Derived classes:
	 (none)

	 **********************************/
private:
#if (1) // private member variables specific to this class

	mass_type _mass_;

#endif // end private member variables

public:
#if (1) // public variables and functions

#if (1) // Constructors
	point_mass_profile();

	point_mass_profile( const mass_type init_mass, const flt_type & init_z );

#endif // End constructors

	// Destructor
	~point_mass_profile();

#if (1) // Set functions
	virtual void set_mvir( const mass_type & new_halo_mass );
	virtual void set_parameters( const std::vector< any_units_type > &new_parameters );
#endif // End set functions

#if (1) // Basic get functions
	mass_type mass() const;

	mass_type mvir() const;
	mass_type mtot() const;

	distance_type rvir() const;
	distance_type rt() const;
	distance_type rs() const;

	velocity_type vvir() const;

#endif // end basic get functions

#if (1) // advanced get functions
	density_type dens( const distance_type & r ) const;
	density_type enc_dens( const distance_type & r) const;
	mass_type enc_mass( const distance_type & r ) const; // Mass enclosed with sphere of radius r
	size_t num_parameters() const
	{
		return 2; // Mass and redshift
	}
	std::vector< any_units_type > get_parameters() const;

	std::vector< std::string > get_parameter_names() const;
#endif // end advanced get functions

#if (1) // Other operations

	void truncate_to_fraction( const flt_type & fraction);
	virtual redshift_obj *redshift_obj_clone() const
	{
		return new point_mass_profile( *this );
	}
	virtual density_profile *density_profile_clone() const
	{
		return new point_mass_profile( *this );
	}
	virtual point_mass_profile *point_mass_profile_clone() const
	{
		return new point_mass_profile( *this );
	}

#endif

#endif // end public variables and functions
};
// class point_mass_profile

} // end namespace IceBRG

#endif /* _BRG_POINT_MASS_PROFILE_H_ */
