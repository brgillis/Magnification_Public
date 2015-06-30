/**********************************************************************\
  @file lensing_profile_extension.hpp

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

#ifndef _BRG_LENSING_PROFILE_EXTENSION_HPP_
#define _BRG_LENSING_PROFILE_EXTENSION_HPP_

#include "IceBRG_main/common.h"

#include "IceBRG_main/math/interpolator/interpolator.h"
#include "IceBRG_main/units/units.hpp"

#include "IceBRG_lensing/lensing_profile_extension_functors.hpp"
#include "IceBRG_lensing/shifting/shifting_cache.h"
#include "IceBRG_lensing/two_halo_term_functions.hpp"

#include "IceBRG_physics/density_profile/density_profile.h"

// TODO: Update lensing tNFW_profile along with updates here

// Macro definitions

// SPCP: "Static Polymorphic Const Pointer"
#define SPCP(name) static_cast<const name*>(this)

namespace IceBRG {

constexpr flt_type delta_c = 1.686;

template<typename name>
class lensing_profile_extension {
private:

// Calculation functions
#if(1)

// Basic calculation functions which should be overridden if possible
#if(1)

	const surface_density_type _proj_dens( const distance_type & R ) const // Projected surface density_type at radius R
	{
		distance_type R_to_use = abs( R );
		flt_type inf_factor = 20;
		IceBRG::projected_density_functor<name> func( SPCP(name), R_to_use );
		distance_type min_in_param( units_cast<distance_type>(0.) ), max_in_param( inf_factor * SPCP(name)->rvir() );
		surface_density_type out_param( units_cast<mass_type>(0.) );

		if ( value_of(R_to_use) == 0 )
		{
			// In this case, we might be integrating over a singularity, so the trapezoid method is safer
			const int_type num_steps = 10000;

			out_param = IceBRG::integrate_trapezoid( func, min_in_param,
					max_in_param, num_steps );
		}
		else
		{
			out_param = IceBRG::integrate_Romberg( func, min_in_param,
					max_in_param, 0.00001, false );
		}

		return 2. * out_param;
	}

	const mass_type _proj_enc_mass( const distance_type & R ) const // Mass enclosed within a cylinder of radius R
	{
		if ( R == units_cast<distance_type>(0.) )
			return units_cast<mass_type>(0.);
		distance_type R_to_use = abs( R );
		IceBRG::cylindrical_density_functor<name> func( SPCP(name) );
		distance_type min_in_param( units_cast<distance_type>(0.) ), max_in_param( R_to_use );
		mass_type out_param( units_cast<mass_type>(0.) );

		out_param = IceBRG::integrate_Romberg( func, min_in_param,
				max_in_param, 0.00001, false );

		return out_param;
	}

#endif

// Advanced calculations which usually can't be overridden, but should be if possible
#if(1)

	surface_density_type _offset_Delta_Sigma( const distance_type & R,
			const distance_type & offset_R ) const // Expected weak lensing signal in tangential shear Delta-Sigma at radius R from position offset by offset_R
	{
		if ( value_of(offset_R) == 0. )
			return SPCP(name)->Delta_Sigma( R );
		distance_type R_to_use = abs( R );
		distance_type offset_R_to_use = abs( offset_R );
		offset_ring_dens_functor<name> ringfunc( SPCP(name), offset_R_to_use, R_to_use );
		offset_circ_dens_functor<name> circfunc( SPCP(name), offset_R_to_use, R_to_use );

		surface_density_type circmean;
		surface_density_type ringmean;
		surface_density_type result;

		flt_type precision = 0.001;

		angle_type min_in_param_ring(0. * rad);
		angle_type max_in_param_ring(pi * rad);

		auto out_param_ring = IceBRG::integrate_Romberg( ringfunc, min_in_param_ring,
				max_in_param_ring, precision, false );

		ringmean = out_param_ring / (pi * rad);

		distance_type min_in_param_circ = max(offset_R_to_use-R_to_use,0.*m);
		distance_type max_in_param_circ = offset_R_to_use+R_to_use;

		auto out_param_circ = IceBRG::integrate_Romberg( circfunc, min_in_param_circ,
				max_in_param_circ, precision, false );

		circmean = out_param_circ / ( pi * square(R_to_use) );

		result = circmean - ringmean;

		return result;
	}
	surface_density_type _group_Delta_Sigma( const distance_type & R,
			const flt_type & group_c ) const // Expected weak lensing signal in tangential shear Delta-Sigma at radius R from ensemble of satellites in group with satellite concentration group_c
	{
		distance_type R_to_use = abs( R );
		IceBRG::offset_Delta_Sigma_functor<name> func( SPCP(name), R_to_use );
		IceBRG::group_Delta_Sigma_weight_functor<name> weight_func( SPCP(name), group_c );
		distance_type min_in_param( units_cast<distance_type>(units_cast<distance_type>(SMALL_FACTOR)) ),
				max_in_param( 2.5*SPCP(name)->rvir() );

		surface_density_type out_param = IceBRG::integrate_weighted_Romberg( func, weight_func,
				min_in_param, max_in_param, 0.00001, false);

		return out_param;
	}
	surface_density_type _semiquick_group_Delta_Sigma( const distance_type & R,
			const flt_type & group_c ) const // As _group_Delta_Sigma, but uses offset_Delta_sigma cache to speed it up if overwritten
	{
		distance_type R_to_use = abs( R );
		IceBRG::quick_offset_Delta_Sigma_functor<name> func( SPCP(name), R_to_use );
		IceBRG::group_Delta_Sigma_weight_functor<name> weight_func( SPCP(name), group_c );
		distance_type min_in_param( units_cast<distance_type>(SMALL_FACTOR) ),
				max_in_param( 2.5*SPCP(name)->rvir() );

		surface_density_type out_param = IceBRG::integrate_weighted_Romberg( func, weight_func,
				min_in_param, max_in_param, 0.00001, false);
		return out_param;
	}
	surface_density_type _two_halo_Delta_Sigma( const distance_type & R,
			const flt_type & group_c ) const
	{
		assert(false); // TODO: Fill this out
		return surface_density_type();
	}

	surface_density_type _offset_Sigma( const distance_type & R,
			const distance_type & offset_R ) const // Expected Sigma at radius R from position offset by offset_R
	{
		if ( value_of(offset_R) == 0. )
			return SPCP(name)->proj_dens( R );
		distance_type R_to_use = abs( R );
		distance_type offset_R_to_use = abs( offset_R );
		offset_ring_dens_functor<name> ringfunc( SPCP(name), offset_R_to_use, R_to_use );

		flt_type precision = 0.001;

		angle_type min_in_param_ring = 0 * rad;
		angle_type max_in_param_ring = pi * rad;

		auto out_param_ring = IceBRG::integrate_Romberg( ringfunc, min_in_param_ring,
				max_in_param_ring, precision, false );

		surface_density_type ringmean = out_param_ring / (pi*rad);

		return ringmean;
	}
	surface_density_type _group_Sigma( const distance_type & R,
			const flt_type & group_c ) const // Expected Sigma at radius R from ensemble of satellites in group with satellite concentration group_c
	{
		distance_type R_to_use = abs( R );
		auto func = [=, &R_to_use] (const distance_type & offset_R)
		{
			return SPCP(name)->offset_Sigma( R_to_use, offset_R );
		};
		auto weight_func = [=, &group_c] (const distance_type & R)
		{
			if ( group_c == 0 )
			{
				return 2. * pi * R * SPCP(name)->proj_dens( R );
			}
			else
			{
				std::unique_ptr<name> group_profile(SPCP(name)->lensing_profile_extension_clone());
				group_profile->set_c(group_c);
				group_profile->set_tau(group_profile->tau()*group_c/SPCP(name)->c());
				return 2. * pi * R * group_profile->proj_dens(R);
			}
		};

		distance_type min_in_param( units_cast<distance_type>(SMALL_FACTOR) ),
				max_in_param( 2.5*SPCP(name)->rvir() );

		surface_density_type out_param = IceBRG::integrate_weighted_Romberg( func, weight_func,
				min_in_param, max_in_param, 0.00001, false);

		return out_param;
	}
	surface_density_type _semiquick_group_Sigma( const distance_type & R,
			const flt_type & group_c ) const // As group_Delta_Sigma, but uses offset_Delta_Sigma cache to speed it up if overwritten
	{
		distance_type R_to_use = abs( R );
		auto func = [=, &R] (const distance_type & offset_R )
		{
			return SPCP(name)->quick_offset_Sigma( R_to_use, offset_R );
		};
		auto weight_func = [=, &group_c] (const distance_type & R)
		{
			if ( group_c == 0 )
			{
				return value_of(2. * pi * R * SPCP(name)->proj_dens( R ));
			}
			else
			{
				std::unique_ptr<name> group_profile(SPCP(name)->lensing_profile_extension_clone());
				group_profile->set_c(group_c);
				group_profile->set_tau(group_profile->tau()*group_c/SPCP(name)->c());
				return value_of(2. * pi * R * group_profile->proj_dens(R));
			}
		};
		distance_type min_in_param( units_cast<distance_type>(SMALL_FACTOR) ), max_in_param( 2.5*SPCP(name)->rvir() );

		surface_density_type out_param = IceBRG::integrate_weighted_Romberg( func, weight_func,
				min_in_param, max_in_param, 0.00001, false);

		return out_param;
	}
	surface_density_type _two_halo_Sigma( const distance_type & R,
			const flt_type & group_c ) const
	{
		assert(false); // TODO: Fill this out

		return surface_density_type();
	}

	surface_density_type _shifted_Delta_Sigma( const distance_type & R ) const
	{
		distance_type R_to_use = abs( R );
		distance_type sigma = SPCP(name)->_shift_sigma(R_to_use);

		IceBRG::shifted_Delta_Sigma_functor<name> func( SPCP(name), R_to_use );
		IceBRG::shifted_Delta_Sigma_weight_functor<name> weight_func( sigma );

		flt_type precision = 0.00001;

		distance_type min_in_param( units_cast<distance_type>(0.) ), max_in_param( 4.*sigma );

		surface_density_type out_param = IceBRG::integrate_weighted_Romberg( func, weight_func,
				min_in_param, max_in_param, precision, false );

		return out_param;
	}

	surface_density_type _semiquick_shifted_Delta_Sigma( const distance_type & R ) const
	{
		distance_type R_to_use = abs( R );
		distance_type sigma = SPCP(name)->_shift_sigma(R_to_use);

		IceBRG::shifted_Delta_Sigma_functor<name> func( SPCP(name), R_to_use );
		IceBRG::shifted_Delta_Sigma_weight_functor<name> weight_func( sigma );

		distance_type min_in_param( units_cast<distance_type>(0.) ), max_in_param( 4.*sigma );

		surface_density_type out_param = IceBRG::integrate_weighted_Romberg( func, weight_func,
				min_in_param, max_in_param, 0.00001, false);
		return out_param;
	}

#endif // Advanced calculations which usually can't be overridden, but should be if possible

// Quick functions - should be overridden if a cache is implemented for the halo
#if(1)

	surface_density_type _quick_Delta_Sigma( const distance_type & R ) const
	{
		return SPCP(name)->_Delta_Sigma(R);
	}
	surface_density_type _quick_offset_Delta_Sigma( const distance_type & R,
			const distance_type & offset_R ) const
	{
		return SPCP(name)->_offset_Delta_Sigma( R, offset_R );
	}
	surface_density_type _quick_group_Delta_Sigma( const distance_type & R,
			const flt_type & group_c ) const
	{
		return SPCP(name)->_group_Delta_Sigma( R, group_c );
	}
	surface_density_type _quick_two_halo_Delta_Sigma( const distance_type & R ) const
	{
		return SPCP(name)->_two_halo_Delta_Sigma(R);
	}
	surface_density_type _quick_Sigma( const distance_type & R ) const
	{
		return SPCP(name)->_Sigma(R);
	}
	surface_density_type _quick_offset_Sigma( const distance_type & R,
			const distance_type & offset_R ) const // As offset_Sigma, but uses cache to speed it up if overwritten
	{
		return SPCP(name)->_offset_Sigma( R, offset_R );
	}
	surface_density_type _quick_group_Sigma( const distance_type & R,
			const flt_type & group_c ) const // As Sigma, but uses group_Delta_Sigma cache to speed it up if overwritten
	{
		return SPCP(name)->_group_Sigma( R, group_c );
	}
	surface_density_type _quick_two_halo_Sigma( const distance_type & R ) const
	{
		return SPCP(name)->_two_halo_Sigma(R);
	}
	surface_density_type _quick_shifted_Delta_Sigma( const distance_type & R ) const
	{
		return SPCP(name)->_shifted_Delta_Sigma( R );
	}

#endif // Quick functions - should be overridden if a cache is implemented for the halo

// Simple calculation functions that should rarely need to be overridden
#if (1)
	surface_density_type _proj_enc_dens( const distance_type & R ) const // Mean surface density_type enclosed within a cylinder of radius R
	{
		distance_type R_to_use = max( abs( R ), units_cast<distance_type>(units_cast<distance_type>(SMALL_FACTOR)) );
		return SPCP(name)->proj_enc_mass( R_to_use )
				/ ( pi * square( R_to_use ) );
	}

	distance_type _shift_sigma( const distance_type & R ) const
	{
		return R*shift_factor(R);
	}
	flt_type _shift_factor( const distance_type & R ) const
	{
		if(value_of(R)==0) return 0.;
		const angle_type theta_separation = afd(R,SPCP(name)->z());
		return any_cast<angle_type>(shifting_cache().get(theta_separation,SPCP(name)->z()))
				/ theta_separation;
	}
	surface_density_type _Delta_Sigma( const distance_type & R ) const // Weak lensing signal in tangential shear Delta-Sigma at radius R
	{
		return SPCP(name)->proj_enc_dens( R ) - SPCP(name)->proj_dens( R );
	}
#endif // Simple calculation functions that should rarely need to be overridden

#endif // Calculation functions

public:

	// Constructors and destructors
#if (1)
	lensing_profile_extension()
	{
	}
	~lensing_profile_extension()
	{
	}
#endif

	// Virtual functions that will need to be implemented by base classes
#if (1)

	// Clone function for this
	name* lensing_profile_extension_clone() const
	{
		return new name(*SPCP(name));
	}

#endif // Virtual functions that will need to be implemented by base classes

	// Projected mass_type and density_type functions
#if (1)

	// These ones should be overridden if at all possible, as otherwise they'll have to be
	// integrated
#if (1)
	surface_density_type proj_dens( const distance_type & R ) const // Projected surface density_type at radius R
	{
		return SPCP(name)->_proj_dens(R);
	}
	flt_type proj_enc_mass( const distance_type & R ) const // Mass enclosed within a cylinder of radius R
	{
		return SPCP(name)->_proj_enc_mass(R);
	}
#endif

	// These ones typically don't need to be overridden, but they can be if it's convenient
#if (1)
	surface_density_type proj_enc_dens( const distance_type & R ) const // Mean surface density_type enclosed within a cylinder of radius R
	{
		return SPCP(name)->_proj_enc_dens(R);
	}
#endif

#endif // Projected mass_type and density_type functions

	// Weak lensing functions
#if (1)

	// Delta Sigma
#if (1)
	surface_density_type Delta_Sigma( const distance_type & R ) const // Weak lensing signal in tangential shear Delta-Sigma at radius R
	{
		return SPCP(name)->_Delta_Sigma(R);
	}
	surface_density_type quick_Delta_Sigma( const distance_type & R ) const // As deltasigma, but uses cache to speed it up if overwritten
	{
		return SPCP(name)->_quick_Delta_Sigma( R );
	}
#endif

	// Offset Delta Sigma
#if (1)
	surface_density_type offset_Delta_Sigma( const distance_type & R,
			const distance_type & offset_R ) const // Expected weak lensing signal in tangential shear Delta-Sigma at radius R from position offset by offset_R
	{
		return SPCP(name)->_offset_Delta_Sigma( R, offset_R );
	}
	surface_density_type quick_offset_Delta_Sigma( const distance_type & R,
			const distance_type & offset_R ) const // As offset_Delta_Sigma, but uses cache to speed it up if overwritten
	{
		return SPCP(name)->_quick_offset_Delta_Sigma( R, offset_R );
	}
#endif

	// Group Delta Sigma
#if (1)
	surface_density_type group_Delta_Sigma( const distance_type & R,
			const flt_type & group_c=2.5 ) const // Expected weak lensing signal in tangential shear Delta-Sigma at radius R from ensemble of satellites in group with satellite concentration group_c
	{
		return SPCP(name)->_group_Delta_Sigma( R, group_c );
	}
	surface_density_type semiquick_group_Delta_Sigma( const distance_type & R,
			const flt_type & group_c=2.5 ) const // As group_Delta_Sigma, but uses offset_Delta_Sigma cache to speed it up if overwritten
	{
		return SPCP(name)->_semiquick_group_Delta_Sigma( R, group_c );
	}
	surface_density_type quick_group_Delta_Sigma( const distance_type & R,
			const flt_type & group_c=2.5 ) const // As deltasigma, but uses group_Delta_Sigma cache to speed it up if overwritten
	{
		return SPCP(name)->_quick_group_Delta_Sigma( R, group_c );
	}
#endif

	// Two Halo Delta Sigma
#if (1)
	surface_density_type two_halo_Delta_Sigma( const distance_type & R ) const // Expected weak lensing signal in tangential shear Delta-Sigma at radius R from ensemble of satellites in group with satellite concentration group_c
	{
		return SPCP(name)->_two_halo_Delta_Sigma( R );
	}
	surface_density_type quick_two_halo_Delta_Sigma( const distance_type & R ) const // As deltasigma, but uses group_Delta_Sigma cache to speed it up if overwritten
	{
		return SPCP(name)->_quick_two_halo_Delta_Sigma( R );
	}
#endif

	// Shifted Delta Sigma
#if (1)
	// This represents the weak-lensing signal after being corrected for errors due to relative
	// shifting of the lens and source due to an intervening mass_type distribution

	flt_type shift_factor( const distance_type & R ) const
	{
		return SPCP(name)->_shift_factor( R );
	}

	surface_density_type shifted_Delta_Sigma( const distance_type & R ) const
	{
		return SPCP(name)->_shifted_Delta_Sigma( R );
	}
	surface_density_type semiquick_shifted_Delta_Sigma( const distance_type & R ) const
	{
		return SPCP(name)->_semiquick_shifted_Delta_Sigma( R );
	}
	surface_density_type quick_shifted_Delta_Sigma( const distance_type & R ) const
	{
		return SPCP(name)->_quick_shifted_Delta_Sigma( R );
	}
#endif

	// Sigma
#if (1)
	surface_density_type Sigma( const distance_type & R ) const // Magnification signal Sigma at radius R
	{
		return SPCP(name)->_proj_dens(R);
	}
	surface_density_type quick_Sigma( const distance_type & R ) const // As Sigma, but uses cache to speed it up if overridden
	{
		return SPCP(name)->_quick_Sigma( R );
	}
#endif

	// Offset Sigma
#if (1)
	surface_density_type offset_Sigma( const distance_type & R,
			const distance_type & offset_R ) const // Expected weak lensing signal in tangential shear Delta-Sigma at radius R from position offset by offset_R
	{
		return SPCP(name)->_offset_Sigma( R, offset_R );
	}
	surface_density_type quick_offset_Sigma( const distance_type & R,
			const distance_type & offset_R ) const // As offset_Delta_Sigma, but uses cache to speed it up if overwritten
	{
		return SPCP(name)->_quick_offset_Sigma( R, offset_R );
	}
#endif

	// Group Sigma
#if (1)
	surface_density_type group_Sigma( const distance_type & R,
			const flt_type & group_c=2.5 ) const // Expected weak lensing signal in tangential shear Delta-Sigma at radius R from ensemble of satellites in group with satellite concentration group_c
	{
		return SPCP(name)->_group_Sigma( R, group_c );
	}
	surface_density_type semiquick_group_Sigma( const distance_type & R,
			const flt_type & group_c=2.5 ) const // As group_Delta_Sigma, but uses offset_Delta_Sigma cache to speed it up if overwritten
	{
		return SPCP(name)->_semiquick_group_Sigma( R, group_c );
	}
	surface_density_type quick_group_Sigma( const distance_type & R,
			const flt_type & group_c=2.5 ) const // As deltasigma, but uses group_Delta_Sigma cache to speed it up if overwritten
	{
		return SPCP(name)->_quick_group_Sigma( R, group_c );
	}
#endif

	// Two Halo Sigma
#if (1)
	surface_density_type two_halo_Sigma( const distance_type & R ) const // Expected weak lensing signal in tangential shear Delta-Sigma at radius R from ensemble of satellites in group with satellite concentration group_c
	{
		return SPCP(name)->_two_halo_Delta_Sigma( R );
	}
	surface_density_type quick_two_halo_Sigma( const distance_type & R ) const // As deltasigma, but uses group_Delta_Sigma cache to speed it up if overwritten
	{
		return SPCP(name)->_quick_two_halo_Delta_Sigma( R );
	}
#endif

#include "IceBRG_lensing/two_halo_term_methods.hpp"

#endif

};

} // end namespace IceBRG

// Undef macros
#undef SPP
#undef SPCP

#endif /* _BRG_LENSING_PROFILE_EXTENSION_H_ */
