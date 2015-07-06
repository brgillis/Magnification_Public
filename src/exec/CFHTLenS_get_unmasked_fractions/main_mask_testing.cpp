/**********************************************************************\
 @file main.cpp
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

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <functional>
#include <sstream>
#include <string>
#include <unordered_map>

#include <boost/lexical_cast.hpp>
#include <boost/serialization/vector.hpp>

#include <CCfits/CCfits>

#include "IceBRG_main/external/sgsmooth.h"
#include "IceBRG_main/file_access/ascii_table_map.hpp"
#include "IceBRG_main/file_access/binary_archive.hpp"
#include "IceBRG_main/file_access/open_file.hpp"
#include "IceBRG_main/join_path.hpp"
#include "IceBRG_main/math/misc_math.hpp"
#include "IceBRG_main/math/random/random_functions.hpp"
#include "IceBRG_main/vector/limit_vector.hpp"

#include "IceBRG_physics/astro.h"
#include "IceBRG_main/units/units.hpp"
#include "IceBRG_main/units/unit_conversions.hpp"

#include "get_data_directory.hpp"
#include "magic_values.hpp"

#include "get_good_positions.hpp"
#include "load_pixel_table.h"

using namespace IceBRG;

// Magic values

#undef USE_CALIBRATION
#undef USE_MOCKS

#ifdef USE_CALIBRATION

const std::string lens_root = "_calibration_lens.dat";
const std::string lens_output_root = "_calibration_lens_mask_frac.dat";

#else // #ifdef USE_CALIBRATION

#ifdef USE_MOCKS
const std::string lens_root = "_small_mock_lens.dat";
const std::string lens_output_root = "_small_mock_lens_mask_frac.dat";
#else
const std::string lens_root = "_lens.dat";
const std::string lens_output_root = "_lens_mask_frac.dat";
#endif

#endif // #ifdef USE_CALIBRATION // #else

#define USE_SAVED_MASK

/*
 * Max physical separation is 2000 kpc.
 * At redshift 0.2 this is 601.5"
 * Pixel scale is 0.186"/px
 * Maximum pixel separation we care about is thus 3235 px.
 */

#ifdef USE_CALIBRATION

constexpr float min_kpc_sep=0;
constexpr float max_kpc_sep=25000;
constexpr float num_sep_steps = 1;
#define COUNTING_TYPE long unsigned

#else

constexpr float min_kpc_sep=0;
constexpr float max_kpc_sep=2000;
constexpr float num_sep_steps = 40;
#define COUNTING_TYPE unsigned

#endif

constexpr unsigned short sampling_factor=4;

static_assert(sampling_factor>0,"Subsample must be positive.");

constexpr short unsigned sg_window_small = 2;
constexpr short unsigned sg_window_large = 5;
constexpr short unsigned sg_deg = 3;

inline void _increment_bin(unsigned index, bool good,
					std::vector<COUNTING_TYPE> & total_px_per_bin,
					std::vector<COUNTING_TYPE> & lens_total_px_per_bin,
					std::vector<COUNTING_TYPE> & good_px_per_bin,
					std::vector<COUNTING_TYPE> & lens_good_px_per_bin)
{
	++total_px_per_bin[index];
	++lens_total_px_per_bin[index];
	if(good)
	{
		++good_px_per_bin[index];
		++lens_good_px_per_bin[index];
	}
}

