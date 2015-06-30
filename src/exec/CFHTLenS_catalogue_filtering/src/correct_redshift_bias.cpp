/**********************************************************************\
 @file correct_redshift_bias.cpp
 ------------------

 TODO <Insert file description here>

 **********************************************************************

 Copyright (C) 2015  Bryan R. Gillis

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

#include <string>
#include <vector>

#include <boost/lexical_cast.hpp>

#include "IceBRG_main/math/interpolator/interpolator.h"
#include "IceBRG_main/file_access/ascii_table_map.hpp"

#include "correct_redshift_bias.h"

const std::string z_cal_filename = "/disk2/brg/git/CFHTLenS_cat/Data/redshift_calibration_table.dat";

IceBRG::interpolator redshift_calibrator_wrapper::redshift_calibration_interpolator = IceBRG::interpolator();

void correct_redshift_bias( std::vector<std::string> & redshifts)
{
	// Check if we need to set up the interpolator
	if(redshift_calibrator_wrapper::redshift_calibration_interpolator.empty())
	{
		redshift_calibrator_wrapper::redshift_calibration_interpolator.set_interpolation_type(IceBRG::interpolator::SPLINE);

		auto z_cal_map = IceBRG::load_table_map<double>(z_cal_filename);

		for(unsigned i=0; i<z_cal_map.at("bin_mid").size(); ++i)
		{
			redshift_calibrator_wrapper::redshift_calibration_interpolator.add_point(z_cal_map.at("bin_mid").at(i),z_cal_map.at("z_p_minus_z_s_over_1_plus_z_s").at(i));
		}
	}

	for(auto & z_s : redshifts)
	{
		double z = boost::lexical_cast<double>(z_s);
		double z_d = redshift_calibrator_wrapper::redshift_calibration_interpolator(z);
		z = (z-z_d)/(1+z_d);
		z_s = boost::lexical_cast<std::string>(z);
	}

	return;
}


