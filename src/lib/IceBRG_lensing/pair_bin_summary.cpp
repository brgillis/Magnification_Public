/**********************************************************************\
 @file pair_bin_summary.cpp
 ------------------

 Source file for the pair_bin class.

 **********************************************************************

 Copyright (C) 2014, 2015  Bryan R. Gillis

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
#include <fstream>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include "IceBRG_main/common.h"

#include "IceBRG_main/file_access/open_file.hpp"
#include "IceBRG_main/math/calculus/integrate.hpp"
#include "IceBRG_main/math/misc_math.hpp"
#include "IceBRG_main/vector/summary_functions.hpp"

#include "IceBRG_lensing/magnification/magnification_alpha.h"
#include "IceBRG_lensing/magnification/magnification_functors.h"
#include "IceBRG_lensing/pair_bin.h"
#include "IceBRG_main/units/units.hpp"

#include "pair_bin_summary.h"

namespace IceBRG {

// Private methods
#if(1)

flt_type pair_bin_summary::_magf_gamma_t_mean() const
{
	return delta_Sigma_t_mean()/magf_sigma_crit();
}
flt_type pair_bin_summary::_magf_gamma_t_mean_square() const
{
	return square(_magf_gamma_t_mean());
}
flt_type pair_bin_summary::_magf_gamma_x_mean() const
{
	return delta_Sigma_x_mean()/magf_sigma_crit();
}
flt_type pair_bin_summary::_magf_gamma_x_mean_square() const
{
	return square(_magf_gamma_x_mean());
}
flt_type pair_bin_summary::_magf_gamma_mean() const
{
	return std::sqrt(gamma_mean_square());
}
flt_type pair_bin_summary::_magf_gamma_mean_square() const
{
	return square(gamma_t_mean())+square(gamma_x_mean());

}

flt_type pair_bin_summary::_magf_gamma_t_stderr() const
{
	return delta_Sigma_t_stderr()/magf_sigma_crit();
}
flt_type pair_bin_summary::_magf_gamma_t_square_stderr() const
{
	return 2*_magf_gamma_t_mean()*_magf_gamma_t_stderr();
}
flt_type pair_bin_summary::_magf_gamma_x_stderr() const
{
	return delta_Sigma_x_stderr()/magf_sigma_crit();
}
flt_type pair_bin_summary::_magf_gamma_x_square_stderr() const
{
	return 2*_magf_gamma_x_mean()*_magf_gamma_x_stderr();
}
flt_type pair_bin_summary::_magf_gamma_stderr() const
{
	return quad_add_err(gamma_t_mean(),gamma_t_stderr(),gamma_x_mean(),gamma_x_stderr());
}
flt_type pair_bin_summary::_magf_gamma_square_stderr() const
{
	return 2*gamma_mean()*gamma_stderr();
}

lensing_tNFW_profile pair_bin_summary::_shear_model_profile(const flt_type & MLratio) const
{
	return lensing_tNFW_profile(MLratio*shear_lens_m_mean(),shear_lens_z_mean());
}

lensing_tNFW_profile pair_bin_summary::_magf_model_profile(const flt_type & MLratio) const
{
	return lensing_tNFW_profile(MLratio*magf_lens_m_mean(),magf_lens_z_mean());
}

#endif

pair_bin_summary::pair_bin_summary( const distance_type & init_R_min, const distance_type & init_R_max,
		const mass_type & init_m_min, const mass_type & init_m_max,
		const flt_type & init_z_min, const flt_type & init_z_max,
		const flt_type & init_mag_min, const flt_type & init_mag_max )
:	_R_min_(init_R_min),
	_R_max_(init_R_max),
	_m_min_(init_m_min),
	_m_max_(init_m_max),
	_z_min_(init_z_min),
	_z_max_(init_z_max),
	_mag_min_(init_mag_min),
	_mag_max_(init_mag_max),

	_shear_pair_count_(0),
	_magf_pair_count_(0),

	_shear_R_mean_(0),
	_magf_R_mean_(0),

	_shear_lens_m_mean_(0),
	_magf_lens_m_mean_(0),
	_shear_lens_z_mean_(0),
	_magf_lens_z_mean_(0),
	_shear_lens_mag_mean_(0),
	_magf_lens_mag_mean_(0),

	_shear_source_z_mean_(0),
	_magf_source_z_mean_(0),

	_shear_sum_of_weights_(0),
	_magf_sum_of_weights_(0),
	_shear_sum_of_square_weights_(0),
	_magf_sum_of_square_weights_(0),
	_delta_Sigma_t_mean_(0),
	_delta_Sigma_x_mean_(0),
	_delta_Sigma_t_mean_square_(0),
	_delta_Sigma_x_mean_square_(0),

	_mu_hat_(0),
	_mu_square_hat_(0),
	_area_(0),
	_magf_lens_count_(0)
{
}

pair_bin_summary::pair_bin_summary( const pair_bin & bin)
: 	_R_min_(bin.R_min()),
	_R_max_(bin.R_max()),
	_m_min_(bin.m_min()),
	_m_max_(bin.m_max()),
	_z_min_(bin.z_min()),
	_z_max_(bin.z_max()),
	_mag_min_(bin.mag_min()),
	_mag_max_(bin.mag_max()),

	_shear_pair_count_(bin.shear_pair_count()),
	_magf_pair_count_(bin.magf_pair_count()),

	_shear_R_mean_(bin.shear_R_mean()),
	_magf_R_mean_(bin.magf_R_mean()),

	_shear_lens_m_mean_(bin.shear_lens_m_mean()),
	_magf_lens_m_mean_(bin.magf_lens_m_mean()),
	_shear_lens_z_mean_(bin.shear_lens_z_mean()),
	_magf_lens_z_mean_(bin.magf_lens_z_mean()),
	_shear_lens_mag_mean_(bin.shear_lens_mag_mean()),
	_magf_lens_mag_mean_(bin.magf_lens_mag_mean()),

	_shear_source_z_mean_(bin.shear_source_z_mean()),
	_magf_source_z_mean_(bin.magf_source_z_mean()),

	_shear_sum_of_weights_(bin.shear_sum_of_weights()),
	_magf_sum_of_weights_(bin.magf_sum_of_weights()),
	_shear_sum_of_square_weights_(bin.shear_sum_of_square_weights()),
	_magf_sum_of_square_weights_(bin.magf_sum_of_square_weights()),
	_delta_Sigma_t_mean_(bin.delta_Sigma_t_mean()),
	_delta_Sigma_x_mean_(bin.delta_Sigma_x_mean()),
	_delta_Sigma_t_mean_square_(bin.delta_Sigma_t_mean_square()),
	_delta_Sigma_x_mean_square_(bin.delta_Sigma_x_mean_square()),

	_mu_hat_(bin.mu_hat()),
	_mu_square_hat_(bin.mu_square_hat()),
	_area_(bin.area()),
	_magf_lens_count_(bin.magf_num_lenses())
{
}

pair_bin_summary::pair_bin_summary( std::istream & in )
{
	load(in);
}
pair_bin_summary::pair_bin_summary( std::string & filename )
{
	load(filename);
}

// Adding and clearing data
#if(1)

void pair_bin_summary::clear()
{
	_shear_pair_count_ = 0;
	_magf_pair_count_ = 0;

	_shear_R_mean_ = 0;
	_magf_R_mean_ = 0;

	_shear_lens_m_mean_ = 0;
	_magf_lens_m_mean_ = 0;
	_shear_lens_z_mean_ = 0;
	_magf_lens_z_mean_ = 0;
	_shear_lens_mag_mean_ = 0;
	_magf_lens_mag_mean_ = 0;

	_shear_source_z_mean_ = 0;
	_magf_source_z_mean_ = 0;

	_shear_sum_of_weights_ = 0;
	_magf_sum_of_weights_ = 0;
	_shear_sum_of_square_weights_ = 0;
	_magf_sum_of_square_weights_ = 0;

	_delta_Sigma_t_mean_ = 0;
	_delta_Sigma_x_mean_ = 0;
	_delta_Sigma_t_mean_square_ = 0;
	_delta_Sigma_x_mean_square_ = 0;

	_mu_hat_ = 0;
	_mu_square_hat_ = 0;
	_area_ = 0;
	_magf_lens_count_ = 0;

	_uncache_values();
}

#endif // Adding and clearing data

// Combining summaries together
#if(1)

pair_bin_summary & pair_bin_summary::operator+=( const pair_bin_summary & other )
{
	// Check bin limits are all the same
	assert(_R_min_==other.R_min());
	assert(_R_max_==other.R_max());
	assert(_m_min_==other.m_min());
	assert(_m_max_==other.m_max());
	assert(_z_min_==other.z_min());
	assert(_z_max_==other.z_max());
	assert(_mag_min_==other.mag_min());
	assert(_mag_max_==other.mag_max());

	// Check whether this or the other is empty in shear or magnification data
	const bool this_magf_zero = (magf_sum_of_weights()==0);
	const bool other_magf_zero = (other.magf_sum_of_weights()==0);
	const bool this_shear_zero = (shear_sum_of_weights()==0);
	const bool other_shear_zero = (other.shear_sum_of_weights()==0);

	// Check if the other is completely empty
	if(other_magf_zero && other_shear_zero) return *this;

	_uncache_values(); // If we get this far, something is changing with this bin

	// If the other's magnification data isn't empty
	if(!other_magf_zero)
	{

		// If this is empty, simply copy everything
		if(this_magf_zero)
		{
			// Add the magnification data
			_magf_pair_count_ = other.magf_pair_count();
			_magf_lens_count_ = other.magf_num_lenses();
			_area_ = other.area();

			_magf_R_mean_ = other.magf_R_mean();

			_magf_lens_m_mean_ = other.magf_lens_m_mean();
			_magf_lens_z_mean_ = other.magf_lens_z_mean();
			_magf_lens_mag_mean_ = other.magf_lens_mag_mean();

			_magf_source_z_mean_ = other.magf_source_z_mean();

			_magf_sum_of_weights_ = other.magf_sum_of_weights();
			_magf_sum_of_square_weights_ = other.magf_sum_of_square_weights();

			_mu_hat_ = other.mu_hat();
			_mu_square_hat_ = other.mu_square_hat();
		}
		else
		{
			// Add the magnification data
			_magf_lens_count_ += other.magf_num_lenses();
			_area_ += other.area();

			const auto new_pair_count = magf_pair_count() + other.magf_pair_count();

			if(new_pair_count > 0)
			{
				_magf_R_mean_ = (magf_R_mean()*(flt_type)magf_pair_count() +
						other.magf_R_mean()*(flt_type)other.magf_pair_count())/(flt_type)new_pair_count;
				_magf_source_z_mean_ = (magf_source_z_mean()*magf_pair_count() +
						other.magf_source_z_mean()*other.magf_pair_count())/new_pair_count;
			}

			_magf_pair_count_ = new_pair_count;

			IceBRG::fixbad(_magf_sum_of_weights_);
			const auto o_mu_W = other.magf_sum_of_weights();
			const auto new_mu_W = _magf_sum_of_weights_ + o_mu_W;

			_mu_hat_ = (mu_hat()*_magf_sum_of_weights_ + other.mu_hat()*o_mu_W)/new_mu_W;
			_mu_square_hat_ = (mu_square_hat()*_magf_sum_of_weights_ + other.mu_square_hat()*o_mu_W)/new_mu_W;

			_magf_lens_m_mean_ = (magf_lens_m_mean()*_magf_sum_of_weights_ + other.magf_lens_m_mean()*o_mu_W)/new_mu_W;
			_magf_lens_z_mean_ = (magf_lens_z_mean()*_magf_sum_of_weights_ + other.magf_lens_z_mean()*o_mu_W)/new_mu_W;
			_magf_lens_mag_mean_ = (magf_lens_mag_mean()*_magf_sum_of_weights_ + other.magf_lens_mag_mean()*o_mu_W)/new_mu_W;

			_magf_sum_of_weights_ = new_mu_W;
			_magf_sum_of_square_weights_ += other.magf_sum_of_square_weights();
		}
	}

	// Add the shear data

	// If the other's shear data isn't empty
	if(!other_shear_zero)
	{

		// If this is empty, simply copy everything
		if(this_shear_zero)
		{
			_shear_pair_count_ = other.shear_pair_count();

			_shear_R_mean_ = other.shear_R_mean();
			_shear_lens_m_mean_ = other.shear_lens_m_mean();
			_shear_lens_z_mean_ = other.shear_lens_z_mean();
			_shear_lens_mag_mean_ = other.lens_mag_mean();
			_shear_source_z_mean_ = other.source_z_mean();

			_delta_Sigma_t_mean_ = other.delta_Sigma_t_mean();
			_delta_Sigma_t_mean_square_ = other.delta_Sigma_t_mean_square();
			_delta_Sigma_x_mean_ = other.delta_Sigma_x_mean();
			_delta_Sigma_x_mean_square_ = other.delta_Sigma_x_mean_square();
			_shear_sum_of_weights_ = other.shear_sum_of_weights();
			_shear_sum_of_square_weights_ = other.shear_sum_of_square_weights();
		}
		else
		{
			_shear_pair_count_ += other.shear_pair_count();

			const auto o_shear_sum_of_weights = other.shear_sum_of_weights();

			const auto new_sum_of_weights = _shear_sum_of_weights_ + o_shear_sum_of_weights;

			_shear_R_mean_ = ( shear_R_mean()*_shear_sum_of_weights_ + other.shear_R_mean()*o_shear_sum_of_weights)/new_sum_of_weights;
			_shear_lens_m_mean_ = ( shear_lens_m_mean()*_shear_sum_of_weights_ + other.shear_lens_m_mean()*o_shear_sum_of_weights)/new_sum_of_weights;
			_shear_lens_z_mean_ = ( shear_lens_z_mean()*_shear_sum_of_weights_ + other.shear_lens_z_mean()*o_shear_sum_of_weights)/new_sum_of_weights;
			_shear_lens_mag_mean_ = ( shear_lens_mag_mean()*_shear_sum_of_weights_ + other.lens_mag_mean()*o_shear_sum_of_weights)/new_sum_of_weights;
			_shear_source_z_mean_ = ( shear_source_z_mean()*_shear_sum_of_weights_ + other.source_z_mean()*o_shear_sum_of_weights)/new_sum_of_weights;

			_delta_Sigma_t_mean_ = ( delta_Sigma_t_mean()*_shear_sum_of_weights_ + other.delta_Sigma_t_mean()*o_shear_sum_of_weights)
					/new_sum_of_weights;
			_delta_Sigma_t_mean_square_ = ( delta_Sigma_t_mean_square()*shear_sum_of_weights() +
					other.delta_Sigma_t_mean_square()*o_shear_sum_of_weights)
					/new_sum_of_weights;
			_delta_Sigma_x_mean_ = ( delta_Sigma_x_mean()*shear_sum_of_weights() + other.delta_Sigma_x_mean()*o_shear_sum_of_weights)
							/new_sum_of_weights;
			_delta_Sigma_x_mean_square_ = ( delta_Sigma_x_mean_square()*shear_sum_of_weights() +
					other.delta_Sigma_x_mean_square()*o_shear_sum_of_weights)
					/new_sum_of_weights;
			_shear_sum_of_weights_ = new_sum_of_weights;
			_shear_sum_of_square_weights_ += other.shear_sum_of_square_weights();
		}
	}

	return *this;
}

#endif // Combining summaries together

// Fix bad values
void pair_bin_summary::fixbad()
{
	IceBRG::fixbad(_R_min_);
	IceBRG::fixbad(_R_max_);
	IceBRG::fixbad(_m_min_);
	IceBRG::fixbad(_m_max_);
	IceBRG::fixbad(_z_min_);
	IceBRG::fixbad(_z_max_);
	IceBRG::fixbad(_mag_min_);
	IceBRG::fixbad(_mag_max_);

	IceBRG::fixbad(_shear_R_mean_);
	IceBRG::fixbad(_magf_R_mean_);

	IceBRG::fixbad(_shear_lens_m_mean_);
	IceBRG::fixbad(_magf_lens_m_mean_);
	IceBRG::fixbad(_shear_lens_z_mean_);
	IceBRG::fixbad(_magf_lens_z_mean_);
	IceBRG::fixbad(_shear_lens_mag_mean_);
	IceBRG::fixbad(_magf_lens_mag_mean_);

	IceBRG::fixbad(_shear_source_z_mean_);
	IceBRG::fixbad(_magf_source_z_mean_);

	IceBRG::fixbad(_shear_sum_of_weights_);
	IceBRG::fixbad(_magf_sum_of_weights_);
	IceBRG::fixbad(_shear_sum_of_square_weights_);
	IceBRG::fixbad(_magf_sum_of_square_weights_);

	IceBRG::fixbad(_delta_Sigma_t_mean_);
	IceBRG::fixbad(_delta_Sigma_x_mean_);
	IceBRG::fixbad(_delta_Sigma_t_mean_square_);
	IceBRG::fixbad(_delta_Sigma_x_mean_square_);

	IceBRG::fixbad(_mu_hat_);
	IceBRG::fixbad(_mu_square_hat_);
	IceBRG::fixbad(_area_);
	IceBRG::fixbad(_magf_lens_count_);
}

// Saving/loading data
#if(1)

void pair_bin_summary::save(std::ostream & out) const
{
	boost::archive::text_oarchive ar(out);
	ar << *this;
}
void pair_bin_summary::save(const std::string & filename) const
{
	std::ofstream out;
	open_file_output(out,filename);
	save(out);
}
void pair_bin_summary::save(std::ostream & out)
{
	fixbad();
	boost::archive::text_oarchive ar(out);
	ar << *this;
}
void pair_bin_summary::save(const std::string & filename)
{
	std::ofstream out;
	open_file_output(out,filename);
	save(out);
}
void pair_bin_summary::load(std::istream & in)
{
	boost::archive::text_iarchive ar(in);
	ar >> *this;
}
void pair_bin_summary::load(const std::string & filename)
{
	std::ifstream in;
	open_file_input(in,filename);
	load(in);
}

#endif



// Summary values
#if (1)

surface_density_type pair_bin_summary::shear_sigma_crit() const
{
	surface_density_type result(IceBRG::sigma_crit(shear_lens_z_mean(),shear_source_z_mean()));
	return result;
}

surface_density_type pair_bin_summary::magf_sigma_crit() const
{
	surface_density_type result(IceBRG::sigma_crit(magf_lens_z_mean(),magf_source_z_mean()));
	return result;
}

// Shear
#if (1)

surface_density_type pair_bin_summary::delta_Sigma_t_std() const
{
	return sqrt( _delta_Sigma_t_mean_square_ - square(_delta_Sigma_t_mean_) );
}
surface_density_type pair_bin_summary::delta_Sigma_x_std() const
{
	return sqrt( _delta_Sigma_x_mean_square_ - square(_delta_Sigma_x_mean_) );
}

surface_density_type pair_bin_summary::delta_Sigma_t_stderr() const
{
	if(_shear_pair_count_<2) return units_cast<surface_density_type>(std::numeric_limits<flt_type>::max());
	surface_density_type result = delta_Sigma_t_std()*std::sqrt(_shear_pair_count_/(shear_effective_pair_count()*(_shear_pair_count_-1)) );
	if(isgood(result)) return result;
	return 0;
}
surface_density_type pair_bin_summary::delta_Sigma_x_stderr() const
{
	if(_shear_pair_count_<2) return units_cast<surface_density_type>(std::numeric_limits<flt_type>::max());
	surface_density_type result = delta_Sigma_x_std()*std::sqrt(_shear_pair_count_/(shear_effective_pair_count()*(_shear_pair_count_-1)) );
	if(isgood(result)) return result;
	return 0;
}

flt_type pair_bin_summary::gamma_t_mean() const
{
	return delta_Sigma_t_mean()/shear_sigma_crit();
}
flt_type pair_bin_summary::gamma_x_mean() const
{
	return delta_Sigma_x_mean()/shear_sigma_crit();
}
flt_type pair_bin_summary::gamma_mean() const
{
	return std::sqrt(gamma_mean_square());
}
flt_type pair_bin_summary::gamma_mean_square() const
{
	return square(gamma_t_mean())+square(gamma_x_mean());

}

flt_type pair_bin_summary::gamma_t_stderr() const
{
	return delta_Sigma_t_stderr()/shear_sigma_crit();
}
flt_type pair_bin_summary::gamma_x_stderr() const
{
	return delta_Sigma_x_stderr()/shear_sigma_crit();
}
flt_type pair_bin_summary::gamma_stderr() const
{
	return quad_add_err(gamma_t_mean(),gamma_t_stderr(),gamma_x_mean(),gamma_x_stderr());
}
flt_type pair_bin_summary::gamma_square_stderr() const
{
	return 2*gamma_mean()*gamma_stderr();
}

surface_density_type pair_bin_summary::model_delta_Sigma_t(const flt_type & MLratio_1h, const mass_type & mean_group_mass, const flt_type & sat_frac) const
{
	surface_density_type res = model_1h_delta_Sigma_t(MLratio_1h) + model_offset_delta_Sigma_t(mean_group_mass,sat_frac);
	return res;
}
flt_type pair_bin_summary::model_gamma_t(const flt_type & MLratio_1h, const mass_type & mean_group_mass, const flt_type & sat_frac) const
{
	flt_type res = model_delta_Sigma_t(MLratio_1h,mean_group_mass,sat_frac)/shear_sigma_crit();
	return res;
}
surface_density_type pair_bin_summary::model_1h_delta_Sigma_t(const flt_type & MLratio) const
{
	surface_density_type res = _shear_model_profile(MLratio).Delta_Sigma(shear_R_mean());
	return res;
}
flt_type pair_bin_summary::model_1h_gamma_t(const flt_type & MLratio) const
{
	flt_type res = model_delta_Sigma_t(MLratio)/shear_sigma_crit();
	return res;
}
surface_density_type pair_bin_summary::model_offset_delta_Sigma_t(const mass_type & mean_group_mass, const flt_type & sat_frac) const
{
	surface_density_type res = sat_frac*IceBRG::lensing_tNFW_profile(mean_group_mass,shear_lens_z_mean()).quick_group_Delta_Sigma(shear_R_mean());
	return res;
}
flt_type pair_bin_summary::model_offset_gamma_t(const mass_type & mean_group_mass, const flt_type & sat_frac) const
{
	flt_type res = model_offset_delta_Sigma_t(mean_group_mass,sat_frac)/shear_sigma_crit();
	return res;
}

#endif // Shear

// Magnification
#if (1)

custom_unit_type<0,0,0,2,0> pair_bin_summary::area_per_lens() const
{
	return area()/(flt_type)magf_num_lenses();
}
flt_type pair_bin_summary::mu_std() const
{
	flt_type std_uncorrected = std::sqrt(mu_square_hat()-square(mu_hat()));
	flt_type correction = magf_sum_of_weights()/std::sqrt( square(magf_sum_of_weights()) - magf_sum_of_square_weights() );

	return correction * std_uncorrected;
}
flt_type pair_bin_summary::mu_stderr() const
{
	if(magf_num_lenses()<2) return std::numeric_limits<flt_type>::max();
	flt_type result = mu_std()/std::sqrt(magf_num_lenses()-1);
	if(isgood(result)) return result;
	return 0;
}
flt_type pair_bin_summary::kappa() const
{
	flt_type gms = _magf_gamma_t_mean_square();
	IceBRG::fixbad(gms);
	return 1.-std::sqrt(1/mu_hat()+gms);
}
flt_type pair_bin_summary::kappa_stderr() const
{
	flt_type gms = _magf_gamma_t_mean_square();
	flt_type gserr = _magf_gamma_t_square_stderr();
	IceBRG::fixbad(gms);
	IceBRG::fixbad(gserr);
	return sqrt_err(1/mu_hat()+gms,quad_add(mu_stderr(),gserr));
}

surface_density_type pair_bin_summary::Sigma() const
{
	surface_density_type res = kappa() * magf_sigma_crit();
	return res;
}
surface_density_type pair_bin_summary::Sigma_stderr() const
{
	surface_density_type res = kappa_stderr() * magf_sigma_crit();
	return res;
}

flt_type pair_bin_summary::model_mu(const flt_type & MLratio_1h, const mass_type & mean_group_mass, const flt_type & sat_frac) const
{
	flt_type res = 1./(square(1-model_kappa(MLratio_1h,mean_group_mass,sat_frac))+square(model_gamma_t(MLratio_1h,mean_group_mass,sat_frac)));
	return res;
}
flt_type pair_bin_summary::model_kappa(const flt_type & MLratio_1h, const mass_type & mean_group_mass, const flt_type & sat_frac) const
{
	flt_type res = model_1h_kappa(MLratio_1h) + model_offset_kappa(mean_group_mass,sat_frac);
	return res;
}
surface_density_type pair_bin_summary::model_Sigma(const flt_type & MLratio_1h, const mass_type & mean_group_mass, const flt_type & sat_frac) const
{
	surface_density_type res = model_1h_Sigma(MLratio_1h) + model_offset_Sigma(mean_group_mass,sat_frac);
	return res;
}

flt_type pair_bin_summary::model_1h_mu(const flt_type & MLratio_1h) const
{
	flt_type res = 1./(square(1-model_1h_kappa(MLratio_1h))+square(model_1h_gamma_t(MLratio_1h)));
	return res;
}
flt_type pair_bin_summary::model_1h_kappa(const flt_type & MLratio) const
{
	flt_type res = model_1h_Sigma(MLratio)/magf_sigma_crit();
	return res;
}
surface_density_type pair_bin_summary::model_1h_Sigma(const flt_type & MLratio) const
{
	surface_density_type res = _magf_model_profile(MLratio).proj_dens(magf_R_mean());
	return res;
}
flt_type pair_bin_summary::model_offset_mu(const mass_type & mean_group_mass, const flt_type & sat_frac) const
{
	flt_type res = 1./(square(1-model_offset_kappa(mean_group_mass,sat_frac))+square(model_offset_gamma_t(mean_group_mass,sat_frac)));
	return res;
}
flt_type pair_bin_summary::model_offset_kappa(const mass_type & mean_group_mass, const flt_type & sat_frac) const
{
	flt_type res = model_offset_Sigma(mean_group_mass,sat_frac)/magf_sigma_crit();
	return res;
}
surface_density_type pair_bin_summary::model_offset_Sigma(const mass_type & mean_group_mass, const flt_type & sat_frac) const
{
	surface_density_type res = sat_frac*IceBRG::lensing_tNFW_profile(mean_group_mass,magf_lens_z_mean()).quick_group_Sigma(magf_R_mean());
	return res;
}

#endif // Magnification

#endif

} // end namespace IceBRG
