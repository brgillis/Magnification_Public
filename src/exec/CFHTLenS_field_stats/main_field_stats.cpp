/**********************************************************************\
 @file main_field_stats.cpp
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

#include <iostream>
#include <omp.h>
#include <string>
#include <vector>

#include <Eigen/Core>

#include "IceBRG_main/container/labeled_array.hpp"
#include "IceBRG_main/file_access/ascii_table_map.hpp"
#include "IceBRG_main/file_access/binary_archive.hpp"
#include "IceBRG_main/file_access/open_file.hpp"
#include "IceBRG_main/join_path.hpp"
#include "IceBRG_main/math/misc_math.hpp"
#include "IceBRG_main/units/unit_conversions.hpp"
#include "IceBRG_main/vector/limit_vector.hpp"

#include "get_data_directory.hpp"
#include "magic_values.hpp"

#define MAKE_WEIGHT_TABLE

using namespace IceBRG;

// Magic values

#ifdef MAKE_WEIGHT_TABLE
const std::string output_filename = "field_lens_weights.dat";
constexpr unsigned z_bins = 110;
#else
const std::string output_filename = "field_stats.dat";
constexpr unsigned z_bins = 11;
#endif

inline size_t num_good_pixels(const std::vector<std::vector<bool>> & input)
{
	size_t result(0);

	unsigned ncol = input.size();
	assert(ncol>0);
	unsigned nrow = input[0].size();

	for(unsigned i=0; i<ncol; ++i)
	{
		for(unsigned j=0; j<nrow; ++j)
		{
			if(input[i][j]) ++result;
		}
	}

	return result;
}

/**
 * TODO (description)
 *
 * @param TODO (params to be passed at command-line)
 * @return
 */
int main( const int argc, const char *argv[] )
{

#ifdef _OPENMP
	omp_set_num_threads(5);
#endif

	limit_vector<double> z_limits(z_bin_min,z_bin_max,z_bins);

	// Set up output data table
	labeled_array<double> output_table;

	// Open and read in the fields list and get the data directory
	std::ifstream fi;
	const std::string data_directory = get_data_directory(argc,argv,fi);

	const std::string field_directory = join_path(data_directory,field_subdirectory);
	const std::string output_file = join_path(data_directory,output_filename);

	std::vector<std::string> field_names;
	std::vector<std::string> output_header;

	#ifdef MAKE_WEIGHT_TABLE
	output_header.push_back("z_bin_min");
	output_table.insert_col(z_limits.lower_limits());
	#endif

	std::string temp_field_name;

	while(fi>>temp_field_name)
	{
		field_names.push_back(temp_field_name);

		#ifdef MAKE_WEIGHT_TABLE
		output_header.push_back(temp_field_name.substr(0,6));
		#endif
	}

	unsigned num_fields = field_names.size();

	#ifndef MAKE_WEIGHT_TABLE

	std::stringstream ss;

	output_header.push_back("field_index");
	for(unsigned i=0; i<z_limits.num_bins(); ++i)
	{
		ss.str("");
		ss << "lens_dens_z_" << z_limits.lower_limit(i);
		output_header.push_back(ss.str());
	}
	#endif // #ifdef MAKE_WEIGHT_TABLE

	unsigned num_completed_fields = 0;

	//num_fields = 1;

	#ifdef _OPENMP
	#pragma omp parallel for schedule(dynamic)
	#endif
	for(unsigned field_i=0;field_i<num_fields;++field_i)
	{
		const std::string & field_name = field_names[field_i];
		const std::string field_name_root = field_name.substr(0,6);

		std::stringstream ss;

		ss.str("");
		ss << field_directory << field_name_root << pixel_map_tail;
		const std::string pixel_map_file_name = ss.str();

		// Get the size of this field

		std::vector<std::vector<bool>> good_pixels;

		good_pixels = binary_load<std::vector<std::vector<bool>>>(pixel_map_file_name);
		// Get the good size of this field now
		size_t num_good = num_good_pixels(good_pixels);
		const double field_size = square(rad_per_px)*num_good;

		// Get the lens file name
		ss.str("");
		ss << field_directory << field_name_root << lens_tail;
		const std::string lens_file_name = ss.str();

		labeled_array<double> lens_table;

		lens_table.load(lens_file_name);

		// Loop over lenses and bin them by redshift
		Eigen::ArrayXd lens_count = Eigen::ArrayXd::Zero(z_bins);

		for(const auto & lens : lens_table.rows())
		{
			double z = lens.at_label("Z_B");
			if(z_limits.inside_limits(z))
				lens_count[z_limits.get_bin_index(z)] += 1;
		}

		Eigen::ArrayXd lens_dens = lens_count/field_size;

#ifndef MAKE_WEIGHT_TABLE

		std::vector<double> output_row;
		output_row.reserve(lens_dens.size()+1);

		output_row.push_back(field_i);

		for(int i=0; i<lens_dens.size(); ++i)
			output_row.push_back(lens_dens(i));

#endif // #ifndef MAKE_WEIGHT_TABLE

		// Add this data to the output table

		#ifdef _OPENMP
		#pragma omp critical(update_field_stats_output)
		#endif
		{
			#ifdef MAKE_WEIGHT_TABLE
			output_table.insert_col(lens_dens);
			#else
			output_table.insert_row(output_row);
			#endif
			std::cout << "Field " << field_name_root << " (#" <<
					++num_completed_fields << "/" << num_fields << ") complete.\n";
		}
	}

	output_table.set_labels(output_header);

	output_table.save(output_file);

	std::cout << "Done!\nOutput saved to " << output_file << "." << std::endl;

	return 0;
}
