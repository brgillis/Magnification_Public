/**********************************************************************\
 @file pair_bin_summary.h
 ------------------

 A class representing a bin of lens-source pairs, which can be used for
 calculating statistics on the contained pairs. This base class only
 includes the summary data.

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

// body file: pair_bin.cpp
#ifndef _BRG_PAIR_BIN_SUMMARY_H_INCLUDED_
#define _BRG_PAIR_BIN_SUMMARY_H_INCLUDED_

#include <utility>
#include <vector>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/access.hpp>

#include "IceBRG_main/units/units.hpp"

#include "IceBRG_main/common.h"

#include "IceBRG_main/container/table_typedefs.hpp"
#include "IceBRG_main/vector/summary_functions.hpp"

#include "IceBRG_lensing/lensing_tNFW_profile.h"

namespace IceBRG {

// Forward-declare pair_bin
class pair_bin;

/**
 *
 */
class pair_bin_summary {
private:

	// Bin limits
#if(1)

	distance_type _R_min_, _R_max_;
	mass_type _m_min_, _m_max_;
	flt_type _z_min_, _z_max_;
	flt_type _mag_min_, _mag_max_;

#endif // Bin limits

	// Summary data
#if (1)

	ssize_t _shear_pair_count_;
	ssize_t _magf_pair_count_;

	distance_type _shear_R_mean_;
	distance_type _magf_R_mean_;

	mass_type _shear_lens_m_mean_;
	mass_type _magf_lens_m_mean_;
	flt_type _shear_lens_z_mean_;
	flt_type _magf_lens_z_mean_;
	flt_type _shear_lens_mag_mean_;
	flt_type _magf_lens_mag_mean_;

	flt_type _shear_source_z_mean_;
	flt_type _magf_source_z_mean_;

	flt_type _shear_sum_of_weights_;
	flt_type _magf_sum_of_weights_;
	flt_type _shear_sum_of_square_weights_;
	flt_type _magf_sum_of_square_weights_;
	surface_density_type _delta_Sigma_t_mean_, _delta_Sigma_x_mean_;
	custom_unit_type<-4,0,2,0,0> _delta_Sigma_t_mean_square_, _delta_Sigma_x_mean_square_;

	flt_type _mu_hat_;
	flt_type _mu_square_hat_;
	custom_unit_type<0,0,0,2,0> _area_;
	ssize_t _magf_lens_count_;

#endif

	// Serialization (to save it to a file)
#if(1)
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const int_type version)
	{
		ar & _R_min_ & _R_max_;
		ar & _m_min_ & _m_max_;
		ar & _z_min_ & _z_max_;
		ar & _mag_min_ & _mag_max_;

		ar & _shear_pair_count_;
		ar & _magf_pair_count_;

		ar & _shear_R_mean_;
		ar & _magf_R_mean_;

		ar & _shear_lens_m_mean_;
		ar & _magf_lens_m_mean_;
		ar & _shear_lens_z_mean_;
		ar & _magf_lens_z_mean_;
		ar & _shear_lens_mag_mean_;
		ar & _magf_lens_mag_mean_;

		ar & _shear_source_z_mean_;
		ar & _magf_source_z_mean_;

		ar & _shear_sum_of_weights_;
		ar & _magf_sum_of_weights_;
		ar & _shear_sum_of_square_weights_;
		ar & _magf_sum_of_square_weights_;
		ar & _delta_Sigma_t_mean_ & _delta_Sigma_x_mean_;
		ar & _delta_Sigma_t_mean_square_ & _delta_Sigma_x_mean_square_;

		ar & _mu_hat_;
		ar & _mu_square_hat_;
		ar & _area_;
		ar & _magf_lens_count_;
	}
#endif

	flt_type _magf_gamma_t_mean() const;
	flt_type _magf_gamma_t_mean_square() const;
	flt_type _magf_gamma_x_mean() const;
	flt_type _magf_gamma_x_mean_square() const;
	flt_type _magf_gamma_mean() const;
	flt_type _magf_gamma_mean_square() const;

	flt_type _magf_gamma_t_stderr() const;
	flt_type _magf_gamma_t_square_stderr() const;
	flt_type _magf_gamma_x_stderr() const;
	flt_type _magf_gamma_x_square_stderr() const;
	flt_type _magf_gamma_stderr() const;
	flt_type _magf_gamma_square_stderr() const;

	lensing_tNFW_profile _shear_model_profile(const flt_type & MLratio = 50.) const;
	lensing_tNFW_profile _magf_model_profile(const flt_type & MLratio = 50.) const;

