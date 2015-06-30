/**********************************************************************\
 @file mag_calibration_loader.h
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

// body file: mag_calibration_loader.cpp

#ifndef _BRG_PHYSICS_LENSING_MAGNIFICATION_MAG_CALIBRATION_LOADER_H_INCLUDED_
#define _BRG_PHYSICS_LENSING_MAGNIFICATION_MAG_CALIBRATION_LOADER_H_INCLUDED_

#include <string>
#include <vector>

#include "IceBRG_main/common.h"

namespace IceBRG {

/**
 *
 */
class mag_calibration_loader {
	static bool _loaded_;
	static std::vector<flt_type> _z_mins_;
	static std::vector<flt_type> _calibration_values_;
	static std::string _filename_;

	static void _load();
public:

	// Setting parameters for where the data is stored
#if(1)
	static void set_z_mins(const std::vector<flt_type> & new_limits_vector);
	static void set_z_mins(std::vector<flt_type> && new_limits_vector);
	static void set_filename(const std::string & new_filename);
	static void set_filename(std::string && new_filename);
#endif

	static flt_type get(const flt_type & z);

	static void unload();
};

} // end namespace IceBRG

#endif // _BRG_PHYSICS_LENSING_MAGNIFICATION_MAG_CALIBRATION_LOADER_H_INCLUDED_
