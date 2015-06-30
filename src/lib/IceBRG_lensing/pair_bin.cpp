/**********************************************************************\
 @file pair_bin.cpp
 ------------------

 Source file for the pair_bin class.

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

#include <limits>

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/count.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/moment.hpp>
#include <boost/accumulators/statistics/variance.hpp>
#include <boost/accumulators/statistics/stats.hpp>

#include "IceBRG_main/common.h"

#include "IceBRG_main/math/calculus/integrate.hpp"
#include "IceBRG_main/math/statistics/statistic_extractors.hpp"
#include "IceBRG_main/utility.hpp"
#include "IceBRG_main/math/statistics/effective_count.hpp"
#include "IceBRG_main/math/statistics/error_of_weighted_mean.hpp"

#include "IceBRG_lensing/lens_source_pair.h"
#include "IceBRG_lensing/pair_bin_summary.h"
#include "IceBRG_lensing/magnification/mag_global_values.h"
#include "IceBRG_lensing/magnification/magnification_alpha.h"
#include "IceBRG_lensing/magnification/magnification_functors.h"
#include "IceBRG_lensing/magnification/mag_calibration_cache.h"
#include "IceBRG_lensing/magnification/mag_signal_integral_cache.h"
#include "IceBRG_lensing/magnification/mag_weight_integral_cache.h"

#include "pair_bin.h"
#include "IceBRG_main/units/units.hpp"

namespace IceBRG {

constexpr flt_type pair_bin::_z_buffer_default_value_;

void pair_bin::_uncache_values()
{
	_mu_hat_cached_value_ = std::numeric_limits<flt_type>::infinity();
	_mu_W_cached_value_ = std::numeric_limits<flt_type>::infinity();
}

pair_bin::pair_bin( const distance_type & init_R_min, const distance_type & init_R_max,
		const mass_type & init_m_min, const mass_type & init_m_max,
		const flt_type & init_z_min, const flt_type & init_z_max,
		const flt_type & init_mag_min, const flt_type & init_mag_max,
		const flt_type & init_z_buffer)
:	pair_bin_summary(init_R_min,init_R_max,init_m_min,init_m_max,
		init_z_min,init_z_max,init_mag_min,init_mag_max),
	_mu_hat_cached_value_(std::numeric_limits<flt_type>::infinity()),
	_mu_W_cached_value_(std::numeric_limits<flt_type>::infinity()),
	_z_buffer_(init_z_buffer)
{
}

// Adding and clearing data
#if(1)

void pair_bin::add_pair( const lens_source_pair & new_pair)
{
	const flt_type & shear_weight = new_pair.shear_weight();
	const flt_type & mag_weight = new_pair.mag_weight();

	// Shear first
	if(shear_weight>0)
	{
		// General info
		_shear_R_values_(value_of(new_pair.R_proj()), boost::accumulators::weight = shear_weight);

		_shear_lens_m_values_(value_of(new_pair.m_lens()), boost::accumulators::weight = shear_weight);
		_shear_lens_mag_values_(new_pair.mag_lens(), boost::accumulators::weight = shear_weight);
		_shear_lens_z_values_(new_pair.z_lens(), boost::accumulators::weight = shear_weight);
		_shear_source_z_values_(new_pair.z_source(), boost::accumulators::weight = shear_weight);

		// Shear info
		_delta_Sigma_t_values_(value_of(new_pair.delta_Sigma_t()),
				boost::accumulators::weight = shear_weight);
		_delta_Sigma_x_values_(value_of(new_pair.delta_Sigma_x()),
				boost::accumulators::weight = shear_weight);
	}

	// Now magnification
	const auto & mag_source = new_pair.mag_source();
	const auto & z_source = new_pair.z_source();
	if((mag_weight>0) && (mag_source>=mag_m_min) && (mag_source<mag_m_max) &&
		(z_source>=mag_z_min) && (z_source<mag_z_max) )
	{
		// General info
		_magf_R_values_(value_of(new_pair.R_proj()), boost::accumulators::weight = mag_weight);
		_magf_source_z_values_(new_pair.z_source(), boost::accumulators::weight = mag_weight);

		// Magnification info
		_mu_obs_values_(IceBRG::magnification_alpha(new_pair.mag_source(),new_pair.z_lens()+_z_buffer_)-1,
				boost::accumulators::weight = mag_weight);
		_uncache_values();
	}
}
void pair_bin::add_lens( const lens_id & lens )
{
	if(_distinct_lens_ids_.find(lens.id)==_distinct_lens_ids_.end())
	{
		_distinct_lens_ids_.insert(lens.id);
		_magf_lens_m_values_(value_of(lens.m), boost::accumulators::weight = lens.weight);
		_magf_lens_mag_values_(lens.mag, boost::accumulators::weight = lens.weight);
		_magf_lens_z_values_(lens.z, boost::accumulators::weight = lens.weight);

		flt_type unmasked_frac = lens.unmasked_frac(R_amid());
		_magf_unmasked_fracs_(unmasked_frac, boost::accumulators::weight = lens.weight);

		// Get the new weight from the lens's individual weight and its unmasked area
		custom_unit_type<0,0,0,2,0> area = pi*(square(afd(R_max(),lens.z))-square(afd(R_min(),lens.z)));
		custom_unit_type<0,0,0,2,0> unmasked_area = unmasked_frac*area;

		flt_type mu_W = unmasked_area*any_cast<custom_unit_type<0,0,0,-2,0>>(
				mag_weight_integral_cache().get(lens.z+_z_buffer_));

		IceBRG::fixbad(mu_W);

		_lens_weights_.insert(std::make_pair(lens.id,lens.weight*mu_W));
		_uncache_values();
	}
}
void pair_bin::clear()
{
	set_zero(_magf_R_values_);
	set_zero(_shear_R_values_);
	set_zero(_magf_lens_m_values_);
	set_zero(_shear_lens_m_values_);
	set_zero(_magf_lens_z_values_);
	set_zero(_shear_lens_z_values_);
	set_zero(_magf_lens_mag_values_);
	set_zero(_shear_lens_mag_values_);

	set_zero(_magf_source_z_values_);
	set_zero(_shear_source_z_values_);

	set_zero(_magf_unmasked_fracs_);
	set_zero(_mu_obs_values_);

	set_zero(_delta_Sigma_t_values_);
	set_zero(_delta_Sigma_x_values_);

	set_zero(_distinct_lens_ids_);
	set_zero(_lens_weights_);

	_uncache_values();

	pair_bin_summary::clear();
}

#endif

// Calculations on stored values
#if (1)

flt_type pair_bin::magf_sum_of_weights() const
{
	flt_type res = 0;
	for(const auto & id : _distinct_lens_ids_)
	{
		res += _lens_weights_.at(id);
	}
	return res;
}

flt_type pair_bin::magf_sum_of_square_weights() const
{
	flt_type res = 0;
	for(const auto & id : _distinct_lens_ids_)
	{
		res += square(_lens_weights_.at(id));
	}
	return res;
}

custom_unit_type<0,0,0,2,0> pair_bin::area() const
{
	flt_type mean_frac = unmasked_frac();
	custom_unit_type<0,0,0,2,0> mean_area = pi*(square(afd(R_max(),magf_lens_z_mean()))-square(afd(R_min(),magf_lens_z_mean())));

	custom_unit_type<0,0,0,2,0> result = mean_frac*magf_num_lenses()*mean_area;
	IceBRG::fixbad(result);
	return result;
}

surface_density_type pair_bin::delta_Sigma_t_mean() const
{
	return units_cast<surface_density_type>(safe_extract_mean(_delta_Sigma_t_values_));
}
surface_density_type pair_bin::delta_Sigma_x_mean() const
{
	return units_cast<surface_density_type>(safe_extract_mean(_delta_Sigma_x_values_));
}

custom_unit_type<-4,0,2,0,0> pair_bin::delta_Sigma_t_mean_square() const
{
	return units_cast<custom_unit_type<-4,0,2,0,0>>(safe_extract_moment<2>(_delta_Sigma_t_values_));
}
custom_unit_type<-4,0,2,0,0> pair_bin::delta_Sigma_x_mean_square() const
{
	return units_cast<custom_unit_type<-4,0,2,0,0>>(safe_extract_moment<2>(_delta_Sigma_x_values_));
}

surface_density_type pair_bin::delta_Sigma_t_std() const
{
	return units_cast<surface_density_type>(sqrt(safe_extract_variance(_delta_Sigma_t_values_)));
}
surface_density_type pair_bin::delta_Sigma_x_std() const
{
	return units_cast<surface_density_type>(sqrt(safe_extract_variance(_delta_Sigma_x_values_)));
}

surface_density_type pair_bin::delta_Sigma_t_stderr() const
{
	return units_cast<surface_density_type>(safe_extract_error_of_weighted_mean(_delta_Sigma_t_values_));
}
surface_density_type pair_bin::delta_Sigma_x_stderr() const
{
	return units_cast<surface_density_type>(safe_extract_error_of_weighted_mean(_delta_Sigma_x_values_));
}

#endif

// Magnification calculations
#if(1)

flt_type pair_bin::mu_W() const
{
	if(_mu_W_cached_value_==std::numeric_limits<flt_type>::infinity())
	{
		_mu_W_cached_value_ = area()*
			any_cast<custom_unit_type<0,0,0,-2,0>>(mag_weight_integral_cache().get(magf_lens_z_mean()+_z_buffer_));
	}
	return _mu_W_cached_value_;
}

flt_type pair_bin::mu_hat() const
{
	if(_mu_hat_cached_value_==std::numeric_limits<flt_type>::infinity())
	{
		// Not cached, so calculate and cache it

		const flt_type & mu_observed = extract_weighted_sum(_mu_obs_values_)/(extract_mean_weight(_mu_obs_values_)*mu_W());

		const flt_type & mu_base = area()*
				any_cast<custom_unit_type<0,0,0,-2,0>>(mag_signal_integral_cache().get(magf_lens_z_mean()+_z_buffer_))
				/mu_W();

		_mu_hat_cached_value_ = 1.+mu_observed-mu_base;

		// Use calibration cache to correct for integration errors
		// Be sure to comment this line out when doing a calibration run!
		//_mu_hat_cached_value_ /= mag_calibration_cache().get(magf_lens_z_mean()+_z_buffer_);

		IceBRG::fixbad(_mu_hat_cached_value_);
	}
	return _mu_hat_cached_value_;

}

flt_type pair_bin::mu_square_hat() const
{
	return square(mu_hat());
}

#endif

} // end namespace IceBRG
