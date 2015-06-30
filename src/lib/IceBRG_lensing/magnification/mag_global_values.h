/**********************************************************************\
 @file mag_global_values.h
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


// body file: mag_global_values.cpp


#ifndef _BRG_MAG_GLOBAL_VALUES_H_INCLUDED_
#define _BRG_MAG_GLOBAL_VALUES_H_INCLUDED_

#include "IceBRG_main/units/unit_conversions.hpp"
#include "IceBRG_main/math/misc_math.hpp"


namespace IceBRG {

constexpr flt_type mag_m_counting_min = 19;
constexpr flt_type mag_m_counting_max = 24.7;

constexpr flt_type mag_m_min = 20;
constexpr flt_type mag_m_max = 24.7;
constexpr flt_type mag_m_step = 0.1;

constexpr flt_type mag_z_min = 0.2;
constexpr flt_type mag_z_max = 4.0;
constexpr flt_type mag_z_step = 0.01;

constexpr flt_type mag_lens_T_min = 2.0;
constexpr flt_type mag_lens_T_max = 4.0;

constexpr flt_type mag_min_count = 0.1;

} // namespace IceBRG

#endif // _BRG_MAG_GLOBAL_VALUES_H_INCLUDED_
