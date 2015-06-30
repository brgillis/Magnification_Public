/**********************************************************************\
  @file density_profile.h

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

// body file: brg_physics/astro/density_profile/density_profile.cpp

#ifndef _BRG_DENSITY_PROFILE_H_
#define _BRG_DENSITY_PROFILE_H_

#include "IceBRG_main/units/units.hpp"
#include <stdexcept>
#include <vector>

#include "IceBRG_main/common.h"

#include "IceBRG_main/math/misc_math.hpp"
#include "IceBRG_main/math/safe_math.hpp"
#include "IceBRG_physics/astro.h"

namespace IceBRG {

#ifndef _BRG_VIRIAL_DENSITY_FACTOR_DEFFED_
#define _BRG_VIRIAL_DENSITY_FACTOR_DEFFED_
constexpr flt_type virial_density_factor = 200.;
#endif

class density_profile: public virtual redshift_obj
{
	/**********************************
	 density_profile class
	 -------------

	 An abstract class for anything which has
	 a mass_type density_type profile. Meant to be
	 overriden by child classes.

	 The methods for this class are sorted
	 into four groups:
	 -Pure virtual functions (must be implemented
	 by any derived classes)
	 -Virtual functions which must be overwritten if they're going
	 to be used (not pure virtual since they may not be needed)
	 -Virtual functions which should be overwritten if possible and
	 if they're going to be used (those that require integration,
	 but may have an analytic solution for certain profiles)
	 -Virtual functions that don't need to be overwritten in most
	 cases and non-virtual functions non-virtual functions

	 Derived classes:
	 tNFW_profile
	 point_mass profile

	 **********************************/

private:
#if (1)
	char _hm_type_;

	mutable distance_type _rhmvir_cache_, _rhmtot_cache_;

	velocity_type _v_from_r( distance_type r ) const
	{
		acceleration_type a = accel( fabs( r ) );
		if ( a >= acceleration_type(0) )
			return 0;
		return sqrt( -a * fabs( r ) );
	}
#endif

protected:
	mutable bool hmvir_cached, hmtot_cached;

public:
#if (1)

#if (1) // Constructor
	density_profile()
	{
		_hm_type_ = 1;
		_rhmvir_cache_ = 0;
		hmvir_cached = false;
		_rhmtot_cache_ = 0;
		hmtot_cached = false;
	}
#endif

#if (1) // Destructor
	virtual ~density_profile();
#endif

#if (1) // Pure virtual functions (must be implemented by any derived classes)

	virtual mass_type mvir() const =0; // Virial mass_type (exact definition can be chosen per profile)
	virtual density_type dens( const distance_type & r ) const =0; // Local density_type at radius r

	virtual density_profile *density_profile_clone() const =0; // Creates a clone of this
#endif

#if (1) // Virtual functions which must be overwritten if they're going to be used

#if (1) // Set functions - will return 1 if profile doesn't support this method of setting
	// All take values in default unit set (m, s, kg, K, rad, C)
	virtual void set_mvir( const mass_type & new_mvir )
	{
		throw std::logic_error("density_profile::set_mvir(...) must be overloaded to be used.\n");
	}
	virtual void set_vvir( const velocity_type & new_vvir )
	{
		throw std::logic_error("density_profile::set_vvir(...) must be overloaded to be used.\n");
	}
	virtual void set_rvir( const distance_type & new_rvir )
	{
		throw std::logic_error("density_profile::set_rvir(...) must be overloaded to be used.\n");
	}
	virtual void set_rs( const distance_type & new_rs ) // Scale radius
	{
		throw std::logic_error("density_profile::set_rs(...) must be overloaded to be used.\n");
	}
	virtual void set_rt( const distance_type & new_rt ) // Tidal/truncation radius
	{
		throw std::logic_error("density_profile::set_rt(...) must be overloaded to be used.\n");
	}
	virtual void set_parameters( const std::vector< any_units_type > &parameters )
	{
		throw std::logic_error("density_profile::set_parameters(...) must be overloaded to be used.\n");
	}
	virtual void set_tau( const flt_type & new_tau ) // Truncation parameter
	{
		throw std::logic_error("density_profile::set_tau(...) must be overloaded to be used.\n");
	}
	virtual void set_c( const flt_type & new_c ) // Concentration
	{
		throw std::logic_error("density_profile::set_c(...) must be overloaded to be used.\n");
	}
	void override_rhmvir( const distance_type & new_rhmvir )
	{
		_rhmvir_cache_ = new_rhmvir;
		hmvir_cached = true;
	}
	void override_rhmtot( const distance_type & new_rhmtot )
	{
		_rhmtot_cache_ = new_rhmtot;
		hmtot_cached = true;
	}

#endif

#if (1) // Basic get functions
	// All return values in default unit set (m, s, kg, K, rad, C)

	virtual mass_type mtot() const // Total mass_type
	{
		return 0;
	}

	virtual distance_type rt() const // Tidal/truncation radius
	{
		throw std::logic_error("density_profile::rt() must be overloaded to be used.\n");
	}

