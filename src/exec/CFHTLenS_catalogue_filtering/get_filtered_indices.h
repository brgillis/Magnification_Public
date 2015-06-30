/**********************************************************************\
 @file get_filtered_indices.h
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


// body file: get_filtered_indices.cpp

#ifndef _BRG_GET_FILTERED_INDICES_H_INCLUDED_
#define _BRG_GET_FILTERED_INDICES_H_INCLUDED_

#include <vector>

#include "IceBRG_main/container/labeled_array.hpp"
#include "IceBRG_main/container/table_typedefs.hpp"

void move_y_column_to_i(IceBRG::labeled_array<double> & map);

std::vector<size_t> get_good_lenses(const IceBRG::labeled_array<double> & map, const std::vector<std::vector<bool>> good_pixels);
std::vector<size_t> get_good_sources(const IceBRG::labeled_array<double> & map, const std::vector<std::vector<bool>> good_pixels);

bool galaxy_passes_lens_filter(const IceBRG::labeled_array<double>::const_row_reference & galaxy);
bool galaxy_passes_source_filter(const IceBRG::labeled_array<double>::const_row_reference & galaxy);
bool galaxy_passes_global_filter(const IceBRG::labeled_array<double>::const_row_reference & galaxy);



#endif // _BRG_GET_FILTERED_INDICES_H_INCLUDED_
