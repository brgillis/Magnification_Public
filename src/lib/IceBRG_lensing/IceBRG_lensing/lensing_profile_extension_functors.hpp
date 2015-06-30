/**********************************************************************\
  @file name_functors.h

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

// body file: name_functors.cpp

#ifndef _BRG_LENSING_PROFILE_EXTENSION_FUNCTORS_H_
#define _BRG_LENSING_PROFILE_EXTENSION_FUNCTORS_H_

#include "IceBRG_main/units/unit_conversions.hpp"
#include "IceBRG_main/units/units.hpp"
#include "IceBRG_main/common.h"

#include "IceBRG_main/math/calculus/integrate.hpp"

#include "IceBRG_physics/astro.h"

namespace IceBRG {

template<typename name>
class projected_density_functor
{
	/**********************************
	 projected_density_functor class
	 -----------------------------

	 Function class integrating density_type along a projected line

	 Parent class: function_class (from brg_functors)

	 **********************************/
private:
	const name *_host_ptr_;
	distance_type _offset_R_;

public:

	void set_host_ptr( const name *new_host_ptr )
	{
		_host_ptr_ = new_host_ptr;
	}
	const name * host_ptr()
	{
		return _host_ptr_;
	}

	void set_offset_R( const distance_type & new_offset_R )
	{
		_offset_R_ = new_offset_R;
	}
	const distance_type offset_R()
	{
		return _offset_R_;
	}

	surface_density_type operator()( const distance_type & in_param ) const
	{
		if ( _host_ptr_ == NULL )
		{
			throw std::runtime_error("ERROR: Host must be assigned to projected_density_functor before function can be called.\n");
		}
		distance_type r = quad_add( in_param, _offset_R_ );
		return _host_ptr_->dens( r );
	}

	projected_density_functor( const name *init_host=NULL,
			const distance_type & init_offset_R=0 )
	{
		set_host_ptr( init_host );
		set_offset_R( init_offset_R );
	}
	virtual ~projected_density_functor()
	{
	}
};

template<typename name>
class cylindrical_density_functor
{
	/**********************************
	 cylindrical_density_functor class
	 -----------------------------

	 Function class for integrating density_type in a cylinder

	 Parent class: function_class (from brg_functors)

	 **********************************/
private:
	const name *_host_ptr_;

public:

	void set_host_ptr( const name *new_host_ptr )
	{
		_host_ptr_ = new_host_ptr;
	}
	const name * host_ptr()
	{
		return _host_ptr_;
	}

	custom_unit_type<-1,0,1,0,0> operator()( const distance_type & in_param ) const
	{
		if ( _host_ptr_ == NULL )
		{
			throw std::runtime_error("ERROR: Host must be assigned to cylindrical_density_functor before function can be called.\n");
		}
		return 2 * pi * in_param * _host_ptr_->proj_dens( in_param );
	}

	cylindrical_density_functor( const name *init_host = NULL )
	{
		set_host_ptr( init_host );
	}
	virtual ~cylindrical_density_functor()
	{
	}
};

template<typename name>
class offset_ring_dens_functor
{

	const name *_host_ptr_;
	distance_type _R0_, _R_;

public:

	void set_host_ptr( const name *new_host_ptr )
	{
		_host_ptr_ = new_host_ptr;
	}
	const name * host_ptr()
	{
		return _host_ptr_;
	}

	void set_R0( const distance_type & new_R0 )
	{
		_R0_ = new_R0;
	}
	const distance_type &  R0()
	{
		return _R0_;
	}

	void set_R( const distance_type & new_R )
	{
		_R_ = new_R;
	}
	const distance_type &  R()
	{
		return _R_;
	}

	surface_density_type operator()( const angle_type &  in_param ) const
	{
		if ( _host_ptr_ == NULL )
		{
			throw std::runtime_error("ERROR: Host must be assigned to offset_ring_dens_functor before function can be called.\n");
		}

		distance_type d = lc_add( _R0_, _R_, in_param );

		return _host_ptr_->proj_dens( d );
	}

	offset_ring_dens_functor( const name *new_host=NULL,
			const distance_type & new_R_0 = 0, const distance_type & new_R = 0 )
	{
		_host_ptr_ = new_host;
		_R_ = new_R;
		_R0_ = new_R_0;
	}

};

template<typename name>
class offset_circ_dens_functor
{
private:
	const name *_host_ptr_;
	distance_type _R0_, _R_;

