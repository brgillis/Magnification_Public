/**********************************************************************\
  @file sky_obj.cpp

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

#include "IceBRG_main/common.h"

#include "IceBRG_main/units/units.hpp"

#include "IceBRG_physics/astro_caches.h"
#include "IceBRG_physics/astro.h"

#include "sky_obj.h"

// IceBRG::sky_obj class methods
#if (1)

IceBRG::sky_obj::sky_obj( const angle_type & init_ra, const angle_type & init_dec,
		const flt_type & init_z, const angle_type & init_ra_err,
		const angle_type & init_dec_err, const flt_type & init_z_err )
{
	partial_clear();
	set_ra_dec_z_err( init_ra, init_dec, init_z, init_ra_err, init_dec_err,
			init_z_err );
}

void IceBRG::sky_obj::clear()
{
	set_ra_dec_z_err( 0, 0, 0, 0, 0, 0 );
	return partial_clear();
}

void IceBRG::sky_obj::partial_clear()
{
	_index_ = 0;
	_ID_ = "0";
	_weight_ = 1;
}

void IceBRG::sky_obj::set_ra( const angle_type & new_ra )
{
	_ra_ = new_ra;
}
void IceBRG::sky_obj::set_ra_err( const angle_type & new_ra_err )
{
	_ra_err_ = new_ra_err;
}
void IceBRG::sky_obj::set_dec( const angle_type & new_dec )
{
	_dec_ = new_dec;
}
void IceBRG::sky_obj::set_dec_err( const angle_type & new_dec_err )
{
	_dec_err_ = new_dec_err;
}
void IceBRG::sky_obj::set_ra_dec( const angle_type & new_ra,
		const angle_type & new_dec )
{
	set_ra( new_ra );
	set_dec( new_dec );
}
void IceBRG::sky_obj::set_ra_dec_z( const angle_type & new_ra,
		const angle_type & new_dec, const flt_type & new_z )
{
	set_ra( new_ra );
	set_dec( new_dec );
	set_z( new_z );
}
void IceBRG::sky_obj::set_ra_dec_z_err( const angle_type & new_ra,
		const angle_type & new_dec, const flt_type & new_z,
		const angle_type & new_ra_err, const angle_type & new_dec_err,
		const flt_type & new_z_err )
{
	set_ra( new_ra );
	set_dec( new_dec );
	set_z( new_z );
	set_ra_err( new_ra_err );
	set_dec_err( new_dec_err );
	set_z_err( new_z_err );
}
void IceBRG::sky_obj::set_ra_dec_err( const angle_type & new_ra,
		const angle_type & new_dec, const angle_type & new_ra_err,
		const angle_type & new_dec_err )
{
	set_ra( new_ra );
	set_dec( new_dec );
	set_ra_err( new_ra_err );
	set_dec_err( new_dec_err );
}
void IceBRG::sky_obj::set_weight( const flt_type & new_weight )
{
	_weight_ = new_weight;
}
void IceBRG::sky_obj::set_index( const int_type new_index )
{
	_index_ = new_index;
}
void IceBRG::sky_obj::set_ID( const std::string &new_ID )
{
	_ID_ = new_ID;
}

#endif // end IceBRG::sky_obj class methods

IceBRG::distance_type IceBRG::dfa( const IceBRG::sky_obj *obj1,
		const IceBRG::sky_obj *obj2, const flt_type & z )
{
	flt_type z_to_use;
	if ( z == -1 )
		z_to_use = obj1->z();
	else
		z_to_use = z;
	return IceBRG::dfa(
			skydist2d( obj1->ra(), obj1->dec(), obj2->ra(), obj2->dec() ),
			z_to_use );
}

IceBRG::angle_type IceBRG::skydist2d( const IceBRG::sky_obj *obj1, const IceBRG::sky_obj *obj2 )
{
	return skydist2d(obj1->ra(),obj1->dec(),obj2->ra(),obj2->dec());
}
