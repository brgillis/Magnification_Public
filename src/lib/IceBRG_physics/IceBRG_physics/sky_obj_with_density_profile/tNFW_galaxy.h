/**********************************************************************\
  @file tNFW_galaxy.h

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

// body file: tNFW_galaxy.cpp

#ifndef _BRG_TNFW_GALAXY_H_
#define _BRG_TNFW_GALAXY_H_

#include "IceBRG_main/common.h"

#include "IceBRG_physics/density_profile/density_profile.h"
#include "IceBRG_physics/density_profile/tNFW_profile.h"
#include "IceBRG_physics/sky_obj/sky_obj.h"
#include "IceBRG_physics/sky_obj/galaxy.h"

namespace IceBRG {

class tNFW_galaxy: public tNFW_profile, public galaxy
{
	// Simple combination of the two classes
public:
	tNFW_galaxy()
	{
		galaxy();
		tNFW_profile();
	}
	virtual redshift_obj *redshift_obj_clone() const
	{
		return new tNFW_galaxy( *this );
	}
	virtual density_profile *density_profile_clone() const
	{
		return new tNFW_galaxy( *this );
	}
	virtual tNFW_profile *tNFW_profile_clone() const
	{
		return new tNFW_galaxy( *this );
	}
	virtual sky_obj *sky_obj_clone() const
	{
		return new tNFW_galaxy( *this );
	}
	virtual galaxy *galaxy_clone() const
	{
		return new tNFW_galaxy( *this );
	}
};

} // end namespace IceBRG

#endif /* _BRG_TNFW_GALAXY_H_ */