#endif // end basic get functions

	virtual size_t num_parameters() const
	{
		throw std::logic_error("density_profile::num_parameters() must be overloaded to be used.\n");
	}

	virtual std::vector< any_units_type > get_parameters() const // Returns a set of parameters for this halo (all the variables needed to define it - must be defined for each child)
	{
		throw std::logic_error("density_profile::get_parameters() must be overloaded to be used.\n");
	}

	virtual std::vector< std::string > get_parameter_names() const // Returns a set of names of this halo's parameters
	{
		throw std::logic_error("density_profile::get_parameter_names() must be overloaded to be used.\n");
	}

#endif // end Virtual functions which must be overwritten if they're going to be used

#if (1) // Virtual functions which should be overwritten if at all possible and if they'll be used
	virtual mass_type enc_mass( const distance_type & r ) const; // Mass enclosed with sphere of radius r
#endif

#if (1) // Virtual functions which shouldn't be overwritten in most cases and non-virtual functions

	virtual distance_type rvir() const // Virial radius (exact definition can be chosen per profile if preferred)
	{

		return ipow<1,3>( 2. * mvir() * Gc / ( square( H() ) * virial_density_factor ));
	}
	mass_type hmvir() const // Half virial mass
	{
		return mvir() / 2.;
	}
	mass_type hmtot() const // Half total mass
	{
		return mtot() / 2.;
	}
	virtual mass_type hm() const // Half mass (which depends on hm_type value)
	{
		return ( _hm_type_ == 0 ? hmvir() : hmtot() );
	}
	virtual density_type enc_dens( const distance_type & r ) const // Mean density enclosed with sphere of radius r
	{
		distance_type r_to_use = safe_d( abs(r) );
		return enc_mass( r_to_use )
				/ ( 4. / 3. * pi * cube( fabs( r_to_use ) ) );
	}
	virtual distance_type rhmvir() const; // Half-virial-mass radius
	virtual distance_type rhmtot() const; // Half-total-mass radius
	virtual distance_type rhm() const // Half-mass radius
	{
		return ( _hm_type_ == 0 ? rhmvir() : rhmtot() );
	}
	virtual velocity_type vvir() const // Orbital velocity at rvir
	{
		return _v_from_r( rvir() );
	}
	virtual velocity_type vhmvir() const // Orbital velocity at rhmvir
	{
		return _v_from_r( rhmvir() );
	}
	virtual velocity_type vhmtot() const // Orbital velocity at rhmtot
	{
		return _v_from_r( rhmtot() );
	}
	virtual velocity_type vhm() const // Orbital velocity at rhm
	{
		return ( _hm_type_ == 0 ? vhmvir() : vhmtot() );
	}
	virtual velocity_type vt() const // Orbital velocity at rt
	{
		return _v_from_r( rt() );
	}
	time_type otvir() const // Orbital period at rvir
	{
		return 2. * pi * rvir() / vvir();
	}
	time_type othmvir() const // Orbital period at rhmvir
	{
		return 2. * pi * rhmvir() / safe_d(vhmvir());
	}
	time_type othmtot() const // Orbital period at rhmtot
	{
		return 2. * pi * rhmtot() / safe_d(vhmtot());
	}
	time_type othm() const // Orbital period at rhm
	{
		return 2. * pi * rhm() / safe_d(vhm());
	}
	time_type ott() const // Orbital period at rt
	{
		return 2. * pi * rt() / safe_d(vt());
	}

	void set_hm_type( int_type new_hm_type ) // Whether default half-mass is half virial, half total, or something else
	{
		_hm_type_ = new_hm_type;
	}

#if (1) // advanced get functions

	acceleration_type accel( const distance_type & r) const // Gravitational acceleration at radius r
	{
		if(value_of(r)==0.)
			return 0;
		else
			return -Gc * enc_mass( r ) / square( r );
	}
	virtual custom_unit_type<0,-2,0,0,0> Daccel( const distance_type & r ) const; // Derivative of acceleration at radius r

#endif // end advanced get functions

#endif // end Virtual functions which shouldn't be overwritten in most cases and non-virtual functions

#if (1) // Other operations

	virtual void truncate_to_fraction( const flt_type & fraction ) // Adjusts parameters of this class to decrease the mass to fraction of its previous mass.
	{
		throw std::logic_error("density_profile::truncate_to_fraction() must be overloaded to be used.\n");
	}

#endif

#endif
}; // end class density_profile

// Function to estimate orbital period from current position and velocity_type in a density_type profile
// Note that this is merely an estimate from analogy to calculations in a Keplerian potential
time_type period( const density_profile *host, const distance_type & r,
		const velocity_type & vr, const velocity_type & vt = 0 );

} // end namespace IceBRG

#endif /* _BRG_DENSITY_PROFILE_H_ */
