/**********************************************************************\
 @file pair_bin.h
 ------------------

 A class representing a bin of lens-source pairs, which can be used for
 calculating statistics on the contained pairs.

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
#ifndef _BRG_PAIR_BIN_H_INCLUDED_
#define _BRG_PAIR_BIN_H_INCLUDED_

#include <vector>

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/count.hpp>
#include <boost/accumulators/statistics/weighted_mean.hpp>
#include <boost/accumulators/statistics/weighted_moment.hpp>
#include <boost/accumulators/statistics/weighted_variance.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/container/flat_map.hpp>
#include <boost/container/flat_set.hpp>

#include "IceBRG_main/common.h"

#include "IceBRG_main/math/statistics/effective_count.hpp"
#include "IceBRG_main/math/statistics/error_of_weighted_mean.hpp"
#include "IceBRG_main/math/statistics/mean_weight.hpp"
#include "IceBRG_main/math/statistics/statistic_extractors.hpp"
#include "IceBRG_main/units/units.hpp"
#include "IceBRG_main/utility.hpp"
#include "IceBRG_main/vector/limit_vector.hpp"
#include "IceBRG_main/vector/summary_functions.hpp"

#include "IceBRG_lensing/lens_source_pair.h"
#include "IceBRG_lensing/pair_bin_summary.h"


namespace IceBRG {

// lens_id struct
#if(1)
struct lens_id
{
	ssize_t id;
	mass_type m;
	flt_type z;
	flt_type mag;
	flt_type weight;

	// Data on the unmasked fraction of annuli
#if(1)

	IceBRG::limit_vector<distance_type> unmasked_frac_bin_limits;
	std::vector<flt_type> unmasked_fracs;

	flt_type unmasked_frac(const distance_type & R_proj) const;

#endif

	lens_id(const ssize_t & id, const mass_type & m, const flt_type & z, const flt_type & mag,
			const std::vector<distance_type> & unmasked_frac_bin_limits,
			const std::vector<flt_type> & unmasked_fracs,
			const flt_type & weight=1)
	: id(id),
	  m(m),
	  z(z),
	  mag(mag),
	  weight(weight),
	  unmasked_frac_bin_limits(unmasked_frac_bin_limits),
	  unmasked_fracs(unmasked_fracs)
	{
	}
};
#endif

/**
 *
 */
class pair_bin: public pair_bin_summary {
private:

	template <typename T>
	using bin_stat_vec_t = boost::accumulators::accumulator_set<T,
			boost::accumulators::stats<
				boost::accumulators::tag::count,
				boost::accumulators::tag::weighted_mean,
				boost::accumulators::tag::mean_weight >,
				flt_type >;
	template <typename T>
	using stat_vec_t = boost::accumulators::accumulator_set<T,
			boost::accumulators::stats<
				boost::accumulators::tag::count,
				boost::accumulators::tag::effective_count,
				boost::accumulators::tag::error_of_weighted_mean,
				boost::accumulators::tag::weighted_mean,
				boost::accumulators::tag::weighted_moment<2>,
				boost::accumulators::tag::weighted_variance >,
				flt_type >;

	// Pair data
#if(1)
	bin_stat_vec_t<flt_type> _magf_R_values_;
	bin_stat_vec_t<flt_type> _shear_R_values_;

	bin_stat_vec_t<flt_type> _magf_lens_m_values_;
	bin_stat_vec_t<flt_type> _shear_lens_m_values_;
	bin_stat_vec_t<flt_type> _magf_lens_z_values_;
	bin_stat_vec_t<flt_type> _shear_lens_z_values_;
	bin_stat_vec_t<flt_type> _magf_lens_mag_values_;
	bin_stat_vec_t<flt_type> _shear_lens_mag_values_;

	bin_stat_vec_t<flt_type> _magf_source_z_values_;
	bin_stat_vec_t<flt_type> _shear_source_z_values_;

	bin_stat_vec_t<flt_type> _magf_unmasked_fracs_;
	bin_stat_vec_t<flt_type> _mu_obs_values_;

	stat_vec_t<flt_type> _delta_Sigma_t_values_;
	stat_vec_t<flt_type> _delta_Sigma_x_values_;

	boost::container::flat_set<ssize_t> _distinct_lens_ids_;
	boost::container::flat_map<ssize_t,flt_type> _lens_weights_;

	mutable flt_type _mu_hat_cached_value_;
	mutable flt_type _mu_W_cached_value_;

	flt_type _z_buffer_;
	static constexpr flt_type _z_buffer_default_value_ = 0.1;

#endif // Pair data

protected:
	void _uncache_values();

public:

	// Constructors and destructor
#if(1)
	pair_bin( const distance_type & init_R_min=0, const distance_type & init_R_max=0,
			const mass_type & init_m_min=0, const mass_type & init_m_max=0,
			const flt_type & init_z_min=0, const flt_type & init_z_max=0,
			const flt_type & init_mag_min=0, const flt_type & init_mag_max=0,
			const flt_type & init_z_buffer=_z_buffer_default_value_);
	virtual ~pair_bin()
	{
	}
#endif

