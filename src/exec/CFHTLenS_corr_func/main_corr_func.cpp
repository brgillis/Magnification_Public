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

#include <fstream>
#include <iostream>
#include <tuple>
#include <vector>

#include <boost/lexical_cast.hpp>

#include <Eigen/Core>

#include "IceBRG_main/container/coerce.hpp"
#include "IceBRG_main/container/labeled_array.hpp"
#include "IceBRG_main/file_access/open_file.hpp"
#include "IceBRG_main/file_access/ascii_table_map.hpp"
#include "IceBRG_main/units/unit_conversions.hpp"
#include "IceBRG_main/units/unitconv_map.hpp"
#include "IceBRG_main/utility.hpp"
#include "IceBRG_main/vector/limit_vector.hpp"
#include "IceBRG_lensing/magnification/mag_global_values.h"

#include "corr_func_bin.hpp"
#include "corr_func_config.h"

#undef SMALL_MOCKS

#define LIMIT_TO_MONOPOLE

#define USE_FIELD_WEIGHTING

#include "IceBRG_physics/lensing_correlation_function_estimator.h"
#include "IceBRG_physics/correlation_function_estimator.h"

// Magic values
std::string data_directory = "/disk2/brg/git/CFHTLenS_cat/Data/";
std::string fields_list = data_directory + "fields_list.txt";

const std::string lens_root = "_lens.dat";
const std::string source_root = "_source.dat";

constexpr int reserve_factor = 10000;

#ifdef USE_FIELD_WEIGHTING
const std::string lens_weight_file = data_directory + "field_lens_weights.dat";
#endif

