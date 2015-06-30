/**********************************************************************\
  @file position_grid_cache.cpp

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

#include "IceBRG_main/common.h"

#include "position_grid_cache.h"
#include "IceBRG_main/units/units.hpp"

// Initialisation for IceBRG::grid_cache
#if (1)

int_type IceBRG::grid_cache::_ra_grid_change_num_ = 0;
int_type IceBRG::grid_cache::_dec_grid_change_num_ = 0;
int_type IceBRG::grid_cache::_z_grid_change_num_ = 0;

#ifdef _BRG_USE_UNITS_
IceBRG::angle_type IceBRG::grid_cache::_ra_grid_min_(-pi*boost::units::si::radians);
IceBRG::angle_type IceBRG::grid_cache::_ra_grid_max_(pi*boost::units::si::radians);
IceBRG::angle_type IceBRG::grid_cache::_ra_grid_step_(pi / 8*boost::units::si::radians);
IceBRG::angle_type IceBRG::grid_cache::_dec_grid_min_(-pi / 2*boost::units::si::radians);
IceBRG::angle_type IceBRG::grid_cache::_dec_grid_max_(pi / 2*boost::units::si::radians);
IceBRG::angle_type IceBRG::grid_cache::_dec_grid_step_(pi / 8*boost::units::si::radians);
#else
IceBRG::angle_type IceBRG::grid_cache::_ra_grid_min_ = -pi;
IceBRG::angle_type IceBRG::grid_cache::_ra_grid_max_ = pi;
IceBRG::angle_type IceBRG::grid_cache::_ra_grid_step_ = pi / 8;
IceBRG::angle_type IceBRG::grid_cache::_dec_grid_min_ = -pi / 2;
IceBRG::angle_type IceBRG::grid_cache::_dec_grid_max_ = pi / 2;
IceBRG::angle_type IceBRG::grid_cache::_dec_grid_step_ = pi / 8;
#endif

flt_type IceBRG::grid_cache::_z_grid_min_ = 0;
flt_type IceBRG::grid_cache::_z_grid_max_ = 2;
flt_type IceBRG::grid_cache::_z_grid_step_ = 0.1;

#endif // End initialisation for IceBRG::grid_cache