int main( const int argc, const char *argv[] )
{
	using namespace IceBRG;

	std::ifstream fi; // Will be loaded with fields list first
	const std::string data_directory = get_data_directory(argc,argv,fi);

	const std::string mask_directory = join_path(data_directory,mask_subdirectory);
	const std::string field_directory = join_path(data_directory,field_subdirectory);

	// Set up separation limits vector
	const limit_vector<float> sep_limits(min_kpc_sep,max_kpc_sep,num_sep_steps);

	const unsigned num_bins = sep_limits.num_bins();

	std::vector<std::string> field_names;
	std::string temp_field_name;

	while(fi>>temp_field_name)
	{
		field_names.push_back(temp_field_name);
	}

	// Set up map for each field's average separation hists
	table_map_t<double> mean_results;

	// Add a column to the result table which gives pixel limits
	std::vector<double> bin_mids(num_bins);
	for(unsigned i=0; i<num_bins; ++i)
	{
		bin_mids[i] = (sep_limits.lower_limit(i)+sep_limits.upper_limit(i))/2;
	}
	mean_results["bin_mid_kpc"] = bin_mids;

	// Set up the field sizes table
	table_map_t<double> field_sizes;

	size_t num_fields = field_names.size();

	//num_fields = 1;

	unsigned num_finished_fields = 0;

	#ifdef _OPENMP
	#pragma omp parallel for schedule(dynamic)
	#endif
	for(unsigned field_i=0;field_i<num_fields;++field_i)
	{
		const std::string & field_name = field_names[field_i];
		const std::string field_name_root = field_name.substr(0,6);

		// Get the input file name
		std::stringstream ss("");
		ss << mask_directory << field_name << ".fits.fz";
		const std::string input_file_name = ss.str();

		// Get the lens pixel map file name
		ss.str("");
		ss << field_directory << field_name_root << pixel_map_tail;
		const std::string pixel_map_file_name = ss.str();

		std::vector<std::vector<bool>> good_pixels;

		try
		{
			#ifdef USE_SAVED_MASK
			good_pixels = binary_load<std::vector<std::vector<bool>>>(
					pixel_map_file_name);
			#else
			good_pixels = load_pixel_table(input_file_name);
			#endif
		}
		catch(const CCfits::FitsError &e)
		{
			std::cerr << "Cannot open file " << input_file_name << "!\n"
					<< e.message() << std::endl;
			continue;
		}
		catch(const std::runtime_error &e)
		{
			std::cerr << "Cannot process file " << input_file_name << "!\n"
					<< e.what() << std::endl;
			continue;
		}

//#ifdef _OPENMP
//		#pragma omp critical(mask_status_update)
//#endif
//		{
//			std::cout << "Mask data loaded for " << field_name_root << ".\n";
//		}

		auto is_good_position = [&] (int i, int j) -> bool
		{
			if((i<0)||(j<0)) return false;
			if(static_cast<unsigned>(i)>=good_pixels.size()) return false;
			if(static_cast<unsigned>(j)>=good_pixels[0].size()) return false;

			return good_pixels[i][j];
		};

		// We'll use that table for possible source positions (since we care about where
		// they could be, and not just where they are). For lenses, we can save time_type
		// by using only the actual positions of galaxies.

		// Get the lens table file name
		ss.str("");
		ss << field_directory << field_name_root << lens_root;
		std::string lens_file_name = ss.str();

		// Get the lens output file name
		ss.str("");
		ss << field_directory << field_name_root << lens_output_root;
		std::string lens_output_file_name = ss.str();

		// Load the lens table
		const auto lens_table_map = load_table_map<double>(lens_file_name);

		// Set up vectors to store result data
		std::vector<COUNTING_TYPE> good_px_per_bin(num_bins+2,0); // Add 2 to allow potential over/underflow
		std::vector<COUNTING_TYPE> total_px_per_bin(num_bins+2,0);

		// Set up vectors for the lens's masked fraction
		std::vector<COUNTING_TYPE> lens_good_px_per_bin(num_bins+2,0); // Add 2 to allow potential over/underflow
		std::vector<COUNTING_TYPE> lens_total_px_per_bin(num_bins+2,0);

		// Set up a map for each lens's info
		table_map_t<double> field_result;

		// Add column to this table for bin middles
		field_result["bin_mid_kpc"] = bin_mids;

		// Loop through lens positions now to calculate data
		unsigned num_lenses = lens_table_map.at("Xpos").size();

		auto increment_bin = [&] (unsigned index, bool good)
		{
			_increment_bin(index,good,total_px_per_bin,lens_total_px_per_bin,good_px_per_bin,lens_good_px_per_bin);
		};

		for(size_t lens_i = 0; lens_i<num_lenses; ++lens_i)
		{

			// Get the lens's redshift and calculate related information
			const float lens_z = lens_table_map.at("Z_B")[lens_i];
			if(lens_z>lens_z_max) continue;
			if(lens_z<lens_z_min) continue;
			const mass_type lens_m = units_cast<mass_type>(lens_table_map.at("Mstel_kg")[lens_i]);
			if(lens_m<lens_m_min) continue;
			if(lens_m>lens_m_max) continue;

			// Get the lens's position on the image
			const unsigned lens_xp = round_int(lens_table_map.at("Xpos")[lens_i]);
			const unsigned lens_yp = round_int(lens_table_map.at("Ypos")[lens_i]);

			// Initialize per-lens vectors
			for(auto & elem : lens_good_px_per_bin) elem = 0;
			for(auto & elem : lens_total_px_per_bin) elem = 0;

			const double pxfd_fact = value_of(afd(lens_z))/rad_per_px;

			const int lens_max_px_sep = pxfd_fact*max_kpc_sep+1;

			// Loop over possible source positions

			// Start with those along horizontal and vertical directions
			for(int i=1; i<lens_max_px_sep; i+=sampling_factor)
			{
				const float d = i;
				unsigned bin_i = sep_limits.get_bin_index(d/pxfd_fact);

				increment_bin(bin_i,is_good_position(lens_xp+i,lens_yp));
				increment_bin(bin_i,is_good_position(lens_xp-i,lens_yp));
				increment_bin(bin_i,is_good_position(lens_xp,lens_yp+i));
				increment_bin(bin_i,is_good_position(lens_xp,lens_yp-i));
			}

			// Now do diagonals
			for(int i=1; i<lens_max_px_sep/std::sqrt(2.); i+=sampling_factor)
			{
				const float d = i*std::sqrt(2.);
				unsigned bin_i = sep_limits.get_bin_index(d/pxfd_fact);

				increment_bin(bin_i,is_good_position(lens_xp+i,lens_yp+i));
				increment_bin(bin_i,is_good_position(lens_xp-i,lens_yp+i));
				increment_bin(bin_i,is_good_position(lens_xp+i,lens_yp-i));
				increment_bin(bin_i,is_good_position(lens_xp-i,lens_yp-i));
			}

			// And now all other positions
			for(int i=2; i<lens_max_px_sep; i+=sampling_factor)
			{
				// Loop over possible source positions
				for(int j=1; j<i; j+=sampling_factor)
				{
					const float d = dist2d<float,float>(i,j);
					if(d>lens_max_px_sep) continue;
					unsigned bin_i = sep_limits.get_bin_index(d/pxfd_fact);

					increment_bin(bin_i,is_good_position(lens_xp+i,lens_yp+j));
					increment_bin(bin_i,is_good_position(lens_xp-i,lens_yp+j));
					increment_bin(bin_i,is_good_position(lens_xp+i,lens_yp-j));
					increment_bin(bin_i,is_good_position(lens_xp-i,lens_yp-j));

					increment_bin(bin_i,is_good_position(lens_xp+j,lens_yp+i));
					increment_bin(bin_i,is_good_position(lens_xp-j,lens_yp+i));
					increment_bin(bin_i,is_good_position(lens_xp+j,lens_yp-i));
					increment_bin(bin_i,is_good_position(lens_xp-j,lens_yp-i));

				}
			}

			//print_pixels<decltype(tag_vector),unsigned short>("tagged_pixels.dat",tag_vector);

			// Set up result vector for this lens
			std::vector<double> lens_res(num_bins);
			for(unsigned i=0; i<num_bins; ++i)
			{
				if(static_cast<float>(lens_total_px_per_bin[i])==0)
					lens_res[i] = 1;
				else
					lens_res[i] = static_cast<float>(lens_good_px_per_bin[i])/
						static_cast<float>(lens_total_px_per_bin[i]);

//				std::cout << "Annulus unmasked area_type: " << lens_res[i] * pi *
//					(square(sep_limits.upper_limit(i)*pxfd_fact*rad_per_px)-
//						square(sep_limits.lower_limit(i))*pxfd_fact*rad_per_px) << std::endl;
			}

/*			// Smooth it

			if(sampling_factor>2)
			{
				lens_res = sg_smooth(lens_res,sg_window_large,sg_deg);
			}
			else
			{
				lens_res = sg_smooth(lens_res,sg_window_small,sg_deg);
			}*/

			// Add this to the field table map
			field_result[boost::lexical_cast<std::string>(lens_table_map.at("SeqNr")[lens_i])] =
					lens_res;
		}

		// Set up result vector for this field
		std::vector<double> field_res(num_bins);
		for(unsigned i=0; i<num_bins; ++i)
		{
			if(static_cast<float>(total_px_per_bin[i])==0)
				field_res[i] = 1;
			else
				field_res[i] = static_cast<float>(good_px_per_bin[i])/
				static_cast<float>(total_px_per_bin[i]);
		}

		// Get the good size of this field now
		size_t num_good = num_good_pixels(good_pixels);
		const double good_size_upper = square(rad_per_px)*num_good;
		const double good_size_lower = good_size_upper;
		std::vector<double> size_measures;
		size_measures.push_back(good_size_lower);
		size_measures.push_back(good_size_upper);

		// Add this to the full result map and output this table

#ifdef _OPENMP
		#pragma omp critical(add_mask_test_results_to_map)
#endif
		{
			mean_results[field_name_root] = field_res;
			field_sizes[field_name_root] = size_measures;

			print_table_map(lens_output_file_name,field_result);

			// Archive the good pixel map if we just calculated it
			#ifndef USE_SAVED_MASK
			binary_save(pixel_map_file_name,good_pixels);
			#endif

			std::cout << "Finished processing field " << field_name_root << " (#" << ++num_finished_fields << "/" <<
				num_fields << ")!\n";
		}
	}

	// Get the output file name
	std::stringstream ss("");
	ss << mask_directory << "position_corr.dat";
	std::string output_name = ss.str();

	print_table_map(output_name,mean_results);

	ss.str("");
	ss << mask_directory << "field_sizes.dat";
	std::string field_sizes_name = ss.str();

	print_table_map(field_sizes_name,field_sizes);

	std::cout << "Done!\n";

	return 0;
}