protected:

	virtual void _uncache_values()
	{
	}

public:

	// Constructors and destructor
#if(1)
	pair_bin_summary( const distance_type & init_R_min=0, const distance_type & init_R_max=0,
			const mass_type & init_m_min=0*kg, const mass_type & init_m_max=0*kg,
			const flt_type & init_z_min=0, const flt_type & init_z_max=0,
			const flt_type & init_mag_min=0, const flt_type & init_mag_max=0 );
	pair_bin_summary( const pair_bin & bin);
	pair_bin_summary( std::istream & in );
	pair_bin_summary( std::string & filename );
	pair_bin_summary & operator=(const pair_bin & bin)
	{
		*this = pair_bin_summary(bin);
		return *this;
	}
	virtual ~pair_bin_summary()
	{
	}
#endif

	// Setting bin limits
#if(1)

	template <typename T1, typename T2>
	void set_R_limits(T1&& R_min, T2&& R_max)
	{
		_R_min_ = std::forward<T1>(R_min);
		_R_max_ = std::forward<T2>(R_max);
	}
	template <typename T1, typename T2>
	void set_m_limits(T1&& m_min, T2&& m_max)
	{
		_m_min_ = std::forward<T1>(m_min);
		_m_max_ = std::forward<T2>(m_max);
	}
	template <typename T1, typename T2>
	void set_z_limits(T1&& z_min, T2&& z_max)
	{
		_z_min_ = std::forward<T1>(z_min);
		_z_max_ = std::forward<T2>(z_max);
	}
	template <typename T1, typename T2>
	void set_mag_limits(T1&& mag_min, T2&& mag_max)
	{
		_mag_min_ = std::forward<T1>(mag_min);
		_mag_max_ = std::forward<T2>(mag_max);
	}

	template <typename T1, typename T2,
				typename T3, typename T4,
				typename T5, typename T6,
				typename T7, typename T8>
	void set_limits(T1&& R_min, T2&& R_max,
			T3&& m_min, T4&& m_max,
			T5&& z_min, T6&& z_max,
			T7&& mag_min, T8&& mag_max)
	{
		set_R_limits(std::forward<T1>(R_min),std::forward<T2>(R_max));
		set_m_limits(std::forward<T3>(m_min),std::forward<T4>(m_max));
		set_z_limits(std::forward<T5>(z_min),std::forward<T6>(z_max));
		set_mag_limits(std::forward<T7>(mag_min),std::forward<T8>(mag_max));
	}


#endif

	// Adding and clearing data
#if(1)

	virtual void clear();

#endif

	// Fix bad values
	void fixbad();

	// General statistics accessors
#if(1)

	// Count
	virtual ssize_t pair_count() const
	{
		return _shear_pair_count_;
	}
	virtual ssize_t shear_pair_count() const
	{
		return _shear_pair_count_;
	}
	virtual ssize_t magf_pair_count() const
	{
		return _magf_pair_count_;
	}
	// Effective count
	virtual flt_type effective_pair_count() const
	{
		return shear_effective_pair_count();
	}
	virtual flt_type shear_effective_pair_count() const
	{
		return square(shear_sum_of_weights())/shear_sum_of_square_weights();
	}
	// Sum of weights
	virtual flt_type sum_of_weights() const
	{
		return _shear_sum_of_weights_;
	}
	virtual flt_type shear_sum_of_weights() const
	{
		return _shear_sum_of_weights_;
	}
	virtual flt_type magf_sum_of_weights() const
	{
		return _magf_sum_of_weights_;
	}
	// Sum of square weights
	virtual flt_type sum_of_square_weights() const
	{
		return _shear_sum_of_square_weights_;
	}
	virtual flt_type shear_sum_of_square_weights() const
	{
		return _shear_sum_of_square_weights_;
	}
	virtual flt_type magf_sum_of_square_weights() const
	{
		return _magf_sum_of_square_weights_;
	}

