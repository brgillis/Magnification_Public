/**********************************************************************\
 @file shifting_loader.cpp
 ------------------

 This class is used to load in the data tables provided by Chang and
 Jain for the mean relative shift of two points in space, so the data
 can be stored in the same format as my other caches.

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
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "IceBRG_main/common.h"

#include "IceBRG_main/file_access/ascii_table_map.hpp"
#include "IceBRG_main/utility.hpp"
#include "IceBRG_main/vector/elementwise_functions.hpp"
#include "IceBRG_main/units/units.hpp"

#include "IceBRG_lensing/magnification/count_fitting_results.hpp"

#include "expected_count_fit_loader.h"


// Initialisation of static vars
#if (1)
bool IceBRG::expected_count_fit_loader::_loaded_(false);
IceBRG::table_map_t<flt_type> IceBRG::expected_count_fit_loader::_data_map_;
#endif

const flt_type & num_columns=8;

void IceBRG::expected_count_fit_loader::_load()
{
	#ifdef _OPENMP
	#pragma omp critical(brg_load_expected_count_loader)
	#endif
	{
		if(!_loaded_)
		{
			std::stringstream ss(count_fitting_result_data);

			_data_map_ = load_table_map<flt_type>(ss);

			assert(ssize(_data_map_)==num_columns);
			assert(ssize(_data_map_.at("z_mid"))>=2);

			_loaded_ = true;
		}
	}

}
ssize_t IceBRG::expected_count_fit_loader::_lower_z_index(const flt_type & z)
{
	auto size = ssize(_data_map_.at("z_mid"));
	assert(size>=2);

	for(ssize_t i=1; i<size; ++i)
	{
		if(z<_data_map_["z_mid"][i])
			return i-1;
	}
	return size-2;
}

std::vector<flt_type> IceBRG::expected_count_fit_loader::get(const flt_type & z)
{
	if(!_loaded_) _load();

	const size_t zi = _lower_z_index(z);

	const flt_type & zlo = _data_map_["z_mid"][zi];
	const flt_type & zhi = _data_map_["z_mid"][zi+1];

	const flt_type & weight = zhi-zlo;

	std::vector<flt_type> r_lo, r_hi;

#ifdef _BRG_USE_UNITS_
	r_lo.push_back(_data_map_["N_scale"].at(zi)*(zhi-z));
#else
	r_lo.push_back(_data_map_["N_scale"].at(zi)*(zhi-z));
#endif
	r_lo.push_back(_data_map_["m_star_lower"].at(zi)*(zhi-z));
	r_lo.push_back(_data_map_["alpha"].at(zi)*(zhi-z));
	r_lo.push_back(_data_map_["lower_cutoff_sharpness"].at(zi)*(zhi-z));
#ifdef _BRG_USE_UNITS_
	r_lo.push_back(_data_map_["mag23_jump"].at(zi)*(zhi-z));
#else
	r_lo.push_back(_data_map_["mag23_jump"].at(zi)*(zhi-z));
#endif
	r_lo.push_back(_data_map_["m_star_upper"].at(zi)*(zhi-z));
	r_lo.push_back(_data_map_["upper_cutoff_sharpness"].at(zi)*(zhi-z));

#ifdef _BRG_USE_UNITS_
	r_hi.push_back(_data_map_["N_scale"].at(zi+1)*(z-zlo));
#else
	r_hi.push_back(_data_map_["N_scale"].at(zi+1)*(z-zlo));
#endif
	r_hi.push_back(_data_map_["m_star_lower"].at(zi+1)*(z-zlo));
	r_hi.push_back(_data_map_["alpha"].at(zi+1)*(z-zlo));
	r_hi.push_back(_data_map_["lower_cutoff_sharpness"].at(zi+1)*(z-zlo));
#ifdef _BRG_USE_UNITS_
	r_hi.push_back(_data_map_["mag23_jump"].at(zi+1)*(z-zlo));
#else
	r_hi.push_back(_data_map_["mag23_jump"].at(zi+1)*(z-zlo));
#endif
	r_hi.push_back(_data_map_["m_star_upper"].at(zi+1)*(z-zlo));
	r_hi.push_back(_data_map_["upper_cutoff_sharpness"].at(zi+1)*(z-zlo));

	return divide(add(r_lo,r_hi),weight);
}

void IceBRG::expected_count_fit_loader::unload()
{
	_data_map_.clear();
	_loaded_ = false;
}
