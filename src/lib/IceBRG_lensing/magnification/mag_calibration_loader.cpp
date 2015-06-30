/**********************************************************************\
 @file mag_calibration_loader.cpp
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
#include "IceBRG_main/vector/summary_functions.hpp"

#include "IceBRG_lensing/magnification/mag_calibration_loader.h"
#include "IceBRG_lensing/magnification/mag_global_values.h"

namespace IceBRG {

// Initialisation of static vars
#if (1)
bool mag_calibration_loader::_loaded_(false);

std::vector<flt_type> mag_calibration_loader::_z_mins_;
std::vector<flt_type> mag_calibration_loader::_calibration_values_;

std::string mag_calibration_loader::_filename_("/disk2/brg/git/CFHTLenS_cat/Data/gg_calibration_lensing_signal.dat");

#endif

void mag_calibration_loader::_load()
{
	if(_loaded_) return;

	// Load in the calibration file
	auto calibration_data_table = IceBRG::load_table_map<flt_type>(_filename_);

	if(is_monotonically_increasing(calibration_data_table.at("z_min")))
	{
		_z_mins_ = calibration_data_table.at("z_min");
	}
	else
	{
		throw std::runtime_error("z_min values in calibration table must be monotonically increasing.");
	}

	_calibration_values_ = calibration_data_table.at("mu");

	_loaded_ = true;

} // void mag_calibration_loader::_load()

// Setting parameters for where the data can be loaded from
#if(1)
void mag_calibration_loader::set_z_mins(const std::vector<flt_type> & z_mins)
{
	if(!is_monotonically_increasing(z_mins))
	{
		throw std::logic_error("New z_mins vector must be monotonically increasing.");
	}
	_z_mins_ = z_mins;
}
void mag_calibration_loader::set_z_mins(std::vector<flt_type> && z_mins)
{
	if(!is_monotonically_increasing(z_mins))
	{
		throw std::logic_error("New z_mins vector must be monotonically increasing.");
	}
	_z_mins_ = std::move(z_mins);
}
void mag_calibration_loader::set_filename(const std::string & new_filename)
{
	_filename_ = new_filename;
}
void mag_calibration_loader::set_filename(std::string && new_filename)
{
	_filename_ = std::move(new_filename);
}
#endif // Setting parameters for where the data is

// Access data
#if(1)

flt_type mag_calibration_loader::get(const flt_type & z)
{
	_load();

	// First, find the first z_min value this z is less than
	auto size = ssize(_z_mins_);
	decltype(size) z_i;
	for(z_i = 1; z_i<size; ++z_i)
	{
		if(_z_mins_[z_i]>z) break;
	}

	// Check if it's above the upper bound
	if(z_i>=size) z_i = size;

	--z_i; // Decrease it by one, so it now represents the lower limit of the two we'll interpolate between

	flt_type w = _z_mins_[z_i+1]-_z_mins_[z_i];

	flt_type res = 0;
	res += _calibration_values_[z_i]*(_z_mins_[z_i+1]-z);
	res += _calibration_values_[z_i+1]*(z-_z_mins_[z_i]);
	res /= w;

	return res;
}

#endif // Access data

void mag_calibration_loader::unload()
{
	_loaded_ = false;
	set_zero(_z_mins_);
	set_zero(_calibration_values_);
}

} // end namespace IceBRG
