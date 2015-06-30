/**********************************************************************\
  @file lensing_tNFW_caches.cpp

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
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "IceBRG_main/common.h"

#include "IceBRG_main/math/cache/cache_4d.hpp"

#include "IceBRG_lensing/lensing_tNFW_profile.h"

#include "IceBRG_physics/astro.h"
#include "lensing_tNFW_caches.h"
#include "IceBRG_main/units/units.hpp"

// Initialisation for IceBRG::tNFW_sig_cache
DEFINE_BRG_CACHE_3D_STATIC_VARS( tNFW_sig_cache,
	std::log(1e7*unitconv::Msuntokg),std::log(1e16*unitconv::Msuntokg),(std::log(1e16)-std::log(1e7))/300,
	0.1,1.5,0.1,
	std::log(0.1*unitconv::kpctom),std::log(2000*unitconv::kpctom),(std::log(2000)-std::log(0.1))/300);

// Initialisation for IceBRG::tNFW_offset_sig_cache
DEFINE_BRG_CACHE_4D_STATIC_VARS( tNFW_offset_sig_cache,
	std::log(1e13*unitconv::Msuntokg),std::log(1e16*unitconv::Msuntokg),(std::log(1e16)-std::log(1e13))/30,
	0.1,1.5,0.4,
	std::log(0.1*unitconv::kpctom),std::log(4000*unitconv::kpctom),(std::log(4000)-std::log(0.1))/1000,
	std::log(0.01*unitconv::kpctom),std::log(2000*unitconv::kpctom),(std::log(2000)-std::log(0.01))/100);

// Initialisation for IceBRG::tNFW_group_sig_cache
DEFINE_BRG_CACHE_4D_STATIC_VARS( tNFW_group_sig_cache,
	std::log(1e13*unitconv::Msuntokg),std::log(1e15*unitconv::Msuntokg),(std::log(1e16)-std::log(1e13))/30,
	0.1,1.5,0.4,
	std::log(0.1*unitconv::kpctom),std::log(2000*unitconv::kpctom),(std::log(2000)-std::log(0.1))/100,
	2.5,10.,2.5);

// Initialisation for IceBRG::tNFW_shifted_sig_cache
DEFINE_BRG_CACHE_3D_STATIC_VARS( tNFW_shifted_sig_cache,
	std::log(1e7*unitconv::Msuntokg),std::log(1e16*unitconv::Msuntokg),(std::log(1e16)-std::log(1e7))/100,
	0.1,1.5,0.4,
	std::log(0.1*unitconv::kpctom),std::log(2000*unitconv::kpctom),(std::log(2000)-std::log(0.1))/100);

// Initialisation for IceBRG::tNFW_Sigma_cache
DEFINE_BRG_CACHE_3D_STATIC_VARS( tNFW_Sigma_cache,
	std::log(1e7*unitconv::Msuntokg),std::log(1e16*unitconv::Msuntokg),(std::log(1e16)-std::log(1e7))/300,
	0.1,1.5,0.1,
	std::log(0.1*unitconv::kpctom),std::log(2000*unitconv::kpctom),(std::log(2000)-std::log(0.1))/300);

// Initialisation for IceBRG::tNFW_offset_Sigma_cache
DEFINE_BRG_CACHE_4D_STATIC_VARS( tNFW_offset_Sigma_cache,
	std::log(1e13*unitconv::Msuntokg),std::log(1e16*unitconv::Msuntokg),(std::log(1e16)-std::log(1e13))/30,
	0.1,1.5,0.4,
	std::log(0.1*unitconv::kpctom),std::log(4000*unitconv::kpctom),(std::log(4000)-std::log(0.1))/1000,
	std::log(0.1*unitconv::kpctom),std::log(4000*unitconv::kpctom),(std::log(4000)-std::log(0.01))/100);

// Initialisation for IceBRG::tNFW_group_Sigma_cache
DEFINE_BRG_CACHE_4D_STATIC_VARS( tNFW_group_Sigma_cache,
	std::log(1e13*unitconv::Msuntokg),std::log(1e16*unitconv::Msuntokg),(std::log(1e16)-std::log(1e13))/30,
	0.1,1.5,0.4,
	std::log(0.1*unitconv::kpctom),std::log(4000*unitconv::kpctom),(std::log(4000)-std::log(0.1))/200,
	2.5,5.0,2.5);

// IceBRG::tNFW_sig_cache class methods
#if (1)
flt_type IceBRG::tNFW_sig_cache::_calculate( const flt_type & in_param_1, const flt_type & in_param_2,
		const flt_type & in_param_3 ) const
{
	const auto mass = units_cast<mass_type>(std::exp(in_param_1));
	const auto z = in_param_2;
	const auto R = units_cast<distance_type>(std::exp(in_param_3));
	IceBRG::lensing_tNFW_profile profile(mass,z);
	return value_of(profile.Delta_Sigma( R ));
}

#endif // end IceBRG::tNFW_sig_cache methods

// IceBRG::NFW_offset_sig_cache class methods
#if (1)

flt_type IceBRG::tNFW_offset_sig_cache::_calculate( const flt_type & in_param_1, const flt_type & in_param_2,
		const flt_type & in_param_3, const flt_type & in_param_4 ) const
{
	const auto mass = units_cast<mass_type>(std::exp(in_param_1));
	const auto z = in_param_2;
	const auto R = units_cast<distance_type>(std::exp(in_param_3));
	const auto offset_R = units_cast<distance_type>(std::exp(in_param_4));
	IceBRG::lensing_tNFW_profile profile(mass,z);
	return value_of(profile.offset_Delta_Sigma( R, offset_R ));
}

#endif // end IceBRG::NFW_offset_sig_cache functions

// IceBRG::tNFW_group_sig_cache class methods
#if (1)

flt_type IceBRG::tNFW_group_sig_cache::_calculate( const flt_type & in_param_1, const flt_type & in_param_2,
		const flt_type & in_param_3, const flt_type & in_param_4 ) const
{
	const auto mass = units_cast<mass_type>(std::exp(in_param_1));
	const auto z = in_param_2;
	const auto R = units_cast<distance_type>(std::exp(in_param_3));
	const auto group_c = in_param_4;
	IceBRG::lensing_tNFW_profile profile(mass,z);
	return value_of(profile.semiquick_group_Delta_Sigma( R, group_c ));
}

#endif // end IceBRG::tNFW_group_sig_cache functions

// IceBRG::tNFW_shifted_sig_cache class methods
#if (1)
flt_type IceBRG::tNFW_shifted_sig_cache::_calculate( const flt_type & in_param_1, const flt_type & in_param_2,
		const flt_type & in_param_3 ) const
{
	const auto mass = units_cast<mass_type>(std::exp(in_param_1));
	const auto z = in_param_2;
	const auto R = units_cast<distance_type>(std::exp(in_param_3));
	IceBRG::lensing_tNFW_profile profile(mass,z);
	return value_of(profile.semiquick_shifted_Delta_Sigma( R ));
}

#endif // end IceBRG::tNFW_sig_cache methods

// IceBRG::tNFW_Sigma_cache class methods
#if (1)
flt_type IceBRG::tNFW_Sigma_cache::_calculate( const flt_type & in_param_1, const flt_type & in_param_2,
		const flt_type & in_param_3 ) const
{
	const auto mass = units_cast<mass_type>(std::exp(in_param_1));
	const auto z = in_param_2;
	const auto R = units_cast<distance_type>(std::exp(in_param_3));
	IceBRG::lensing_tNFW_profile profile(mass,z);
	return value_of(profile.proj_dens( R ));
}

#endif // end IceBRG::tNFW_Sigma_cache methods

// IceBRG::NFW_offset_Sigma_cache class methods
#if (1)

flt_type IceBRG::tNFW_offset_Sigma_cache::_calculate( const flt_type & in_param_1, const flt_type & in_param_2,
		const flt_type & in_param_3, const flt_type & in_param_4 ) const
{
	const auto mass = units_cast<mass_type>(std::exp(in_param_1));
	const auto z = in_param_2;
	const auto R = units_cast<distance_type>(std::exp(in_param_3));
	const auto offset_R = units_cast<distance_type>(std::exp(in_param_4));
	IceBRG::lensing_tNFW_profile profile(mass,z);
	return value_of(profile.offset_Sigma( R, offset_R ));
}

#endif // end IceBRG::NFW_offset_Sigma_cache functions

// IceBRG::tNFW_group_Sigma_cache class methods
#if (1)

flt_type IceBRG::tNFW_group_Sigma_cache::_calculate( const flt_type & in_param_1, const flt_type & in_param_2,
		const flt_type & in_param_3, const flt_type & in_param_4 ) const
{
	const auto mass = units_cast<mass_type>(std::exp(in_param_1));
	const auto z = in_param_2;
	const auto R = units_cast<distance_type>(std::exp(in_param_3));
	const auto group_c = in_param_4;
	IceBRG::lensing_tNFW_profile profile(mass,z);
	return value_of(profile.semiquick_group_Sigma( R, group_c ));
}

#endif // end IceBRG::tNFW_group_Sigma_cache functions


