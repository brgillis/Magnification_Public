/**********************************************************************\
 @file pass_configs_to_binner.h
 ------------------

 A function to create a pair_binner object using a set of configuration
 settings.

 **********************************************************************

 Copyright (C) 2014, 2015  Bryan R. Gillis

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


// body file: pass_configs_to_binner.cpp


#ifndef _BRG_PASS_CONFIGURATIONS_TO_BINNER_H_INCLUDED_
#define _BRG_PASS_CONFIGURATIONS_TO_BINNER_H_INCLUDED_

#include "IceBRG_lensing/pair_binner.h"

#include "gg_lensing_config.h"

IceBRG::pair_binner pass_configs_to_binner(const gg_lensing_config & config);

#endif // _BRG_PASS_CONFIGURATIONS_TO_BINNER_H_INCLUDED_
