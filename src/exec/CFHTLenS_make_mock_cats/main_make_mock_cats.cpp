/**********************************************************************\
 @file main_make_mock_cats.cpp
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

#include <cassert>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#ifdef _OPENMP
#include <omp.h>
#endif

#include <boost/serialization/vector.hpp>

#include "IceBRG_main/file_access/binary_archive.hpp"
#include "IceBRG_main/file_access/open_file.hpp"
#include "IceBRG_main/file_access/ascii_table_map.hpp"
#include "IceBRG_main/math/misc_math.hpp"
#include "IceBRG_main/math/random/random_functions.hpp"

#include "IceBRG_main/units/unit_conversions.hpp"
#include "IceBRG_lensing/magnification/mag_global_values.h"

#include "get_ra_dec.h"
#include "num_good_pixels.hpp"

#undef MAKE_CALIBRATION_LENSES
#undef SMALL_MOCKS

// Magic values
const std::string data_directory = "/disk2/brg/git/CFHTLenS_cat/Data/";
const std::string mask_directory = data_directory + "masks/";
const std::string field_directory = data_directory + "filtered_tables/";
const std::string fields_list = data_directory + "fields_list.txt";
const std::string pixel_map_root = "_lens_good_pixels.bin";

#ifdef MAKE_CALIBRATION_LENSES

const std::string lens_output_root = "_calibration_lens.dat";
constexpr unsigned num_lenses_to_generate = 141;

#else // #ifdef MAKE_CALIBRATION_MOCKS

#ifdef SMALL_MOCKS

const std::string lens_output_root = "_small_mock_lens.dat";
const std::string source_output_root = "_small_mock_source.dat";
constexpr unsigned num_lenses_to_generate = 40000;
constexpr unsigned num_sources_to_generate = 60000;

#else // #ifdef SMALL_MOCKS

const std::string lens_output_root = "_mock_lens.dat";
const std::string source_output_root = "_mock_source.dat";
constexpr unsigned num_lenses_to_generate = 400000;
constexpr unsigned num_sources_to_generate = 600000;

#endif // #ifdef SMALL_MOCKS // #else

#endif // #ifdef MAKE_CALIBRATION_MOCKS // #else

constexpr double min_lens_z = 0.2;
constexpr double max_lens_z = 1.6;
constexpr double min_source_z = 0.2;
constexpr double max_source_z = 4.0;

constexpr double min_lens_mag = 15;
constexpr double max_lens_mag = 23;
constexpr double min_source_mag = 19;
constexpr double max_source_mag = 25;

constexpr double min_lens_lmsun = 9;
constexpr double max_lens_lmsun = 12;

constexpr double min_T = 1.0;
constexpr double max_T = 6.0;

constexpr double min_good_frac = 0.01;

constexpr int base_seed = 12431;

int main( const int argc, const char *argv[] )
{
	// General setup
	#ifdef _OPENMP
	omp_set_num_threads(5);
	#endif
	
	// Open and read in the fields list
	std::ifstream fi;
	IceBRG::open_file_input(fi,fields_list);

	std::vector<std::string> field_names;
	std::string temp_field_name;

	while(fi>>temp_field_name)
	{
		field_names.push_back(temp_field_name);
	}

	size_t num_fields = field_names.size();

	//num_fields = 1;

	// Loop over all fields
	#ifdef _OPENMP
	#pragma omp parallel for schedule(dynamic)
	#endif
	for(unsigned i=0;i<num_fields;++i)
	{
		// Set up and seed the random generator for this field
		std::ranlux48 gen(base_seed*(i+1));

		std::string & field_name = field_names[i];
		std::string field_name_root = field_name.substr(0,6);

		std::string pixel_map_name = field_directory + field_name_root + pixel_map_root;
		std::string lens_output_file_name = field_directory + field_name_root + lens_output_root;

		const std::vector<std::vector<bool>> good_pixels =
				IceBRG::binary_load<std::vector<std::vector<bool>>>(pixel_map_name);

		const size_t ncol = good_pixels.size();
		assert(ncol>0);
		const size_t nrow = good_pixels[0].size();
		assert(nrow>0);

		assert(static_cast<double>(num_good_pixels(good_pixels))/(ncol*nrow)>min_good_frac);

		std::vector<std::pair<double,double>> galaxy_positions;

#ifdef MAKE_CALIBRATION_LENSES

		// For the calibration lenses, we only need one good position, relatively near the centre

		bool good_point_found = false;
		unsigned counter = 0;

		while((!good_point_found)&&(++counter<10000))
		{
			const double x = IceBRG::drand(ncol/2-counter,ncol/2+counter);
			const double y = IceBRG::drand(nrow/2-counter,nrow/2+counter);

			const unsigned xp = IceBRG::round_int(x);
			const unsigned yp = IceBRG::round_int(y);

			if(good_pixels[xp][yp])
			{
				good_point_found = true;
				for(unsigned j = 0; j< num_lenses_to_generate; ++j)
					galaxy_positions.push_back(std::make_pair(x,y));
			}
		}

		if(!good_point_found) throw std::runtime_error("Could not find good point for calibration lens.");

		unsigned num_lenses_generated = galaxy_positions.size();

#else

		unsigned num_lenses_generated = 0;

		// For each galaxy we want to generate, get good random pixel positions for it
		for(unsigned j=0; j<num_lenses_to_generate; ++j)
		{
			const double x = IceBRG::drand(-0.5,ncol-0.5,gen);
			const double y = IceBRG::drand(-0.5,nrow-0.5,gen);

			const unsigned xp = IceBRG::round_int(x);
			const unsigned yp = IceBRG::round_int(y);

			if(good_pixels[xp][yp])
			{
				galaxy_positions.push_back(std::make_pair(x,y));
				++num_lenses_generated;
				// If not, skip - we don't want to introduce a bias here
			}
		}

#endif

		auto galaxy_sky_positions =
				get_ra_dec(field_name,galaxy_positions);

		// Create the output table, and then add each galaxy to it
		IceBRG::table_map_t<double> field_output_table;


#ifdef MAKE_CALIBRATION_LENSES
		unsigned z100step = 100*(max_lens_z-min_lens_z)/(num_lenses_generated-1);
		if(z100step<1) z100step = 1;
		unsigned z100 = 100*min_lens_z - z100step;
#else
		unsigned z100;
#endif


		for( size_t j=0; j<num_lenses_generated; ++j )
		{
			field_output_table["SeqNr"].push_back(j);
			field_output_table["ra_radians"].push_back(galaxy_sky_positions[j].first.first*IceBRG::unitconv::degtorad);
			field_output_table["dec_radians"].push_back(galaxy_sky_positions[j].first.second*IceBRG::unitconv::degtorad);
			field_output_table["Xpos"].push_back(galaxy_sky_positions[j].second.first);
			field_output_table["Ypos"].push_back(galaxy_sky_positions[j].second.second);
#ifdef MAKE_CALIBRATION_LENSES
			z100 += z100step;
#else
			z100 = 100*IceBRG::drand(min_lens_z,max_lens_z);
#endif
			field_output_table["Z_B"].push_back(z100/100.);
			field_output_table["T_B"].push_back(IceBRG::drand(min_T,max_T));
			field_output_table["ODDS"].push_back(1);
			field_output_table["CHI_SQUARED_BPZ"].push_back(1);

			double Mstel_kg = pow(10,IceBRG::drand(min_lens_lmsun,max_lens_lmsun,gen))*IceBRG::unitconv::Msuntokg;
			field_output_table["Mstel_kg"].push_back(Mstel_kg);
			field_output_table["Mstel_lo_kg"].push_back(Mstel_kg/2);
			field_output_table["Mstel_hi_kg"].push_back(2*Mstel_kg);

			field_output_table["MAG_i"].push_back(IceBRG::drand(min_lens_mag,max_lens_mag,gen));
			field_output_table["MAGERR_i"].push_back(1);
			field_output_table["EXTINCTION_i"].push_back(0);
			field_output_table["MAG_r"].push_back(IceBRG::drand(min_lens_mag,max_lens_mag,gen));
			field_output_table["MAGERR_r"].push_back(1);
			field_output_table["EXTINCTION_r"].push_back(0);
		}

		// Output the table
		#ifdef _OPENMP
		#pragma omp critical(output_mock_lenses)
		#endif
		{
			IceBRG::print_table_map(lens_output_file_name,field_output_table);
			std::cout << "Finished generating " << lens_output_file_name << ".\n";
		}

#ifndef MAKE_CALIBRATION_LENSES

		// Repeat for sources
		std::string source_output_file_name = field_directory + field_name_root + source_output_root;

		galaxy_positions.clear();

		unsigned num_sources_generated = 0;

		// For each galaxy we want to generate, get good random pixel positions for it
		for(unsigned j=0; j<num_sources_to_generate; ++j)
		{
			// Generate a random position and add it if it's inside the mask
			const double x = IceBRG::drand(-0.5,ncol-0.5,gen);
			const double y = IceBRG::drand(-0.5,nrow-0.5,gen);

			const unsigned xp = IceBRG::round_int(x);
			const unsigned yp = IceBRG::round_int(y);

			if(good_pixels[xp][yp])
			{
				galaxy_positions.push_back(std::make_pair(x,y));
				++num_sources_generated;
				// If not, skip - we don't want to introduce a bias here
			}
		}

		galaxy_sky_positions =
				get_ra_dec(field_name,galaxy_positions);

		// Create the output table, and then add each galaxy to it
		field_output_table.clear();

		for( size_t j=0; j<num_sources_generated; ++j )
		{
			field_output_table["SeqNr"].push_back(j);
			field_output_table["ra_radians"].push_back(galaxy_sky_positions[j].first.first*IceBRG::unitconv::degtorad);
			field_output_table["dec_radians"].push_back(galaxy_sky_positions[j].first.second*IceBRG::unitconv::degtorad);
			field_output_table["Xpos"].push_back(galaxy_sky_positions[j].second.first);
			field_output_table["Ypos"].push_back(galaxy_sky_positions[j].second.second);
			unsigned z100 = 100*IceBRG::drand(min_source_z,max_source_z,gen);
			field_output_table["Z_B"].push_back(z100/100.);
			field_output_table["T_B"].push_back(IceBRG::drand(min_T,max_T,gen));
			field_output_table["ODDS"].push_back(1);
			field_output_table["e1"].push_back(0);
			field_output_table["e2"].push_back(0);
			field_output_table["weight"].push_back(1);
			field_output_table["m"].push_back(0);
			field_output_table["c2"].push_back(0);

			double Mstel_kg = pow(10,IceBRG::drand(min_lens_lmsun,max_lens_lmsun,gen))*IceBRG::unitconv::Msuntokg;
			field_output_table["Mstel_kg"].push_back(Mstel_kg);
			field_output_table["Mstel_lo_kg"].push_back(Mstel_kg/2);
			field_output_table["Mstel_hi_kg"].push_back(2*Mstel_kg);

			field_output_table["MAG_i"].push_back(IceBRG::drand(min_source_mag,max_source_mag,gen));
			field_output_table["MAGERR_i"].push_back(1);
			field_output_table["EXTINCTION_i"].push_back(0);
			field_output_table["MAG_r"].push_back(IceBRG::drand(min_source_mag,max_source_mag,gen));
			field_output_table["MAGERR_r"].push_back(1);
			field_output_table["EXTINCTION_r"].push_back(0);
		}

		// Output the table
		#ifdef _OPENMP
		#pragma omp critical(output_mock_sources)
		#endif
		{
			IceBRG::print_table_map(source_output_file_name,field_output_table);
			std::cout << "Finished generating " << source_output_file_name << ".\n";
		}

#endif // #ifndef MAKE_CALIBRATION_LENSES
	}

	return 0;
}