int main( const int argc, const char *argv[] )
{
	using namespace IceBRG;

	// Get the configuration file from the command-line arguments
	const corr_func_config config(argc,argv);

	const short lensing_style = config.lensing_style;

	flt_type source_z_min = IceBRG::mag_z_min;
	flt_type source_z_max = IceBRG::mag_z_max;

	if(!lensing_style)
	{
		source_z_min = config.z_min;
		source_z_max = config.z_max;
	}

	// Set up file names based on this
	const std::string output_name;

	#ifdef SMALL_MOCKS

	if(lensing_style==2)
		const_cast<std::string &>(output_name) = data_directory + "magf_corr_funcs_quick.dat";
	else if(lensing_style==1)
		const_cast<std::string &>(output_name) = data_directory + "lensing_corr_funcs_quick.dat";
	else
		const_cast<std::string &>(output_name) = data_directory + "auto_corr_funcs_quick.dat";

	const std::string lens_mock_root = "_small_mock_lens.dat";
	const std::string source_mock_root = "_small_mock_source.dat";

	#else // #ifdef SMALL_MOCKS

	if(lensing_style==2)
		const_cast<std::string &>(output_name) = data_directory + "magf_corr_funcs.dat";
	else if(lensing_style==1)
		const_cast<std::string &>(output_name) = data_directory + "lensing_corr_funcs.dat";
	else
		const_cast<std::string &>(output_name) = data_directory + "auto_corr_funcs.dat";

	const std::string lens_mock_root = "_mock_lens.dat";
	const std::string source_mock_root = "_mock_source.dat";

	#endif // #ifdef SMALL_MOCKS // #else


	// Set up global data

	auto R_lin_or_log = config.R_log ? IceBRG::limit_vector<distance_type>::type::LOG : IceBRG::limit_vector<distance_type>::type::LINEAR;
	IceBRG::limit_vector<distance_type> R_limits(R_lin_or_log, config.R_min,config.R_max,config.R_bins);

	auto z_lin_or_log = config.z_log ? IceBRG::limit_vector<flt_type>::type::LOG : IceBRG::limit_vector<flt_type>::type::LINEAR;
	IceBRG::limit_vector<flt_type> lens_z_limits(z_lin_or_log, config.z_min,config.z_max,config.z_bins);

	auto m_lin_or_log = config.m_log ? IceBRG::limit_vector<mass_type>::type::LOG : IceBRG::limit_vector<mass_type>::type::LINEAR;
	IceBRG::limit_vector<mass_type> lens_m_limits(m_lin_or_log, config.m_min,config.m_max,config.m_bins);

	auto mag_lin_or_log = config.mag_log ? IceBRG::limit_vector<flt_type>::type::LOG : IceBRG::limit_vector<flt_type>::type::LINEAR;
	IceBRG::limit_vector<flt_type> lens_mag_limits(mag_lin_or_log, config.mag_min,config.mag_max,config.mag_bins);


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

	#ifdef USE_FIELD_WEIGHTING
	// Load the lens weight table
	const IceBRG::labeled_array<flt_type> lens_weight_table(lens_weight_file);
	auto lens_weight_z_limits_builder = IceBRG::coerce<std::vector<flt_type>>(lens_weight_table.at_label("z_bin_min"));
	lens_weight_z_limits_builder.push_back(2*lens_weight_z_limits_builder.back()-
										   lens_weight_z_limits_builder.at(lens_weight_z_limits_builder.size()-2));
	const IceBRG::limit_vector<flt_type> lens_weight_z_limits(std::move(lens_weight_z_limits_builder));
	#endif

	typedef std::vector<std::tuple<angle_type,angle_type,flt_type,flt_type>> pos_vec;

	// Make the multi-vector of bin sums

	corr_func_bin template_bin(R_limits.num_bins());
	template_bin.lensing_style = config.lensing_style;
	template_bin.z_buffer = config.z_buffer;
	template_bin.lens_positions = pos_vec();

	std::vector<std::vector<std::vector<corr_func_bin>>> corr_func_bin_sums;

	IceBRG::make_vector_value(corr_func_bin_sums,
								template_bin,
								lens_z_limits.num_bins(),
								lens_m_limits.num_bins(),
								lens_mag_limits.num_bins());

	// Load in source, mock lens, and mock source data, which will each be common to each redshift bin
	size_t num_fields = field_names.size();
	//num_fields = 1;

	// Load each field in turn and process it
	size_t num_processed = 0;

	#ifdef _OPENMP
	#pragma omp parallel for schedule(dynamic)
	#endif
	for(size_t field_i=0;field_i<num_fields;++field_i)
	{
		std::vector<std::vector<std::vector<corr_func_bin>>> corr_func_bins;

		IceBRG::make_vector_value(corr_func_bins,
									template_bin,
									lens_z_limits.num_bins(),
									lens_m_limits.num_bins(),
									lens_mag_limits.num_bins());

		std::vector<pos_vec> source_positions(1);
		if(!lensing_style) source_positions.resize(lens_z_limits.num_bins());
		std::vector<pos_vec> mock_lens_positions(lens_z_limits.num_bins());
		std::vector<pos_vec> mock_source_positions(1);
		if(!lensing_style) mock_source_positions.resize(lens_z_limits.num_bins());

		for(auto & pos : source_positions)
			pos.reserve(reserve_factor);
		for(auto & pos : mock_lens_positions)
			pos.reserve(reserve_factor);
		for(auto & pos : mock_source_positions)
			pos.reserve(reserve_factor);

		std::string field_name_root = field_names[field_i].substr(0,6);

		std::stringstream ss("");
		ss << data_directory << "filtered_tables/" << field_name_root << lens_root;
		std::string lens_input_name = ss.str();

		{
			const IceBRG::labeled_array<flt_type> gals(lens_input_name);

			int z_col = gals.get_index_for_label("Z_B");
			int mag_col = gals.get_index_for_label("MAG_r");
			int m_col = gals.get_index_for_label("Mstel_kg");
			int ra_col = gals.get_index_for_label("ra_radians");
			int dec_col = gals.get_index_for_label("dec_radians");

			for(const auto & gal : gals.rows())
			{
				const flt_type & z = gal.at(z_col);
				if(lens_z_limits.outside_limits(z)) continue;
				const mass_type m = gal.at(m_col)*kg;
				if(lens_m_limits.outside_limits(m)) continue;
				const flt_type & mag = gal.at(mag_col);
				if(lens_mag_limits.outside_limits(mag)) continue;

				auto pos = std::make_tuple(gal.at(ra_col)*rad,gal.at(dec_col)*rad,z,mag);

				corr_func_bins.at(lens_z_limits.get_bin_index(z)).at(lens_m_limits.get_bin_index(m)).at(
					lens_mag_limits.get_bin_index(mag)).lens_positions->push_back(std::move(pos));
			}
		}

		ss.str("");
		ss << data_directory << "filtered_tables/" << field_name_root << source_root;
		std::string source_input_name = ss.str();

		{
			const IceBRG::labeled_array<flt_type> gals(source_input_name);

			int z_col = gals.get_index_for_label("Z_B");
			int mag_col = gals.get_index_for_label("MAG_r");
			int ra_col = gals.get_index_for_label("ra_radians");
			int dec_col = gals.get_index_for_label("dec_radians");

			for(const auto & gal : gals.rows())
			{
				const flt_type & z = gal.at(z_col);
				if((z<source_z_min)||(z>source_z_max)) continue;
				const flt_type & mag = gal.at(mag_col);
				if((mag<IceBRG::mag_m_min)||(mag>IceBRG::mag_m_max)) continue;

				auto pos = std::make_tuple(gal.at(ra_col)*rad,gal.at(dec_col)*rad,z,mag);

				if(lensing_style)
				{
					source_positions[0].push_back(std::move(pos));
				}
				else
				{
					if(lens_z_limits.outside_limits(z)) continue;
					source_positions[lens_z_limits.get_bin_index(z)].push_back(std::move(pos));
				}
			}
		}

		ss.str("");
		ss << data_directory << "filtered_tables/" << field_name_root << lens_mock_root;
		std::string mock_lens_input_name = ss.str();

		{
			const IceBRG::labeled_array<flt_type> gals(mock_lens_input_name);

			int z_col = gals.get_index_for_label("Z_B");
			int mag_col = gals.get_index_for_label("MAG_r");
			int ra_col = gals.get_index_for_label("ra_radians");
			int dec_col = gals.get_index_for_label("dec_radians");

			for(const auto & gal : gals.rows())
			{
				const flt_type & z = gal.at(z_col);
				if(lens_z_limits.outside_limits(z)) continue;
				const flt_type & mag = gal.at(mag_col);

				auto pos = std::make_tuple(gal.at(ra_col)*rad,gal.at(dec_col)*rad,z,mag);
				mock_lens_positions[lens_z_limits.get_bin_index(z)].push_back(std::move(pos));
			}
		}

		ss.str("");
		ss << data_directory << "filtered_tables/" << field_name_root << source_mock_root;
		std::string mock_source_input_name = ss.str();

		{
			const IceBRG::labeled_array<flt_type> gals(mock_source_input_name);

			int z_col = gals.get_index_for_label("Z_B");
			int mag_col = gals.get_index_for_label("MAG_r");
			int ra_col = gals.get_index_for_label("ra_radians");
			int dec_col = gals.get_index_for_label("dec_radians");

			for(const auto & gal : gals.rows())
			{
				const flt_type & z = gal.at(z_col);
				if((z<source_z_min)||(z>source_z_max)) continue;
				const flt_type & mag = gal.at(mag_col);
				if((mag<IceBRG::mag_m_min)||(mag>IceBRG::mag_m_max)) continue;

				auto pos = std::make_tuple(gal.at(ra_col)*rad,gal.at(dec_col)*rad,z,mag);

				if(lensing_style)
				{
					mock_source_positions[0].push_back(std::move(pos));
				}
				else
				{
					if(lens_z_limits.outside_limits(z)) continue;
					mock_source_positions[lens_z_limits.get_bin_index(z)].push_back(std::move(pos));
				}
			}
		}

		// Set up each bin with the pointer to the proper vectors, then calculate
		for(int z_i = 0; z_i < lens_z_limits.num_bins(); ++z_i)
		{

			#ifdef USE_FIELD_WEIGHTING
			const auto & z_weights = lens_weight_table.at_label(field_name_root).raw();
			flt_type field_weight = z_weights(lens_weight_z_limits.get_bin_index((lens_z_limits.lower_limit(z_i)+lens_z_limits.upper_limit(z_i))/2));
			#else
			constexpr flt_type field_weight = 1.;
			#endif

			for(int m_i = 0; m_i < lens_m_limits.num_bins(); ++m_i)
			{
				for(int mag_i = 0; mag_i < lens_mag_limits.num_bins(); ++mag_i)
				{
					auto & bin = corr_func_bins.at(z_i).at(m_i).at(mag_i);

					// Check we have a nonzero lens count in this bin
					if(bin.lens_positions->empty()) continue;
					if(lensing_style)
					{
						bin.source_positions = &(source_positions[0]);
						bin.mock_source_positions = &(mock_source_positions[0]);
					}
					else
					{
						bin.source_positions = &(source_positions[z_i]);
						bin.mock_source_positions = &(mock_source_positions[z_i]);
					}
					bin.mock_lens_positions = &(mock_lens_positions[z_i]);

					flt_type z_mid = (lens_z_limits.lower_limit(z_i) + lens_z_limits.upper_limit(z_i))/2;

					#ifdef LIMIT_TO_MONOPOLE
					bin.add_monopole(R_limits,z_mid,field_weight);
					#else
					bin.add_all(R_limits,z_mid,field_weight);
					#endif
				}
			}
		}

		#ifdef _OPENMP
		#pragma omp critical(CFHTLenS_corr_func_combine_fields)
		#endif
		{
			try
			{
				// Loop through each bin and add it to the appropriate sum bin

				for(int z_i = 0; z_i < lens_z_limits.num_bins(); ++z_i)
				{
					for(int m_i = 0; m_i < lens_m_limits.num_bins(); ++m_i)
					{
						for(int mag_i = 0; mag_i < lens_mag_limits.num_bins(); ++mag_i)
						{
							corr_func_bin_sums[z_i][m_i][mag_i] += corr_func_bins[z_i][m_i][mag_i];
						}
					}
				}

				std::cout << "Field " << field_name_root << " (#" <<
						++num_processed << "/" << num_fields << ") complete!\n";
			}
			catch (const std::exception &e)
			{
				std::cerr << "Error combining data from field " << field_name_root << " (#" <<
						++num_processed << "/" << num_fields << ")!\n"
						<< e.what();
			}
		}

	}


	// Set up the output table
	IceBRG::labeled_array<flt_type> output_table;
	#ifdef LIMIT_TO_MONOPOLE
	std::vector<std::string> labels = {"R_bin_mid_kpc", "z_bin_mid", "m_bin_mid_Msun", "mag_bin_mid", "xi_mp"};
	#else
	std::vector<std::string> labels = {"R_bin_mid_kpc", "z_bin_mid", "m_bin_mid_Msun", "mag_bin_mid",
		"xi_mp", "xi_dp_1", "xi_dp_2", "xi_qp_1", "xi_qp_2", "xi_op_1", "xi_op_2"};
	#endif

	output_table.set_labels(std::move(labels));

	// Insert each bin in turn


	for(int z_i = 0; z_i < lens_z_limits.num_bins(); ++z_i)
	{
		flt_type z_mid = lens_z_limits.bin_mid(z_i);

		for(int m_i = 0; m_i < lens_m_limits.num_bins(); ++m_i)
		{
			mass_type m_mid = lens_m_limits.bin_mid(m_i);

			for(int mag_i = 0; mag_i < lens_mag_limits.num_bins(); ++mag_i)
			{
				flt_type mag_mid = lens_mag_limits.bin_mid(mag_i);

				// Normalize the bin sum by the total weights
				corr_func_bin_sums[z_i][m_i][mag_i].normalize();

				for(int R_i = 0; R_i < R_limits.num_bins(); ++R_i)
				{
					distance_type R_mid = R_limits.bin_mid(R_i);

					#ifdef LIMIT_TO_MONOPOLE
					std::vector<flt_type> new_row = {value_of(R_mid), z_mid, value_of(m_mid), mag_mid,
						corr_func_bin_sums[z_i][m_i][mag_i].monopole_corr_func_sum[R_i]};
					#else
					std::vector<flt_type> new_row = {R_mid, z_mid, m_mid, mag_mid,
						corr_func_bin_sums[z_i][m_i][mag_i].monopole_corr_func_sum[R_i],
						corr_func_bin_sums[z_i][m_i][mag_i].dipole_1_corr_func_sum[R_i],
						corr_func_bin_sums[z_i][m_i][mag_i].dipole_2_corr_func_sum[R_i],
						corr_func_bin_sums[z_i][m_i][mag_i].quadrupole_1_corr_func_sum[R_i],
						corr_func_bin_sums[z_i][m_i][mag_i].quadrupole_2_corr_func_sum[R_i],
						corr_func_bin_sums[z_i][m_i][mag_i].octopole_1_corr_func_sum[R_i],
						corr_func_bin_sums[z_i][m_i][mag_i].octopole_2_corr_func_sum[R_i]};
					#endif

					output_table.insert_row(std::move(new_row));
				}
			}
		}
	}

	IceBRG::unitconv_map u_map;

	u_map["R_bin_mid_kpc"] = IceBRG::unitconv::kpctom;
	u_map["m_bin_mid_Msun"] = IceBRG::unitconv::Msuntokg;

	output_table.apply_unitconvs(u_map);

	output_table.save(output_name,true);

	std::cout << "Done!\n";

	return 0;
}
