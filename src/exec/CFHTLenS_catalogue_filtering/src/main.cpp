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

#include "IceBRG_main/units/unit_conversions.hpp"
#include <cmath>
#include <iostream>
#include <fstream>
#include <functional>
#include <sstream>
#include <string>

#include "IceBRG_main/container/labeled_array.hpp"
#include "IceBRG_main/error_handling.h"
#include "IceBRG_main/file_access/binary_archive.hpp"
#include "IceBRG_main/file_access/open_file.hpp"

#include "correct_redshift_bias.h"
#include "get_filtered_indices.h"
#include "make_output_map.h"

// Magic values
const std::string fields_directory = "/disk2/brg/git/CFHTLenS_cat/Data/";
const std::string fields_list = fields_directory + "fields_list.txt";
const std::string lens_pixel_map_root = "_lens_good_pixels.bin";

int main( const int argc, const char *argv[] )
{

	// Open and read in the fields list
	std::ifstream fi;
	IceBRG::open_file_input(fi,fields_list);

	std::vector<std::string> field_names;

	std::string field_name;

	while(fi>>field_name)
	{
		field_names.push_back(field_name);
	}

	fi.close();

	// Load each field in turn and process it

	size_t num_fields = field_names.size();

	//num_fields = 1;

	#ifdef _OPENMP
	#pragma omp parallel for schedule(dynamic)
	#endif
	for(size_t field_i=0;field_i<num_fields;++field_i)
	{
		std::string field_name = field_names[field_i];
		std::string field_name_root = field_name.substr(0,6);

		// Get the input file name
		std::stringstream ss("");
		ss << fields_directory << "full_tables/" << field_name << ".dat";
		std::string input_file_name = ss.str();

		// Get the name of the mask file for this table and load it
#if(1)
		ss.str("");
		ss << fields_directory << "filtered_tables/" << field_name_root << lens_pixel_map_root;
		const std::string lens_pixel_map_file_name = ss.str();

		std::vector<std::vector<bool>> good_pixels;

		try
		{
			good_pixels = IceBRG::binary_load<std::vector<std::vector<bool>>>(
				lens_pixel_map_file_name);
		}
		catch( const std::exception &e )
		{
			std::cerr << "Mask file " << lens_pixel_map_file_name << " cannot be loaded.\n";
			throw;
		}
#endif

		// Get the lens and source output file names
#if(1)
		ss.str("");
		ss << fields_directory << "filtered_tables/" << field_name_root << "_lens.dat";
		const std::string lens_output_name = ss.str();

		ss.str("");
		ss << fields_directory << "filtered_tables/" << field_name_root << "_source.dat";
		const std::string source_output_name = ss.str();
#endif

		// Load in the input file
		IceBRG::labeled_array<double> table;
		try
		{
			table.load(input_file_name); // TODO Check we're properly loading
		}
		catch(const std::runtime_error &e)
		{
			IceBRG::handle_error_message(e.what());
			continue;
		}

		// Rename the y magnitude column i if it's present
		move_y_column_to_i(table);

		// Lens file
		std::vector<size_t> good_indices(get_good_lenses(table,good_pixels));

		// Set up the header columns vector for the ones we want to output
		IceBRG::header_t lens_header_columns;
		lens_header_columns.push_back("SeqNr");
		lens_header_columns.push_back("ALPHA_J2000");
		lens_header_columns.push_back("DELTA_J2000");
		lens_header_columns.push_back("Xpos");
		lens_header_columns.push_back("Ypos");
		lens_header_columns.push_back("Z_B");
		lens_header_columns.push_back("T_B");
		lens_header_columns.push_back("ODDS");
		lens_header_columns.push_back("CHI_SQUARED_BPZ");
		lens_header_columns.push_back("LP_log10_SM_MED");
		lens_header_columns.push_back("LP_log10_SM_INF");
		lens_header_columns.push_back("LP_log10_SM_SUP");
		lens_header_columns.push_back("MAG_i");
		lens_header_columns.push_back("MAGERR_i");
		lens_header_columns.push_back("EXTINCTION_i");
		lens_header_columns.push_back("MAG_r");
		lens_header_columns.push_back("MAGERR_r");
		lens_header_columns.push_back("EXTINCTION_r");

		// Set up a map of the conversions to apply
		std::map<std::string,std::function<double(double)>> lens_conversions;
		auto deg_to_rad = [] (double theta) {return theta*IceBRG::unitconv::degtorad;};
		auto l10_Msun_to_kg = [] (double l10_Msun)
				{return std::pow(10.,l10_Msun)*IceBRG::unitconv::Msuntokg;};
		lens_conversions["ALPHA_J2000"] = deg_to_rad;
		lens_conversions["DELTA_J2000"] = deg_to_rad;
		lens_conversions["LP_log10_SM_MED"] = l10_Msun_to_kg;
		lens_conversions["LP_log10_SM_INF"] = l10_Msun_to_kg;
		lens_conversions["LP_log10_SM_SUP"] = l10_Msun_to_kg;

		IceBRG::labeled_array<double> output_map(make_output_map(table,good_indices,lens_header_columns,
				lens_conversions));

		// Rename columns we've applied unit convesions to
		output_map.change_label("ALPHA_J2000","ra_radians");
		output_map.change_label("DELTA_J2000","dec_radians");
		output_map.change_label("LP_log10_SM_MED","Mstel_kg");
		output_map.change_label("LP_log10_SM_INF","Mstel_lo_kg");
		output_map.change_label("LP_log10_SM_SUP","Mstel_hi_kg");

		//correct_redshift_bias(output_map.at("Z_B"));

		output_map.save(lens_output_name,true);

		// Source file

		good_indices = get_good_sources(table, good_pixels);

		// Set up the header columns vector for the ones we want to output
		IceBRG::header_t source_header_columns;
		source_header_columns.push_back("SeqNr");
		source_header_columns.push_back("ALPHA_J2000");
		source_header_columns.push_back("DELTA_J2000");
		source_header_columns.push_back("Xpos");
		source_header_columns.push_back("Ypos");
		source_header_columns.push_back("Z_B");
		source_header_columns.push_back("T_B");
		source_header_columns.push_back("ODDS");
		source_header_columns.push_back("CHI_SQUARED_BPZ");
		source_header_columns.push_back("e1");
		source_header_columns.push_back("e2");
		source_header_columns.push_back("weight");
		source_header_columns.push_back("m");
		source_header_columns.push_back("c2");
		source_header_columns.push_back("LP_log10_SM_MED");
		source_header_columns.push_back("LP_log10_SM_INF");
		source_header_columns.push_back("LP_log10_SM_SUP");
		source_header_columns.push_back("MAG_i");
		source_header_columns.push_back("MAGERR_i");
		source_header_columns.push_back("EXTINCTION_i");
		source_header_columns.push_back("MAG_r");
		source_header_columns.push_back("MAGERR_r");
		source_header_columns.push_back("EXTINCTION_r");

		// Set up a map of the conversions to apply
		std::map<std::string,std::function<double(double)>> source_conversions;
		source_conversions["ALPHA_J2000"] = deg_to_rad;
		source_conversions["DELTA_J2000"] = deg_to_rad;
		source_conversions["LP_log10_SM_MED"] = l10_Msun_to_kg;
		source_conversions["LP_log10_SM_INF"] = l10_Msun_to_kg;
		source_conversions["LP_log10_SM_SUP"] = l10_Msun_to_kg;

		output_map = make_output_map(table,good_indices,source_header_columns,source_conversions);

		// Rename columns we've applied unit convesions to
		output_map.change_label("ALPHA_J2000","ra_radians");
		output_map.change_label("DELTA_J2000","dec_radians");
		output_map.change_label("LP_log10_SM_MED","Mstel_kg");
		output_map.change_label("LP_log10_SM_INF","Mstel_lo_kg");
		output_map.change_label("LP_log10_SM_SUP","Mstel_hi_kg");

		//correct_redshift_bias(output_map.at("Z_B"));

		output_map.save(source_output_name,true);

#ifdef _OPENMP
#pragma omp critical(CFHTLenS_catalogue_filtering_print_tables)
#endif
		{
			std::cout << "Finished filtering " << field_name_root << "!\n";
		}

	}

	std::cout << "Done!\n";

	return 0;
}

