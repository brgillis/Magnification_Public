/**********************************************************************\
 @file make_output_map.cpp
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


#include <functional>
#include <map>
#include <string>
#include <vector>

#include <boost/lexical_cast.hpp>

#include "IceBRG_main/container/table_typedefs.hpp"

#include "make_output_map.h"

IceBRG::labeled_array<double> make_output_map(const IceBRG::labeled_array<double> & map,
													const std::vector<size_t> & good_indices, const IceBRG::header_t & header_labels,
													const std::map<std::string,std::function<double(double)>> & conversions)
{
	IceBRG::labeled_array<double> result_map;
	result_map.set_labels(header_labels);

	size_t num_new_cols = header_labels.size();

	for(const auto & i : good_indices)
	{
		const auto & galaxy = map.row(i);
		std::vector<double> new_row;

		new_row.reserve(num_new_cols);

		for(const auto & label : header_labels)
		{
			new_row.push_back(galaxy.at_label(label));
		}

		result_map.insert_row(std::move(new_row));
	}

	// Now apply conversions as appropriate
	for(const auto & conv_value_pair : conversions)
	{
		const auto & label = conv_value_pair.first;
		const auto & func = conv_value_pair.second;

		// If this label is in the map, apply the conversion to that column
		try
		{
			auto col = result_map.at_label(label);
			for(auto & val : col)
			{
				val = func(val);
			}
		}
		catch(const std::exception &e)
		{
			// The label isn't in the map most likely.
			std::cerr << "ERROR: Cannot apply conversion for label " << label << ".\n";
			throw;
		}
	}

	return result_map;
}