	distance_type _arc_length_in_circle( const distance_type & R2 ) const
	{
		// Check for complete enclosure
		if( _R0_ + R2 <= _R_)
		{
			return 2.*pi*R2;
		}
		else
		{
			distance_type res = 2.*R2 * std::acos( (square(_R0_)-square(_R_)+square(R2)) / (2.*_R0_*R2) );
			if(value_of(res)>0) return res;
			return units_cast<distance_type>(0.); // We'll get here only in cases due to round-off error, where it should actually be zero
		}
	}

public:

	void set_host_ptr( const name *new_host_ptr )
	{
		_host_ptr_ = new_host_ptr;
	}
	const name * host_ptr()
	{
		return _host_ptr_;
	}

	void set_R0( const distance_type & new_R0 )
	{
		_R0_ = new_R0;
	}
	const distance_type & R0()
	{
		return _R0_;
	}

	void set_R( const distance_type & new_R )
	{
		_R_ = new_R;
	}
	const distance_type & R()
	{
		return _R_;
	}

	custom_unit_type<-1,0,1,0,0> operator()( const distance_type & in_param ) const
	{
		if ( _host_ptr_ == NULL )
		{
			throw std::runtime_error("ERROR: Host must be assigned to offset_circ_dens_functor before function can be called.\n");
		}

		distance_type L = _arc_length_in_circle(in_param);

		return L * _host_ptr_->proj_dens( in_param );
	}

	offset_circ_dens_functor( const name *new_host=NULL,
			const distance_type & new_R0 = 0, const distance_type & new_R = 0  )
	{
		_host_ptr_ = new_host;
		_R0_ = new_R0;
		_R_ = new_R;
	}
};

template<typename name>
class offset_Delta_Sigma_functor
{

private:

	const name *_host_ptr_;
	distance_type _R_;

public:

	void set_host_ptr( const name *new_host_ptr )
	{
		_host_ptr_ = new_host_ptr;
	}
	const name * host_ptr()
	{
		return _host_ptr_;
	}

	void set_R( const distance_type & new_R )
	{
		_R_ = new_R;
	}
	const distance_type &  R()
	{
		return _R_;
	}

	surface_density_type operator()( const distance_type &  in_param ) const
	{
		if ( _host_ptr_ == NULL )
		{
			throw std::runtime_error("ERROR: Host must be assigned to offset_Delta_Sigma_functor before function can be called.\n");
		}

		return _host_ptr_->offset_Delta_Sigma( _R_, in_param );
	}

	offset_Delta_Sigma_functor( const name *init_host=NULL,
			const distance_type & init_R = 0 )
	{
		_host_ptr_ = init_host;
		_R_ = init_R;
	}

};

template<typename name>
class quick_offset_Delta_Sigma_functor
{

private:

	const name *_host_ptr_;
	distance_type _R_;

public:

	void set_host_ptr( const name *new_host_ptr )
	{
		_host_ptr_ = new_host_ptr;
	}
	const name * host_ptr()
	{
		return _host_ptr_;
	}

	void set_R( const distance_type & new_R )
	{
		_R_ = new_R;
	}
	const distance_type &  R()
	{
		return _R_;
	}

	surface_density_type operator()( const distance_type &  in_param ) const
	{
		if ( _host_ptr_ == NULL )
		{
			throw std::runtime_error("ERROR: Host must be assigned to offset_Delta_Sigma_functor before function can be called.\n");
		}

		return _host_ptr_->quick_offset_Delta_Sigma( _R_, in_param );
	}

	quick_offset_Delta_Sigma_functor( const name *init_host=NULL,
			const distance_type & init_R = 0 )
	{
		_host_ptr_ = init_host;
		_R_ = init_R;
	}

};

template<typename name>
class group_Delta_Sigma_weight_functor
{

private:

	const name *_host_ptr_;
	flt_type _c_;

public:

	void set_host_ptr( const name *new_host_ptr )
	{
		_host_ptr_ = new_host_ptr;
	}
	const name * host_ptr()
	{
		return _host_ptr_;
	}

	void set_c( const flt_type & new_c )
	{
		_c_ = new_c;
	}
	const flt_type & c() noexcept
	{
		return _c_;
	}

	flt_type operator()( const distance_type &  in_param ) const
	{
		if ( _host_ptr_ == NULL )
		{
			throw std::runtime_error("ERROR: Host must be assigned to offset_Delta_Sigma_functor before function can be called.\n");
		}

		if ( _c_ == 0 )
		{
			return value_of(2. * pi * in_param * _host_ptr_->proj_dens( in_param ));
		}
		else
		{
			BRG_UNIQUE_PTR<name> group_profile(_host_ptr_->lensing_profile_extension_clone());
			group_profile->set_c(_c_);
			group_profile->set_tau(group_profile->tau()*_c_/_host_ptr_->c());
			return value_of(2. * pi * in_param * group_profile->proj_dens(in_param));
		}
	}

