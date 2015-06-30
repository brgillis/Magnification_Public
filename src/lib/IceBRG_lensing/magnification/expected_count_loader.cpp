/**********************************************************************\
 @file expected_count_loader.cpp
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

#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <boost/lexical_cast.hpp>

#include "IceBRG_main/common.h"

#include "IceBRG_main/file_access/ascii_table_map.hpp"
#include "IceBRG_main/math/misc_math.hpp"
#include "IceBRG_main/utility.hpp"
#include "IceBRG_main/vector/limit_vector.hpp"

#include "IceBRG_lensing/magnification/expected_count_loader.h"
#include "IceBRG_lensing/magnification/mag_global_values.h"

#undef USE_FIELD_WEIGHTING

namespace IceBRG {

// Initialisation of static vars
#if (1)
bool expected_count_loader::_loaded_(false);

IceBRG::limit_vector<flt_type> expected_count_loader::_z_limits_;
std::vector<IceBRG::limit_vector<flt_type>> expected_count_loader::_mag_limits_;
std::vector<std::vector<flt_type>> expected_count_loader::_smoothed_count_;
std::vector<std::vector<flt_type>> expected_count_loader::_smoothed_count_derivative_;

std::string expected_count_loader::_filename_base_("/disk2/brg/git/CFHTLenS_cat/Data/magnitude_hist_gz");
std::string expected_count_loader::_filename_tail_(".dat");

#ifdef USE_FIELD_WEIGHTING
#define COUNT_COLUMN "weighted_count"
#else
#define COUNT_COLUMN "count"
#endif

#endif

void expected_count_loader::_load()
{
	if(_loaded_) return;

	#ifdef _OPENMP
	#pragma omp critical(brg_expected_count_loader_load)
	#endif
	{

	if(!_loaded_)
	{

		_z_limits_.reconstruct(limit_vector<flt_type>::type::LINEAR, mag_z_min, mag_z_max, round_int((mag_z_max-mag_z_min)/mag_z_step));

		// Resize arrays as necessary
		size_t num_z_bins = _z_limits_.num_bins();

		_mag_limits_.resize(num_z_bins);
		_smoothed_count_.resize(num_z_bins);
		_smoothed_count_derivative_.resize(num_z_bins);

		// Loop over z, loading in all files
		for(size_t z_i=0; z_i<num_z_bins; ++z_i)
		{
			const int_type z1000(IceBRG::round_int(1000*_z_limits_.lower_limit(z_i)));
			const std::string filename(_filename_base_ + boost::lexical_cast<std::string>(z1000)
					+ _filename_tail_);

			try
			{
				auto table_map = IceBRG::load_table_map<flt_type>(filename);
				std::vector<flt_type> temp_mag_limits = table_map.at("mag_bin_lower");
				_smoothed_count_[z_i] = table_map.at(COUNT_COLUMN);
				_smoothed_count_derivative_[z_i] = table_map.at("smoothed_alpha");

				// Add a final value to the _mag_limits_ vector
				temp_mag_limits.push_back(2*temp_mag_limits.back()-
						temp_mag_limits.at(ssize(temp_mag_limits)-2));
				_mag_limits_[z_i] = temp_mag_limits;
			}
			catch(const std::runtime_error &e)
			{
				std::cerr << "ERROR: Cannot load data for expected_count_loader. Check that the filename\n"
						<< "base and root match the data (including path), and the z limits are set up to match.\n"
						<< "Original exception:" << e.what();
				throw;
			}

			// Normalize by magnitude bin size
			flt_type bin_size = _mag_limits_[z_i].at(1)-_mag_limits_[z_i].at(0);
			_smoothed_count_[z_i] = IceBRG::divide(_smoothed_count_[z_i],bin_size);

		} // Loop over z, loading in all files

		_loaded_ = true;

	}

	}
} // void expected_count_loader::_load()


flt_type expected_count_loader::_get_interp(const flt_type & mag, const flt_type & z,
		const std::vector<std::vector<flt_type>> & table, const flt_type & default_result)
{
	// Load if necessary
	_load();

	ssize_t z_i = _z_limits_.get_bin_index(z);

	if(z_i>=_z_limits_.num_bins()-1) z_i=_z_limits_.num_bins()-2;
	const flt_type & z_lo = _z_limits_.lower_limit(z_i);
	const flt_type & z_hi = _z_limits_.upper_limit(z_i);

	flt_type tot_weight;
	flt_type temp_result;

	// Get the interpolated value at both the lower redshift and the higher

	// At the lower redshift first
	flt_type lo_result;
	lo_result = _mag_limits_[z_i].interpolate_bins(mag,table[z_i]);

	// At the higher redshift now
	flt_type hi_result;
	hi_result = _mag_limits_[z_i+1].interpolate_bins(mag,table[z_i+1]);

	// And now interpolate between these results

	tot_weight = z_hi-z_lo;

	temp_result = 0;
	temp_result += lo_result * (z_hi-z);
	temp_result += hi_result * (z-z_lo);
	return temp_result / tot_weight;

} // flt_type expected_count_loader::_get_interp(...)

// Setting parameters for where the data can be loaded from
#if(1)
void expected_count_loader::set_z_limits(const std::vector<flt_type> & new_limits_vector)
{
	if(!is_monotonically_increasing(new_limits_vector))
	{
		throw std::logic_error("New z_limits vector must be monotonically increasing.");
	}
	_z_limits_ = new_limits_vector;
}
void expected_count_loader::set_z_limits(std::vector<flt_type> && new_limits_vector)
{
	if(!is_monotonically_increasing(new_limits_vector))
	{
		throw std::logic_error("New z_limits vector must be monotonically increasing.");
	}
	_z_limits_ = std::move(new_limits_vector);
}
void expected_count_loader::set_filename_base(const std::string & new_filename_base)
{
	_filename_base_ = new_filename_base;
}
void expected_count_loader::set_filename_base(std::string && new_filename_base)
{
	_filename_base_ = std::move(new_filename_base);
}
void expected_count_loader::set_filename_tail(const std::string & new_filename_tail)
{
	_filename_tail_ = new_filename_tail;
}
void expected_count_loader::set_filename_tail(std::string && new_filename_tail)
{
	_filename_tail_ = std::move(new_filename_tail);
}
#endif // Setting parameters for where the data is

// Access data
#if(1)

flt_type expected_count_loader::get_count(const flt_type & mag, const flt_type & z)
{
	return _get_interp(mag,z,_smoothed_count_,0.);
}
flt_type expected_count_loader::get_derivative(const flt_type & mag, const flt_type & z)
{
	return _get_interp(mag,z,_smoothed_count_derivative_,1.);
}

#endif // Access data

} // end namespace IceBRG
