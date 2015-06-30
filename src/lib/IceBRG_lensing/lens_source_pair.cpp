/**********************************************************************\
 @file lens_source_pair.cpp
 ------------------

 TODO <Insert file description here>

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

#ifndef _BRG_USE_CPP_11_STD_
#include <boost/shared_ptr.hpp>
#else
#include <memory>
#endif

#include "IceBRG_main/common.h"

#include "IceBRG_main/math/misc_math.hpp"

#include "IceBRG_lensing/source_obj.hpp"

#include "IceBRG_physics/sky_obj/sky_obj.h"
#include "IceBRG_main/units/units.hpp"

#include "lens_source_pair.h"

namespace IceBRG {

#if(1) // Private methods

void lens_source_pair::_calc_gamma(const flt_type & gamma_1, const flt_type & gamma_2) const
{
	flt_type rot_cos = cos(-2.*_theta_);
	flt_type rot_sin = sin(-2.*_theta_);
	_gamma_t_ = -(rot_cos*gamma_1-rot_sin*gamma_2);
	_gamma_x_ = rot_sin*gamma_1+rot_cos*gamma_2;
}

#endif // Private methods

#if(1) // Public methods

// Calculation/recalculation function
#if(1)
void lens_source_pair::store_data() const
{
	assert(lens()!=NULL);
	assert(source()!=NULL);
	const sky_obj *lens_ptr = lens();
	const source_obj *source_ptr = source();

	_z_lens_ = lens_ptr->z();
	_z_source_ = source_ptr->z();
	_m_lens_ = lens_ptr->m();
	_id_lens_ = lens_ptr->index();
	_mag_lens_ = lens_ptr->mag();
	_mag_source_ = source_ptr->mag();
	_weight_lens_ = lens_ptr->weight();
	_weight_source_ = source_ptr->weight();

	// Note minus sign in dra to correct for ra's reverse orientation in the sky
	angle_type dra = -(source_ptr->ra()-lens_ptr->ra())*cos(lens_ptr->dec());
	angle_type ddec = source_ptr->dec()-lens_ptr->dec();

	_R_proj_ = dfa(skydist2d(lens_ptr,source_ptr),_z_lens_);
	_theta_ = atan2(ddec,dra);

	_calc_gamma(source_ptr->gamma_1(),source_ptr->gamma_2());
}
#endif

// Constructors and destructor
#if(1)
lens_source_pair::lens_source_pair()
:	_using_lens_clone_(false),
 	_using_source_clone_(false),
	_init_lens_ptr_(NULL),
	_init_source_ptr_(NULL),
	_data_stored_(false),
	_z_lens_(0),
	_z_source_(0),
	_m_lens_(0),
	_id_lens_(0),
	_mag_lens_(0),
	_mag_source_(0),
	_weight_lens_(1),
	_weight_source_(1),
	_weight_pair_(1),
	_R_proj_(0),
	_theta_(0),
	_gamma_t_(0),
	_gamma_x_(0)
{
}
lens_source_pair::lens_source_pair( const sky_obj* lens_ptr, const source_obj* source_ptr,
		flt_type init_weight_pair, bool make_clones)
:	_using_lens_clone_(make_clones),
 	_using_source_clone_(make_clones),
 	_init_lens_ptr_(lens_ptr),
	_init_source_ptr_(source_ptr),
	_data_stored_(false),
	_z_lens_(0),
	_z_source_(0),
	_m_lens_(0),
	_id_lens_(0),
	_mag_lens_(0),
	_mag_source_(0),
	_weight_lens_(1),
	_weight_source_(1),
	_weight_pair_(init_weight_pair),
	_R_proj_(0),
	_theta_(0),
	_gamma_t_(0),
	_gamma_x_(0)
{
	if(make_clones)
	{
		_lens_clone_ = BRG_SHARED_PTR<const sky_obj>(lens_ptr->sky_obj_clone());
		_source_clone_ = BRG_SHARED_PTR<const source_obj>(source_ptr->source_obj_clone());
	}
	store_data();
}
lens_source_pair::~lens_source_pair()
{
}
#endif

// Set lens and source
#if(1)

void lens_source_pair::set_lens( const sky_obj *lens_ptr, const bool make_clone)
{
	assert(lens_ptr!=NULL);

	if(make_clone)
	{
		_lens_clone_ = BRG_SHARED_PTR<const sky_obj>(lens_ptr->sky_obj_clone());
	}
	else
	{
		_init_lens_ptr_ = lens_ptr;
	}
	_using_lens_clone_ = make_clone;
	store_data();
}

void lens_source_pair::set_source( const source_obj *source_ptr, const bool make_clone)
{
	assert(source_ptr!=NULL);

	if(make_clone)
	{
		_source_clone_ = BRG_SHARED_PTR<const source_obj>(source_ptr->source_obj_clone());
	}
	else
	{
		_init_source_ptr_ = source_ptr;
	}
	_using_source_clone_ = make_clone;
	store_data();
}

#endif

// Set pair weight
#if(1)
void lens_source_pair::set_weight_pair( flt_type new_weight_pair )
{
	_weight_pair_ = new_weight_pair;
}
#endif


// Lens and source access
#if(1)
const sky_obj *lens_source_pair::lens() const
{
	if(_using_lens_clone_)
	{
		return _lens_clone_.get();
	}
	else
	{
		return _init_lens_ptr_;
	}
}
const source_obj *lens_source_pair::source() const
{
	if(_using_source_clone_)
	{
		return _source_clone_.get();
	}
	else
	{
		return _init_source_ptr_;
	}
}
#endif // Lens and source access

// Access to stored values
#if(1)

const flt_type & lens_source_pair::z_lens() const
{
	_conditional_store_data();
	return _z_lens_;
}
const flt_type & lens_source_pair::z_source() const
{
	_conditional_store_data();
	return _z_source_;
}
flt_type lens_source_pair::z_diff() const
{
	_conditional_store_data();
	return _z_source_-_z_lens_+std::numeric_limits<flt_type>::epsilon();
}
const mass_type & lens_source_pair::m_lens() const
{
	_conditional_store_data();
	return _m_lens_;
}
const size_t & lens_source_pair::id_lens() const
{
	_conditional_store_data();
	return _id_lens_;
}
const flt_type & lens_source_pair::mag_lens() const
{
	_conditional_store_data();
	return _mag_lens_;
}
const flt_type & lens_source_pair::mag_source() const
{
	_conditional_store_data();
	return _mag_source_;
}
const flt_type & lens_source_pair::weight_lens() const
{
	_conditional_store_data();
	return _weight_lens_;
}
const flt_type & lens_source_pair::weight_source() const
{
	_conditional_store_data();
	return _weight_source_;
}
const flt_type & lens_source_pair::weight_pair() const
{
	_conditional_store_data();
	return _weight_pair_;
}
flt_type lens_source_pair::shear_weight() const
{
	_conditional_store_data();
	return _weight_lens_*_weight_source_*_weight_pair_;
}
flt_type lens_source_pair::mag_weight() const
{
	_conditional_store_data();
	return _weight_lens_*_weight_pair_;
}
const distance_type & lens_source_pair::R_proj() const
{
	_conditional_store_data();
	return _R_proj_;
}
const angle_type & lens_source_pair::theta() const
{
	_conditional_store_data();
	return _theta_;
}
const flt_type & lens_source_pair::gamma_t() const
{
	_conditional_store_data();
	return _gamma_t_;
}
const flt_type & lens_source_pair::gamma_x() const
{
	_conditional_store_data();
	return _gamma_x_;
}

#endif // Access to stored values

// Calculated values
#if(1)

surface_density_type lens_source_pair::sigma_crit() const
{
	_conditional_store_data();
	if(_z_lens_>=_z_source_) return 0;
	return square( c ) / ( 4. * pi * Gc )
			* ad_distance( 0, _z_source_ )
			/ ( ad_distance( 0, _z_lens_ ) * ad_distance( _z_lens_, _z_source_ ) );
}

surface_density_type lens_source_pair::delta_Sigma_t() const
{
	return sigma_crit()*gamma_t();
}

surface_density_type lens_source_pair::delta_Sigma_x() const
{
	return sigma_crit()*gamma_x();
}
#endif // Calculated values

#endif // Public methods

} // end namespace IceBRG
