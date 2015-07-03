/**********************************************************************\
 @file gg_lensing_config.h
 ------------------

 An object to store configuration settings for the gg_lensing program.

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

// body file: gg_lensing_config.cpp

#ifndef _BRG_GG_LENSING_CONFIG_H_INCLUDED_
#define _BRG_GG_LENSING_CONFIG_H_INCLUDED_

#include <cassert>
#include <cstdlib>

#include "IceBRG_main/common.h"

#include "IceBRG_main/units/unit_conversions.hpp"
#include "IceBRG_main/units/units.hpp"


/**
 *
 */
struct gg_lensing_config {

	static constexpr size_t num_config_params = 19;

	bool use_precalculated_data;
	std::string precalculated_data_filename;

	IceBRG::distance_type R_min, R_max;
	IceBRG::mass_type m_min, m_max;
	double z_min, z_max;
	double mag_min, mag_max;

	size_t R_bins, m_bins, z_bins, mag_bins;
	bool R_log, m_log, z_log, mag_log;

	gg_lensing_config( const int argc=0, const char *argv[]=nullptr );
	virtual ~gg_lensing_config()
	{
	}
};

#endif // _BRG_GG_LENSING_CONFIG_H_INCLUDED_
