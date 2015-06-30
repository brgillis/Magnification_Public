/**********************************************************************\
 @file pair_bins_summary.cpp
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

#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include "IceBRG_main/common.h"

#include "IceBRG_main/file_access/ascii_table_map.hpp"
#include "IceBRG_main/file_access/open_file.hpp"
#include "IceBRG_main/math/misc_math.hpp"
#include "IceBRG_main/units/apply_unitconvs.hpp"
#include "IceBRG_main/units/units.hpp"
#include "IceBRG_main/utility.hpp"
#include "IceBRG_main/vector/make_vector.hpp"
#include "IceBRG_main/vector/limit_vector.hpp"
#include "IceBRG_main/vector/summary_functions.hpp"

#include "IceBRG_lensing/pair_bin.h"
#include "IceBRG_lensing/pair_binner.h"

#include "pair_bins_summary.h"

namespace IceBRG {

	// Private methods
#if(1)

void pair_bins_summary::_check_limits()
{
	// Now check they're all monotonically increasing
	// Note that this function returns false if they're too small as well
	if(!is_monotonically_increasing(_R_bin_limits_))
	{
		_valid_limits_ = false;
		return;
	}
	if(!is_monotonically_increasing(_m_bin_limits_))
	{
		_valid_limits_ = false;
		return;
	}
	if(!is_monotonically_increasing(_z_bin_limits_))
	{
		_valid_limits_ = false;
		return;
	}
	if(!is_monotonically_increasing(_mag_bin_limits_))
	{
		_valid_limits_ = false;
		return;
	}

	_valid_limits_ = true;
}

	// Private implementations of set/clear methods
#if(1)

// Set specific limits through a limits vector
void pair_bins_summary::_set_R_limits(IceBRG::limit_vector< distance_type > R_bin_limits)
{
	_R_bin_limits_ = R_bin_limits;
}
void pair_bins_summary::_set_m_limits(IceBRG::limit_vector< mass_type > m_bin_limits)
{
	_m_bin_limits_ = m_bin_limits;
}
void pair_bins_summary::_set_z_limits(IceBRG::limit_vector< flt_type > z_bin_limits)
{
	_z_bin_limits_ = z_bin_limits;
}
void pair_bins_summary::_set_mag_limits(IceBRG::limit_vector< flt_type > mag_bin_limits)
{
	_mag_bin_limits_ = mag_bin_limits;
}

// Set specific limits through a linear spacing
void pair_bins_summary::_set_linear_R_limits(const distance_type & R_min,
		const distance_type & R_max,
		const ssize_t & R_bins)
{
	_R_bin_limits_.reconstruct(IceBRG::limit_vector<distance_type>::type::LINEAR, R_min, R_max, R_bins);
}
void pair_bins_summary::_set_linear_m_limits(const mass_type & m_min,
		const mass_type & m_max,
		const ssize_t & m_bins)
{
	_m_bin_limits_.reconstruct(IceBRG::limit_vector<mass_type>::type::LINEAR, m_min, m_max, m_bins);
}
void pair_bins_summary::_set_linear_z_limits(const flt_type & z_min,
		const flt_type & z_max,
		const ssize_t & z_bins)
{
	_z_bin_limits_.reconstruct(IceBRG::limit_vector<flt_type>::type::LINEAR, z_min, z_max, z_bins);
}
void pair_bins_summary::_set_linear_mag_limits(const flt_type & mag_min,
		const flt_type & mag_max,
		const ssize_t & mag_bins)
{
	_mag_bin_limits_.reconstruct(IceBRG::limit_vector<flt_type>::type::LINEAR, mag_min, mag_max, mag_bins);
}

// Set specific limits through a log spacing
void pair_bins_summary::_set_log_R_limits(const distance_type & R_min,
		const distance_type & R_max,
		const ssize_t & R_num_bins)
{
	_R_bin_limits_.reconstruct(IceBRG::limit_vector<distance_type>::type::LOG, R_min, R_max, R_num_bins);
}
void pair_bins_summary::_set_log_m_limits(const mass_type & m_min,
		const mass_type & m_max,
		const ssize_t & m_num_bins)
{
	_m_bin_limits_.reconstruct(IceBRG::limit_vector<mass_type>::type::LOG, m_min, m_max, m_num_bins);
}
void pair_bins_summary::_set_log_z_limits(const flt_type & z_min,
		const flt_type & z_max,
		const ssize_t & z_num_bins)
{
	_z_bin_limits_.reconstruct(IceBRG::limit_vector<flt_type>::type::LOG, z_min, z_max, z_num_bins);
}
void pair_bins_summary::_set_log_mag_limits(const flt_type & mag_min,
		const flt_type & mag_max,
		const ssize_t & mag_num_bins)
{
	_mag_bin_limits_.reconstruct(IceBRG::limit_vector<flt_type>::type::LOG, mag_min, mag_max, mag_num_bins);
}

// Clear limits. That is, make them unbound - one bin from neg infinity to pos infinity
void pair_bins_summary::_clear_R_limits()
{
	_R_bin_limits_.clear();
}
void pair_bins_summary::_clear_m_limits()
{
	_m_bin_limits_.clear();
}
void pair_bins_summary::_clear_z_limits()
{
	_z_bin_limits_.clear();
}
void pair_bins_summary::_clear_mag_limits()
{
	_mag_bin_limits_.clear();
}

#endif // Private implementations of set/clear methods

// Reconstruct - make the _pair_bins_summaries_ structure
void pair_bins_summary::_reconstruct() const
{
	// Set up the construction function
	auto func = [=] (const ssize_t & R_i, const ssize_t & m_i, const ssize_t & z_i, const ssize_t & mag_i)
	{
		return pair_bin(R_limits().lower_limit(R_i),R_limits().upper_limit(R_i),
						m_limits().lower_limit(m_i),m_limits().upper_limit(m_i),
						z_limits().lower_limit(z_i),z_limits().upper_limit(z_i),
						mag_limits().lower_limit(mag_i),mag_limits().upper_limit(mag_i));
	};
	make_vector_function(_pair_bin_summaries_,func,R_limits().num_bins(),m_limits().num_bins(),z_limits().num_bins(),mag_limits().num_bins());
}

#endif // Private methods

// Constructors
#if(1)

// Set limits by vectors
pair_bins_summary::pair_bins_summary(IceBRG::limit_vector< distance_type > R_bin_limits,
		IceBRG::limit_vector< mass_type > m_bin_limits,
		IceBRG::limit_vector< flt_type > z_bin_limits,
		IceBRG::limit_vector< flt_type > mag_bin_limits)
:	_R_bin_limits_(R_bin_limits),
 	_m_bin_limits_(m_bin_limits),
 	_z_bin_limits_(z_bin_limits),
 	_mag_bin_limits_(mag_bin_limits),
 	_valid_limits_(false)
{
	_check_limits();
}

// Set limits by min, max, and step
pair_bins_summary::pair_bins_summary(const distance_type & R_min,
				const distance_type & R_max,
				const ssize_t & R_bins,
				const mass_type & m_min,
				const mass_type & m_max,
				const ssize_t & m_bins,
				const flt_type & z_min,
				const flt_type & z_max,
				const ssize_t & z_bins,
				const flt_type & mag_min,
				const flt_type & mag_max,
				const ssize_t & mag_bins)
:	_R_bin_limits_(IceBRG::limit_vector<distance_type>::type::LINEAR,R_min,R_max,R_bins),
 	_m_bin_limits_(IceBRG::limit_vector<mass_type>::type::LINEAR,m_min,m_max,m_bins),
 	_z_bin_limits_(IceBRG::limit_vector<flt_type>::type::LINEAR,z_min,z_max,z_bins),
 	_mag_bin_limits_(IceBRG::limit_vector<flt_type>::type::LINEAR,mag_min,mag_max,mag_bins),
 	_valid_limits_(false)
{
	_check_limits();
}

// Load from archive
pair_bins_summary::pair_bins_summary( std::istream & in )
{
	load(in);
}
pair_bins_summary::pair_bins_summary( const std::string & filename )
{
	load(filename);
}

// Construct from pair_bins
pair_bins_summary::pair_bins_summary( const pair_binner & bins)
:	_R_bin_limits_(bins.R_limits()),
 	_m_bin_limits_(bins.m_limits()),
 	_z_bin_limits_(bins.z_limits()),
 	_mag_bin_limits_(bins.mag_limits()),
 	_valid_limits_(false)
{
	_check_limits();
	if(_valid_limits_)
	{
		bins.sort();
		_pair_bin_summaries_ = bins.pair_bin_summaries();
	}
}

#endif // Constructors

// Set/change limits
#if(1)

// Set specific limits through a limits vector
void pair_bins_summary::set_R_limits(IceBRG::limit_vector< distance_type > R_bin_limits)
{
	_set_R_limits(R_bin_limits);
	_check_limits();
}
void pair_bins_summary::set_m_limits(IceBRG::limit_vector< mass_type > m_bin_limits)
{
	_set_m_limits(m_bin_limits);
	_check_limits();
}
void pair_bins_summary::set_z_limits(IceBRG::limit_vector< flt_type > z_bin_limits)
{
	_set_z_limits(z_bin_limits);
	_check_limits();
}
void pair_bins_summary::set_mag_limits(IceBRG::limit_vector< flt_type > mag_bin_limits)
{
	_set_mag_limits(mag_bin_limits);
	_check_limits();
}

// Set specific limits through a linear spacing
void pair_bins_summary::set_linear_R_limits(const distance_type & R_min,
		const distance_type & R_max,
		const ssize_t & R_bins)
{
	_set_linear_R_limits(R_min,R_max,R_bins);
	_check_limits();
}
void pair_bins_summary::set_linear_m_limits(const mass_type & m_min,
		const mass_type & m_max,
		const ssize_t & m_bins)
{
	_set_linear_m_limits(m_min,m_max,m_bins);
	_check_limits();
}
void pair_bins_summary::set_linear_z_limits(const flt_type & z_min,
		const flt_type & z_max,
		const ssize_t & z_bins)
{
	_set_linear_z_limits(z_min,z_max,z_bins);
	_check_limits();
}
void pair_bins_summary::set_linear_mag_limits(const flt_type & mag_min,
		const flt_type & mag_max,
		const ssize_t & mag_bins)
{
	_set_linear_mag_limits(mag_min,mag_max,mag_bins);
	_check_limits();
}

// Set specific limits through a log spacing
void pair_bins_summary::set_log_R_limits(const distance_type & R_min,
		const distance_type & R_max,
		const ssize_t & R_num_bins)
{
	_set_log_R_limits(R_min,R_max,R_num_bins);
	_check_limits();
}
void pair_bins_summary::set_log_m_limits(const mass_type & m_min,
		const mass_type & m_max,
		const ssize_t & m_num_bins)
{
	_set_log_m_limits(m_min,m_max,m_num_bins);
	_check_limits();
}
void pair_bins_summary::set_log_z_limits(const flt_type & z_min,
		const flt_type & z_max,
		const ssize_t & z_num_bins)
{
	_set_log_z_limits(z_min,z_max,z_num_bins);
	_check_limits();
}
void pair_bins_summary::set_log_mag_limits(const flt_type & mag_min,
		const flt_type & mag_max,
		const ssize_t & mag_num_bins)
{
	_set_log_mag_limits(mag_min,mag_max,mag_num_bins);
	_check_limits();
}

// Clear limits. That is, make them unbound - one bin from neg infinity to pos infinity
void pair_bins_summary::clear_R_limits()
{
	_clear_R_limits();
	_check_limits();
}
void pair_bins_summary::clear_m_limits()
{
	_clear_m_limits();
	_check_limits();
}
void pair_bins_summary::clear_z_limits()
{
	_clear_z_limits();
	_check_limits();
}
void pair_bins_summary::clear_mag_limits()
{
	_clear_mag_limits();
	_check_limits();
}

void pair_bins_summary::set_limits(IceBRG::limit_vector< distance_type > R_bin_limits,
		IceBRG::limit_vector< mass_type > m_bin_limits,
		IceBRG::limit_vector< flt_type > z_bin_limits,
		IceBRG::limit_vector< flt_type > mag_bin_limits)
{
	_set_R_limits(R_bin_limits);
	_set_m_limits(m_bin_limits);
	_set_z_limits(z_bin_limits);
	_set_mag_limits(mag_bin_limits);
	_check_limits();
}

void pair_bins_summary::set_linear_limits(const distance_type & R_min,
			const distance_type & R_max,
			const ssize_t & R_bins,
			const mass_type & m_min,
			const mass_type & m_max,
			const ssize_t & m_bins,
			const flt_type & z_min,
			const flt_type & z_max,
			const ssize_t & z_bins,
			const flt_type & mag_min,
			const flt_type & mag_max,
			const ssize_t & mag_bins)
{
	_set_linear_R_limits(R_min,R_max,R_bins);
	_set_linear_m_limits(m_min,m_max,m_bins);
	_set_linear_z_limits(z_min,z_max,z_bins);
	_set_linear_mag_limits(mag_min,mag_max,mag_bins);
	_check_limits();
}

void pair_bins_summary::set_log_limits(const distance_type & R_min,
			const distance_type & R_max,
			const ssize_t & R_num_bins,
			const mass_type & m_min,
			const mass_type & m_max,
			const ssize_t & m_num_bins,
			const flt_type & z_min,
			const flt_type & z_max,
			const ssize_t & z_num_bins,
			const flt_type & mag_min,
			const flt_type & mag_max,
			const ssize_t & mag_num_bins)
{
	_set_log_R_limits(R_min,R_max,R_num_bins);
	_set_log_m_limits(m_min,m_max,m_num_bins);
	_set_log_z_limits(z_min,z_max,z_num_bins);
	_set_log_mag_limits(mag_min,mag_max,mag_num_bins);
	_check_limits();
}

#endif

// Adding and clearing data
#if(1)

void pair_bins_summary::clear()
{
	_pair_bin_summaries_.clear();
}

#endif // Adding and clearing data

void pair_bins_summary::fixbad()
{
	_R_bin_limits_.fixbad();
	_m_bin_limits_.fixbad();
	_z_bin_limits_.fixbad();
	_mag_bin_limits_.fixbad();
	_construct();
	for(auto & v1 : _pair_bin_summaries_)
		for(auto & v2 : v1)
			for(auto & v3 : v2)
				for(auto & val : v3)
					val.fixbad();
}

// Accessing summary data for bins
#if(1)

// Access by index (will throw if out of bounds)
#if(1)
surface_density_type pair_bins_summary::delta_Sigma_t_mean_for_bin(ssize_t R_i, ssize_t m_i, ssize_t z_i, ssize_t mag_i) const
{
	return pair_bin_summaries().at(R_i).at(m_i).at(z_i).at(mag_i).delta_Sigma_t_mean();
}
surface_density_type pair_bins_summary::delta_Sigma_x_mean_for_bin(ssize_t R_i, ssize_t m_i, ssize_t z_i, ssize_t mag_i) const
{
	return pair_bin_summaries().at(R_i).at(m_i).at(z_i).at(mag_i).delta_Sigma_x_mean();
}

surface_density_type pair_bins_summary::delta_Sigma_t_std_for_bin(ssize_t R_i, ssize_t m_i, ssize_t z_i, ssize_t mag_i) const
{
	return pair_bin_summaries().at(R_i).at(m_i).at(z_i).at(mag_i).delta_Sigma_t_std();
}
surface_density_type pair_bins_summary::delta_Sigma_x_std_for_bin(ssize_t R_i, ssize_t m_i, ssize_t z_i, ssize_t mag_i) const
{
	return pair_bin_summaries().at(R_i).at(m_i).at(z_i).at(mag_i).delta_Sigma_x_std();
}

surface_density_type pair_bins_summary::delta_Sigma_t_stderr_for_bin(ssize_t R_i, ssize_t m_i, ssize_t z_i, ssize_t mag_i) const
{
	return pair_bin_summaries().at(R_i).at(m_i).at(z_i).at(mag_i).delta_Sigma_t_stderr();
}
surface_density_type pair_bins_summary::delta_Sigma_x_stderr_for_bin(ssize_t R_i, ssize_t m_i, ssize_t z_i, ssize_t mag_i) const
{
	return pair_bin_summaries().at(R_i).at(m_i).at(z_i).at(mag_i).delta_Sigma_t_stderr();
}
#endif // Access by index

// Access by position
#if(1)
surface_density_type pair_bins_summary::delta_Sigma_t_mean_for_bin(const distance_type & R, const mass_type & m,
		flt_type z, flt_type mag) const
{
	ssize_t R_i = _R_bin_limits_.get_bin_index(R);
	ssize_t m_i = _m_bin_limits_.get_bin_index(m);
	ssize_t z_i = _z_bin_limits_.get_bin_index(z);
	ssize_t mag_i = _mag_bin_limits_.get_bin_index(mag);
	return pair_bin_summaries().at(R_i).at(m_i).at(z_i).at(mag_i).delta_Sigma_t_mean();
}
surface_density_type pair_bins_summary::delta_Sigma_x_mean_for_bin(const distance_type & R, const mass_type & m,
		flt_type z, flt_type mag) const
{
	ssize_t R_i = _R_bin_limits_.get_bin_index(R);
	ssize_t m_i = _m_bin_limits_.get_bin_index(m);
	ssize_t z_i = _z_bin_limits_.get_bin_index(z);
	ssize_t mag_i = _mag_bin_limits_.get_bin_index(mag);
	return pair_bin_summaries().at(R_i).at(m_i).at(z_i).at(mag_i).delta_Sigma_x_mean();
}

surface_density_type pair_bins_summary::delta_Sigma_t_std_for_bin(const distance_type & R, const mass_type & m,
		flt_type z, flt_type mag) const
{
	ssize_t R_i = _R_bin_limits_.get_bin_index(R);
	ssize_t m_i = _m_bin_limits_.get_bin_index(m);
	ssize_t z_i = _z_bin_limits_.get_bin_index(z);
	ssize_t mag_i = _mag_bin_limits_.get_bin_index(mag);
	return pair_bin_summaries().at(R_i).at(m_i).at(z_i).at(mag_i).delta_Sigma_t_std();
}
surface_density_type pair_bins_summary::delta_Sigma_x_std_for_bin(const distance_type & R, const mass_type & m,
		flt_type z, flt_type mag) const
{
	ssize_t R_i = _R_bin_limits_.get_bin_index(R);
	ssize_t m_i = _m_bin_limits_.get_bin_index(m);
	ssize_t z_i = _z_bin_limits_.get_bin_index(z);
	ssize_t mag_i = _mag_bin_limits_.get_bin_index(mag);
	return pair_bin_summaries().at(R_i).at(m_i).at(z_i).at(mag_i).delta_Sigma_x_std();
}

surface_density_type pair_bins_summary::delta_Sigma_t_stderr_for_bin(const distance_type & R, const mass_type & m,
		flt_type z, flt_type mag) const
{
	ssize_t R_i = _R_bin_limits_.get_bin_index(R);
	ssize_t m_i = _m_bin_limits_.get_bin_index(m);
	ssize_t z_i = _z_bin_limits_.get_bin_index(z);
	ssize_t mag_i = _mag_bin_limits_.get_bin_index(mag);
	return pair_bin_summaries().at(R_i).at(m_i).at(z_i).at(mag_i).delta_Sigma_t_stderr();
}
surface_density_type pair_bins_summary::delta_Sigma_x_stderr_for_bin(const distance_type & R, const mass_type & m,
		flt_type z, flt_type mag) const
{
	ssize_t R_i = _R_bin_limits_.get_bin_index(R);
	ssize_t m_i = _m_bin_limits_.get_bin_index(m);
	ssize_t z_i = _z_bin_limits_.get_bin_index(z);
	ssize_t mag_i = _mag_bin_limits_.get_bin_index(mag);
	return pair_bin_summaries().at(R_i).at(m_i).at(z_i).at(mag_i).delta_Sigma_x_stderr();
}
#endif // Access by index

#endif // Accessing summary data for bins

// Print data for all bins
void pair_bins_summary::print_bin_data(std::ostream &out,
		const unitconv_map & u_map)
{

	// Set up the data and header to be printed
	table_t<flt_type> data;
	header_t header;

	header.push_back("R_min");
	header.push_back("R_max");
	header.push_back("m_min");
	header.push_back("m_max");
	header.push_back("z_min");
	header.push_back("z_max");
	header.push_back("mag_min");
	header.push_back("mag_max");

	header.push_back("shear_R_mean");
	header.push_back("shear_lens_m_mean");
	header.push_back("shear_lens_z_mean");
	header.push_back("shear_lens_mag_mean");

	header.push_back("shear_source_z_mean");

	header.push_back("shear_N_pair");
	header.push_back("shear_N_pair_eff");

	header.push_back("shear_Sigma_crit");

	header.push_back("dS_t_mean");
	header.push_back("dS_t_stddev");
	header.push_back("dS_t_stderr");
	header.push_back("dS_x_mean");
	header.push_back("dS_x_stddev");
	header.push_back("dS_x_stderr");

	header.push_back("gamma_t_mean");
	header.push_back("gamma_t_stderr");
	header.push_back("gamma_x_mean");
	header.push_back("gamma_x_stderr");

	header.push_back("model_dS_t");
	header.push_back("model_gamma_t");

	header.push_back("model_1h_dS_t");
	header.push_back("model_1h_gamma_t");

	header.push_back("model_group_dS_t");
	header.push_back("model_group_gamma_t");

	header.push_back("magf_R_mean");
	header.push_back("magf_lens_m_mean");
	header.push_back("magf_lens_z_mean");
	header.push_back("magf_lens_mag_mean");

	header.push_back("magf_source_z_mean");

	header.push_back("magf_N_lens");
	header.push_back("magf_area");

	header.push_back("magf_Sigma_crit");

	header.push_back("mu");
	header.push_back("mu_stderr");
	header.push_back("kappa");
	header.push_back("kappa_stderr");
	header.push_back("Sigma");
	header.push_back("Sigma_stderr");

	header.push_back("model_mu");
	header.push_back("model_kappa");
	header.push_back("model_Sigma");

	header.push_back("model_1h_mu");
	header.push_back("model_1h_kappa");
	header.push_back("model_1h_Sigma");

	header.push_back("model_group_mu");
	header.push_back("model_group_kappa");
	header.push_back("model_group_Sigma");

	ssize_t num_columns = ssize(header);

	data.resize(num_columns);
	for(const auto & R_bins : pair_bin_summaries())
	{
		for(const auto & Rm_bins : R_bins)
		{
			for(const auto & Rmz_bins : Rm_bins)
			{
				for(const auto & bin : Rmz_bins)
				{

					// Check if this bin is good
					if(bin.shear_effective_pair_count()>=std::numeric_limits<flt_type>::max()) continue;
					if(isbad(bin.shear_effective_pair_count())) continue;
					// It's possible we'll get bins with no shear information like this, but this
					// prunes out at least those without any info

					ssize_t col_i = -1;

					data[++col_i].push_back(value_of(bin.R_min()));
					data[++col_i].push_back(value_of(bin.R_max()));
					data[++col_i].push_back(value_of(bin.m_min()));
					data[++col_i].push_back(value_of(bin.m_max()));
					data[++col_i].push_back(bin.z_min());
					data[++col_i].push_back(bin.z_max());
					data[++col_i].push_back(bin.mag_min());
					data[++col_i].push_back(bin.mag_max());

					data[++col_i].push_back(value_of(bin.shear_R_mean()));
					data[++col_i].push_back(value_of(bin.shear_lens_m_mean()));
					data[++col_i].push_back(bin.shear_lens_z_mean());
					data[++col_i].push_back(bin.shear_lens_mag_mean());

					data[++col_i].push_back(bin.shear_source_z_mean());

					data[++col_i].push_back(bin.shear_pair_count());
					data[++col_i].push_back(bin.shear_effective_pair_count());

					data[++col_i].push_back(value_of(bin.shear_sigma_crit()));

					data[++col_i].push_back(value_of(bin.delta_Sigma_t_mean()));
					data[++col_i].push_back(value_of(bin.delta_Sigma_t_std()));
					data[++col_i].push_back(value_of(bin.delta_Sigma_t_stderr()));
					data[++col_i].push_back(value_of(bin.delta_Sigma_x_mean()));
					data[++col_i].push_back(value_of(bin.delta_Sigma_x_std()));
					data[++col_i].push_back(value_of(bin.delta_Sigma_x_stderr()));

					data[++col_i].push_back(bin.gamma_t_mean());
					data[++col_i].push_back(bin.gamma_t_stderr());
					data[++col_i].push_back(bin.gamma_x_mean());
					data[++col_i].push_back(bin.gamma_x_stderr());

					data[++col_i].push_back(value_of(bin.model_delta_Sigma_t()));
					data[++col_i].push_back(bin.model_gamma_t());

					data[++col_i].push_back(value_of(bin.model_1h_delta_Sigma_t()));
					data[++col_i].push_back(bin.model_1h_gamma_t());

					data[++col_i].push_back(value_of(bin.model_offset_delta_Sigma_t()));
					data[++col_i].push_back(bin.model_offset_gamma_t());

					data[++col_i].push_back(value_of(bin.magf_R_mean()));
					data[++col_i].push_back(value_of(bin.magf_lens_m_mean()));
					data[++col_i].push_back(bin.magf_lens_z_mean());
					data[++col_i].push_back(bin.magf_lens_mag_mean());

					data[++col_i].push_back(bin.magf_source_z_mean());

					data[++col_i].push_back(bin.magf_num_lenses());
					data[++col_i].push_back(value_of(bin.area()));

					data[++col_i].push_back(value_of(bin.magf_sigma_crit()));

					data[++col_i].push_back(bin.mu_hat());
					data[++col_i].push_back(bin.mu_stderr());
					data[++col_i].push_back(bin.kappa());
					data[++col_i].push_back(bin.kappa_stderr());
					data[++col_i].push_back(value_of(bin.Sigma()));
					data[++col_i].push_back(value_of(bin.Sigma_stderr()));

					data[++col_i].push_back(bin.model_mu());
					data[++col_i].push_back(bin.model_kappa());
					data[++col_i].push_back(value_of(bin.model_Sigma()));

					data[++col_i].push_back(bin.model_1h_mu());
					data[++col_i].push_back(bin.model_1h_kappa());
					data[++col_i].push_back(value_of(bin.model_1h_Sigma()));

					data[++col_i].push_back(bin.model_offset_mu());
					data[++col_i].push_back(bin.model_offset_kappa());
					data[++col_i].push_back(value_of(bin.model_offset_Sigma()));
				}
			}
		}
	}

	assert(ssize(data.back())==ssize(data.front())); // Check we didn't miss a column to add to the table

	table_map_t<flt_type> table_map = get_table_after_unitconv(make_table_map(data,header),u_map);

	// And now print it out
	print_table_map<flt_type>(out,table_map);

}
void pair_bins_summary::print_bin_data(const std::string & file_name,
		const unitconv_map & u_map)
{
	std::ofstream fo;
	open_file_output(fo,file_name);

	print_bin_data(fo,u_map);
}

// Operators to combine data
#if (1)

pair_bins_summary & pair_bins_summary::operator+=( const pair_bins_summary & other )
{
	// Check for same size
	assert(_R_bin_limits_==other.R_limits());
	assert(_m_bin_limits_==other.m_limits());
	assert(_z_bin_limits_==other.z_limits());
	assert(_mag_bin_limits_==other.mag_limits());

	// Make sure the other is sorted
	other.sort();

	// Make sure this is constructed
	_construct();

	// And add the bins together

	for(ssize_t R_i=0; R_i<ssize(_pair_bin_summaries_); ++R_i)
	{
		for(ssize_t m_i=0; m_i<ssize(_pair_bin_summaries_[R_i]); ++m_i)
		{
			for(ssize_t z_i=0; z_i<ssize(_pair_bin_summaries_[R_i][m_i]); ++z_i)
			{
				for(ssize_t mag_i=0; mag_i<ssize(_pair_bin_summaries_[R_i][m_i][z_i]); ++mag_i)
				{
					_pair_bin_summaries_[R_i][m_i][z_i][mag_i] +=
							other.pair_bin_summaries()[R_i][m_i][z_i][mag_i];
				}
			}
		}
	}

	return *this;
}

#endif

// Saving/loading data
#if(1)

void pair_bins_summary::save(std::ostream & out) const
{
	_construct();
	boost::archive::text_oarchive ar(out);
	ar << *this;
}
void pair_bins_summary::save(const std::string & filename) const
{
	std::ofstream out;
	open_file_output(out,filename);
	save(out);
}
void pair_bins_summary::save(std::ostream & out)
{
	_construct();
	fixbad();
	boost::archive::text_oarchive ar(out);
	ar << *this;
}
void pair_bins_summary::save(const std::string & filename)
{
	std::ofstream out;
	open_file_output(out,filename);
	save(out);
}
void pair_bins_summary::load(std::istream & in)
{
	boost::archive::text_iarchive ar(in);
	ar >> *this;
}
void pair_bins_summary::load(const std::string & filename)
{
	std::ifstream in;
	open_file_input(in,filename);
	load(in);
}

#endif

} // end namespace IceBRG