	// Setting and accessing z_buffer
#if(1)
	void set_z_buffer(const flt_type & new_z_buffer)
	{
		_z_buffer_ = new_z_buffer;
	}
	flt_type z_buffer() const
	{
		return _z_buffer_;
	}
#endif

	// Adding and clearing data
#if(1)

	void add_pair( const lens_source_pair & new_pair);
	void add_lens( const lens_id & lens );
	virtual void clear() override;

#endif

	// Count
#if(1)
	virtual ssize_t pair_count() const override
	{
		return shear_pair_count();
	}
	virtual ssize_t shear_pair_count() const override
	{
		return extract_count(_shear_R_values_);
	}
	virtual ssize_t magf_pair_count() const override
	{
		return extract_count(_magf_R_values_);
	}
	virtual flt_type effective_pair_count() const override
	{
		return shear_effective_pair_count();
	}
	virtual flt_type shear_effective_pair_count() const override
	{
		return safe_extract_effective_count(_delta_Sigma_t_values_);
	}
	virtual flt_type sum_of_weights() const override
	{
		return shear_sum_of_weights();
	}
	virtual flt_type shear_sum_of_weights() const override
	{
		return safe_extract_sum_of_weights(_delta_Sigma_t_values_);
	}
	virtual flt_type magf_sum_of_weights() const override;
	virtual flt_type sum_of_square_weights() const override
	{
		return shear_sum_of_square_weights();
	}
	virtual flt_type shear_sum_of_square_weights() const override
	{
		return safe_extract_sum_of_square_weights(_delta_Sigma_t_values_);
	}
	virtual flt_type magf_sum_of_square_weights() const override;
	virtual ssize_t num_lenses() const override
	{
		return magf_num_lenses();
	}
	virtual ssize_t magf_num_lenses() const override
	{
		return ssize(_distinct_lens_ids_);
	}
#endif

	// Limits and means accessors
#if(1)

	virtual distance_type R_mean() const override
	{
		return shear_R_mean();
	}
	virtual distance_type shear_R_mean() const override
	{
		return safe_extract_weighted_mean(_shear_R_values_)*m;
	}
	virtual distance_type magf_R_mean() const override
	{
		return safe_extract_weighted_mean(_magf_R_values_)*m;
	}

	virtual mass_type lens_m_mean() const override
	{
		return shear_lens_m_mean();
	}
	virtual mass_type shear_lens_m_mean() const override
	{
		return safe_extract_weighted_mean(_shear_lens_m_values_)*kg;
	}
	virtual mass_type magf_lens_m_mean() const override
	{
		return safe_extract_weighted_mean(_magf_lens_m_values_)*kg;
	}

	virtual flt_type lens_z_mean() const override
	{
		return shear_lens_z_mean();
	}
	virtual flt_type shear_lens_z_mean() const override
	{
		return safe_extract_weighted_mean(_shear_lens_z_values_);
	}
	virtual flt_type magf_lens_z_mean() const override
	{
		return safe_extract_weighted_mean(_magf_lens_z_values_);
	}

	virtual flt_type lens_mag_mean() const override
	{
		return shear_lens_mag_mean();
	}
	virtual flt_type shear_lens_mag_mean() const override
	{
		return safe_extract_weighted_mean(_shear_lens_mag_values_);
	}
	virtual flt_type magf_lens_mag_mean() const override
	{
		return safe_extract_weighted_mean(_magf_lens_mag_values_);
	}

	virtual flt_type source_z_mean() const override
	{
		return shear_source_z_mean();
	}
	virtual flt_type shear_source_z_mean() const override
	{
		return safe_extract_weighted_mean(_shear_source_z_values_);
	}
	virtual flt_type magf_source_z_mean() const override
	{
		return safe_extract_weighted_mean(_magf_source_z_values_);
	}

	flt_type unmasked_frac() const
	{
		return safe_extract_weighted_mean(_magf_unmasked_fracs_);
	}

#endif

	// Calculations on stored values
#if (1)

	virtual custom_unit_type<0,0,0,2,0> area() const override;

	virtual surface_density_type delta_Sigma_t_mean() const override;
	virtual surface_density_type delta_Sigma_x_mean() const override;

	virtual custom_unit_type<-4,0,2,0,0> delta_Sigma_t_mean_square() const override;
	virtual custom_unit_type<-4,0,2,0,0> delta_Sigma_x_mean_square() const override;

	virtual surface_density_type delta_Sigma_t_std() const override;
	virtual surface_density_type delta_Sigma_x_std() const override;

	virtual surface_density_type delta_Sigma_t_stderr() const override;
	virtual surface_density_type delta_Sigma_x_stderr() const override;

	virtual flt_type mu_hat() const override;
	virtual flt_type mu_square_hat() const override;
	virtual flt_type mu_W() const override;

#endif
};

} // end namespace IceBRG

#endif // _BRG_PAIR_BIN_H_INCLUDED_