	group_Delta_Sigma_weight_functor( const name *init_host=NULL,
			const flt_type & init_c = -1 )
	{
		_host_ptr_ = init_host;
		_c_ = init_c;
	}

};

template<typename name>
class shifted_Delta_Sigma_weight_functor
{

private:

	distance_type _sigma_;

public:

	void set_sigma( const distance_type & new_sigma )
	{
		_sigma_ = new_sigma;
	}
	const distance_type & sigma()
	{
		return _sigma_;
	}

	flt_type operator()( const distance_type &  in_param ) const
	{
		// The output here is the height of a Rayleigh distribution at in_param
		return value_of(in_param/square(_sigma_) * std::exp(-square(in_param)/(2.*square(_sigma_))));
	}

	shifted_Delta_Sigma_weight_functor( const distance_type & new_sigma=0 )
	{
		_sigma_ = new_sigma;
	}

};

template<typename name>
class shifted_Delta_Sigma_circ_functor
{

private:

	const name *_host_ptr_;
	distance_type _R_, _R_shift_;

public:

	void set_host_ptr( const name *new_host_ptr )
	{
		_host_ptr_ = new_host_ptr;
	}
	const name * host_ptr()
	{
		return _host_ptr_;
	}

	void set_R_shift( const distance_type & new_R_shift )
	{
		_R_shift_ = new_R_shift;
	}
	const distance_type & R_shift()
	{
		return _R_shift_;
	}

	void set_R( const distance_type & new_R )
	{
		_R_ = new_R;
	}
	const distance_type & R()
	{
		return _R_;
	}

	surface_density_type operator()( const angle_type &  in_param ) const
	{
		// in_param here will be angle_type theta in radians

		const distance_type R_actual(lc_add(_R_, _R_shift_, in_param));

		const angle_type theta(asin(_R_shift_/R_actual * sin(in_param)));
		const flt_type & angle_factor = cos(theta);

		flt_type extra_shear_factor;
		if(value_of(_R_shift_)==0)
			extra_shear_factor = 0;
		else
			extra_shear_factor = (R_actual-_R_)/_R_shift_*
				_host_ptr_->shift_factor(1*IceBRG::unitconv::kpctom*m);

		if(isbad(theta))
		{
			return _host_ptr_->Delta_Sigma(R_actual) +
				extra_shear_factor*sigma_crit(_host_ptr_->z(),2*_host_ptr_->z());
		}
		else
		{
			return _host_ptr_->Delta_Sigma(R_actual)*angle_factor +
					extra_shear_factor*sigma_crit(_host_ptr_->z(),2*_host_ptr_->z()); //TODO Should there be a factor of 1/2 here?
		}
	}

	shifted_Delta_Sigma_circ_functor( const name *new_host=NULL,
			const distance_type & new_R_shift=0, const distance_type & new_R=0 )
	{
		_host_ptr_ = new_host;
		_R_shift_ = new_R_shift;
		_R_ = new_R;
	}

};

template<typename name>
class shifted_Delta_Sigma_functor
{

private:

	const name *_host_ptr_;
	distance_type _R_;

public:

	void set_host_ptr( const name *new_host_ptr )
	{
		_host_ptr_ = new_host_ptr;
	}
	const name * host_ptr()
	{
		return _host_ptr_;
	}

	void set_R( const distance_type & new_R )
	{
		_R_ = new_R;
	}
	const distance_type & R()
	{
		return _R_;
	}

	surface_density_type operator()( const distance_type & in_param ) const
	{
		// in_param here will be R_shift
		shifted_Delta_Sigma_circ_functor<name> func(_host_ptr_,in_param,_R_);

		const angle_type min_in_param = 0 * rad;
		const angle_type max_in_param = pi * rad;
		const flt_type & precision = 0.000001;

		auto out_param = IceBRG::integrate_Romberg( func, min_in_param, max_in_param,
				precision, false );

		return out_param / (pi * rad);
	}

	shifted_Delta_Sigma_functor( const name *new_host=NULL,
			const distance_type & new_R=0 )
	{
		_host_ptr_ = new_host;
		_R_ = new_R;
	}

};

} // namespace IceBRG

#endif /* _BRG_name_FUNCTORS_H_ */
