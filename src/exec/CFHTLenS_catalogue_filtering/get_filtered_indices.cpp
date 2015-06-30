/**********************************************************************\
 @file get_filtered_indices.cpp
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

#include <limits>
#include <map>
#include <utility>
#include <vector>

#include <boost/lexical_cast.hpp>

#include "IceBRG_main/container/table_typedefs.hpp"
#include "IceBRG_main/math/misc_math.hpp"

#include "is_good_position.hpp"

#include "get_filtered_indices.h"

#undef WARN_MASK_MISMATCH

#ifdef WARN_MASK_MISMATCH
#include "IceBRG_main/error_handling.h"
#endif

void move_y_column_to_i(IceBRG::labeled_array<double> & map)
{
	// Check if the i columns exist, and if not, move in y columns
	if(map.count("MAG_i")==0)
	{
		map.change_label("MAG_y","MAG_i");
	}
	if(map.count("MAGERR_i")==0)
	{
		map.change_label("MAGERR_y","MAGERR_i");
	}
	if(map.count("EXTINCTION_i")==0)
	{
		map.change_label("EXTINCTION_y","EXTINCTION_i");
	}
}

std::vector<size_t> get_good_lenses(const IceBRG::labeled_array<double> & map, const std::vector<std::vector<bool>> good_pixels)
{
	// Determine the number of rows
	const size_t num_rows = map.num_rows();

	std::vector<size_t> good_indices;
	good_indices.reserve(num_rows);

	for(size_t i=0; i<num_rows; ++i)
	{
		const auto & galaxy = map.row(i);

		// Check if this object is within the mask
		const unsigned xp = galaxy.at_label("Xpos");
		const unsigned yp = galaxy.at_label("Ypos");

		if(!is_good_position(xp,yp,good_pixels))
		{

#ifdef WARN_MASK_MISMATCH
			// Check that the mask value in the table agrees
			if(galaxy.at_label("MASK"))<=1)
			{
				handle_notification(std::string("Mask value mismatch for lens index ") + i + " at position (" + xp + ", " + yp + ").\n"
				+ "Table's mask value is " + galaxy.at_label("MASK") + ", but saved value is 'false'.");
			}
#endif

			continue;
		}
		else
		{
#ifdef WARN_MASK_MISMATCH
			// Check that the mask value in the table agrees
			if(galaxy.at_label("MASK")>1)
			{
				handle_notification(std::string("Mask value mismatch for lens index ") + i + " at position (" + xp + ", " + yp + ").\n"
				+ "Table's mask value is " + galaxy.at_label("MASK") + ", but saved value is 'true'.");
			}
#endif
		}

		if(galaxy_passes_lens_filter(galaxy))
		{
			good_indices.push_back(i);
		}
	}

	return good_indices;
}

std::vector<size_t> get_good_sources(const IceBRG::labeled_array<double> & map, const std::vector<std::vector<bool>> good_pixels)
{
	// Determine the number of rows
	const size_t num_rows = map.num_rows();

	std::vector<size_t> good_indices;
	good_indices.reserve(num_rows);

	for(size_t i=0; i<num_rows; ++i)
	{
		const auto & galaxy = map.row(i);

		// Check if this object is within the mask
		const unsigned xp = galaxy.at_label("Xpos");
		const unsigned yp = galaxy.at_label("Ypos");

		if(!is_good_position(xp,yp,good_pixels))
		{

#ifdef WARN_MASK_MISMATCH
			// Check that the mask value in the table agrees
			if(galaxy.at_label("MASK"))<=1)
			{
				std::cerr << "WARNING: Mask value mismatch for lens index " << i << " at position (" << xp << ", " << yp << ")." << std::endl;
				std::cerr << "Table's mask value is " << galaxy.at_label("MASK") << ", but saved value is 'false'.\n";
			}
#endif

			continue;
		}
		else
		{
#ifdef WARN_MASK_MISMATCH
			// Check that the mask value in the table agrees
			if(galaxy.at_label("MASK")>1)
			{
				std::cerr << "WARNING: Mask value mismatch for lens index " << i << " at position (" << xp << ", " << yp << ")." << std::endl;
				std::cerr << "Table's mask value is " << galaxy.at_label("MASK") << ", but saved value is 'true'.\n";
			}
#endif
		}

		if(galaxy_passes_source_filter(galaxy))
		{
			good_indices.push_back(i);
		}
	}

	return good_indices;
}

bool galaxy_passes_lens_filter(const IceBRG::labeled_array<double>::const_row_reference & galaxy)
{
	if(galaxy.at_label("Z_B")>1.6) return false;
	if(!galaxy_passes_global_filter(galaxy)) return false;
	return true;
}
bool galaxy_passes_source_filter(const IceBRG::labeled_array<double>::const_row_reference & galaxy)
{
	if(!galaxy_passes_global_filter(galaxy)) return false;
	return true;
}
bool galaxy_passes_global_filter(const IceBRG::labeled_array<double>::const_row_reference & galaxy)
{
	if(galaxy.at_label("Z_B")>4) return false;
	if(galaxy.at_label("MAG_r")>24.7) return false;
	if(galaxy.at_label("CHI_SQUARED_BPZ")>2) return false;
	//if(galaxy.at_label("ODDS")<0.8) return false;
	return true;
}