#endif

	// Limits and means accessors
#if(1)

	const distance_type & R_min() const
	{
		return _R_min_;
	}
	const distance_type & R_max() const
	{
		return _R_max_;
	}
	distance_type R_mid() const
	{
		return (_R_max_+_R_min_)/2.;
	}
	distance_type R_amid() const
	{
		using std::sqrt;
		return sqrt((square(_R_max_)+square(_R_min_))/2.);
	}

	virtual distance_type R_mean() const
	{
		return shear_R_mean();
	}
	virtual distance_type shear_R_mean() const
	{
		return _shear_R_mean_;
	}
	virtual distance_type magf_R_mean() const
	{
		return _magf_R_mean_;
	}

	const mass_type & m_min() const
	{
		return _m_min_;
	}
	const mass_type & m_max() const
	{
		return _m_max_;
	}
	mass_type m_mid() const
	{
		return (_m_max_+_m_min_)/2.;
	}
	virtual mass_type lens_m_mean() const
	{
		return shear_lens_m_mean();
	}
	virtual mass_type shear_lens_m_mean() const
	{
		return _shear_lens_m_mean_;
	}
	virtual mass_type magf_lens_m_mean() const
	{
		return _magf_lens_m_mean_;
	}

	flt_type z_min() const
	{
		return _z_min_;
	}
	flt_type z_max() const
	{
		return _z_max_;
	}
	flt_type z_mid() const
	{
		return (_z_max_+_z_min_)/2.;
	}
	virtual flt_type lens_z_mean() const
	{
		return shear_lens_z_mean();
	}
	virtual flt_type shear_lens_z_mean() const
	{
		return _shear_lens_z_mean_;
	}
	virtual flt_type magf_lens_z_mean() const
	{
		return _magf_lens_z_mean_;
	}

	virtual flt_type source_z_mean() const
	{
		return shear_source_z_mean();
	}
	virtual flt_type shear_source_z_mean() const
	{
		return _shear_source_z_mean_;
	}
	virtual flt_type magf_source_z_mean() const
	{
		return _magf_source_z_mean_;
	}

	flt_type mag_min() const
	{
		return _mag_min_;
	}
	flt_type mag_max() const
	{
		return _mag_max_;
	}
	flt_type mag_mid() const
	{
		return (_mag_max_+_mag_min_)/2.;
	}

	virtual flt_type lens_mag_mean() const
	{
		return shear_lens_mag_mean();
	}
	virtual flt_type shear_lens_mag_mean() const
	{
		return _shear_lens_mag_mean_;
	}
	virtual flt_type magf_lens_mag_mean() const
	{
		return _magf_lens_mag_mean_;
	}

#endif

	// Summary values
#if (1)

	surface_density_type shear_sigma_crit() const;
	surface_density_type magf_sigma_crit() const;

	// Shear
#if (1)
	virtual surface_density_type delta_Sigma_t_mean() const
	{
		return _delta_Sigma_t_mean_;
	}
	virtual surface_density_type delta_Sigma_x_mean() const
	{
		return _delta_Sigma_x_mean_;
	}

	virtual custom_unit_type<-4,0,2,0,0> delta_Sigma_t_mean_square() const
	{
		return _delta_Sigma_t_mean_square_;
	}
	virtual custom_unit_type<-4,0,2,0,0> delta_Sigma_x_mean_square() const
	{
		return _delta_Sigma_x_mean_square_;
	}

	virtual surface_density_type delta_Sigma_t_std() const;
	virtual surface_density_type delta_Sigma_x_std() const;

	virtual surface_density_type delta_Sigma_t_stderr() const;
	virtual surface_density_type delta_Sigma_x_stderr() const;

	flt_type gamma_t_mean() const;
	flt_type gamma_x_mean() const;
	flt_type gamma_mean() const;
	flt_type gamma_mean_square() const;

	flt_type gamma_t_stderr() const;
	flt_type gamma_x_stderr() const;
	flt_type gamma_stderr() const;
	flt_type gamma_square_stderr() const;

	surface_density_type model_delta_Sigma_t(const flt_type & MLratio_1h = 50.,
			const mass_type & mean_group_mass = 1e14*unitconv::Msuntokg*kg,
			const flt_type & sat_frac = 0.2) const;
	flt_type model_gamma_t(const flt_type & MLratio_1h = 50.,
			const mass_type & mean_group_mass = 1e14*unitconv::Msuntokg*kg,
			const flt_type & sat_frac = 0.2) const;

	surface_density_type model_1h_delta_Sigma_t(const flt_type & MLratio_1h = 50.) const;
	flt_type model_1h_gamma_t(const flt_type & MLratio_1h = 50.) const;

	surface_density_type model_offset_delta_Sigma_t(
			const mass_type & mean_group_mass = 1e14*unitconv::Msuntokg*kg,
			const flt_type & sat_frac = 0.2) const;
	flt_type model_offset_gamma_t(const mass_type & mean_group_mass = 1e14*unitconv::Msuntokg*kg,
			const flt_type & sat_frac = 0.2) const;

