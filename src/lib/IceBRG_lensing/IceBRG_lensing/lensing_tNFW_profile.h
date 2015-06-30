/**********************************************************************\
  @file lensing_tNFW_profile.h

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

// body file: lensing_tNFW_profile.cpp

#ifndef _BRG_LENSING_TNFW_PROFILE_H_
#define _BRG_LENSING_TNFW_PROFILE_H_

#include "IceBRG_main/common.h"

#include "IceBRG_lensing/lensing_profile_extension.hpp"
#include "IceBRG_physics/density_profile/tNFW_profile.h"

namespace IceBRG {

/*
 *
 */
class lensing_tNFW_profile: public tNFW_profile, public lensing_profile_extension<lensing_tNFW_profile> {
private:

	// Lensing related calculation methods
#if (1)
	surface_density_type _proj_dens( const distance_type & R ) const;
	surface_density_type _proj_enc_dens( const distance_type & R ) const;
	mass_type _proj_enc_mass( const distance_type & R ) const;
	surface_density_type _quick_Delta_Sigma( const distance_type & R ) const;
	surface_density_type _quick_offset_Delta_Sigma( const distance_type & R,
			const distance_type & offset_R ) const;
	surface_density_type _quick_group_Delta_Sigma( const distance_type & R,
			const flt_type & group_c ) const;
	surface_density_type _quick_shifted_Delta_Sigma( const distance_type & R ) const;
	surface_density_type _quick_Sigma( const distance_type & R ) const;
	surface_density_type _quick_offset_Sigma( const distance_type & R,
			const distance_type & offset_R ) const;
	surface_density_type _quick_group_Sigma( const distance_type & R,
			const flt_type & group_c ) const;

#endif // Lensing related calculation methods

	friend class lensing_profile_extension<lensing_tNFW_profile>;

public:
	// Constructors and destructor
#if (1)
	lensing_tNFW_profile()
	{
	}
	lensing_tNFW_profile( const mass_type & init_mvir0, const flt_type & init_z,
			const flt_type & init_c = -1, const flt_type & init_tau = -1 )
	: tNFW_profile(init_mvir0, init_z, init_c, init_tau)
	{
	}
	virtual ~lensing_tNFW_profile()
	{
	}
#endif // Constructors and destructor

	// Implementations of clone functions
#if (1)
	virtual density_profile *density_profile_clone() const
	{
		return new lensing_tNFW_profile( *this );
	}
	virtual tNFW_profile *tNFW_profile_clone() const
	{
		return new lensing_tNFW_profile( *this );
	}
#endif
};

} // end namespace IceBRG

#endif /* _BRG_LENSING_TNFW_PROFILE_H_ */