#endif // Shear

	// Magnification
#if (1)

	custom_unit_type<0,0,0,2,0> area_per_lens() const;
	virtual custom_unit_type<0,0,0,2,0> area() const
	{
		return _area_;
	}
	virtual ssize_t num_lenses() const
	{
		return _magf_lens_count_;
	}
	virtual flt_type magf_effective_num_lenses() const
	{
		return square(magf_sum_of_weights())/magf_sum_of_square_weights();
	}
	virtual ssize_t magf_num_lenses() const
	{
		return _magf_lens_count_;
	}
	virtual flt_type mu_hat() const
	{
		return _mu_hat_;
	}
	virtual flt_type mu_square_hat() const
	{
		return _mu_square_hat_;
	}
	virtual flt_type mu_W() const
	{
		return magf_sum_of_weights();
	}
	flt_type mu_std() const;
	flt_type mu_stderr() const;
	flt_type kappa() const;
	flt_type kappa_stderr() const;

	surface_density_type Sigma() const;
	surface_density_type Sigma_stderr() const;

	flt_type model_mu(const flt_type & MLratio_1h = 50.,
			const mass_type & mean_group_mass = 1e14*unitconv::Msuntokg*kg,
			const flt_type & sat_frac = 0.2) const;
	flt_type model_kappa(const flt_type & MLratio_1h = 50.,
			const mass_type & mean_group_mass = 1e14*unitconv::Msuntokg*kg,
			const flt_type & sat_frac = 0.2) const;
	surface_density_type model_Sigma(const flt_type & MLratio_1h = 50.,
			const mass_type & mean_group_mass = 1e14*unitconv::Msuntokg*kg,
			const flt_type & sat_frac = 0.2) const;

	flt_type model_1h_mu(const flt_type & MLratio_1h = 50.) const;
	flt_type model_1h_kappa(const flt_type & MLratio_1h = 50.) const;
	surface_density_type model_1h_Sigma(const flt_type & MLratio_1h = 50.) const;

	flt_type model_offset_mu(const mass_type & mean_group_mass = 1e14*unitconv::Msuntokg*kg,
			const flt_type & sat_frac = 0.2) const;
	flt_type model_offset_kappa(const mass_type & mean_group_mass = 1e14*unitconv::Msuntokg*kg,
			const flt_type & sat_frac = 0.2) const;
	surface_density_type model_offset_Sigma(const mass_type & mean_group_mass = 1e14*unitconv::Msuntokg*kg,
			const flt_type & sat_frac = 0.2) const;

#endif // Magnification

#endif

	// Combining summaries together
#if(1)

	pair_bin_summary & operator+=( const pair_bin_summary & other );
	pair_bin_summary operator+( pair_bin_summary other ) const
	{
		return other += *this;
	}

#endif // Combining summaries together

	// Saving/loading data
#if(1)

	void save(std::ostream &) const;
	void save(const std::string &) const;
	void save(std::ostream &);
	void save(const std::string &);
	void load(std::istream &);
	void load(const std::string &);

#endif
};

} // end namespace IceBRG

#endif // _BRG_PAIR_BIN_H_INCLUDED_
