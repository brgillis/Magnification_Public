/**********************************************************************\
 @file main_gg_lensing.cpp
 ------------------

 Main source file for calculating a galaxy-galaxy lensing signal in the
 CFHTLenS, including both shear and magnification.

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

#include <fstream>
#include <iostream>
#include <vector>

#include <boost/lexical_cast.hpp>

#include "IceBRG_main/container/labeled_array.hpp"
#include "IceBRG_main/file_access/ascii_table_map.hpp"
#include "IceBRG_main/file_access/open_file.hpp"
#include "IceBRG_main/units/unitconv_map.hpp"
#include "IceBRG_main/units/units.hpp"
#include "IceBRG_main/vector/elementwise_functions.hpp"
#include "IceBRG_main/vector/limit_vector.hpp"

#include "IceBRG_lensing/magnification/expected_count_cache.h"
#include "IceBRG_lensing/magnification/expected_count_derivative_cache.h"
#include "IceBRG_lensing/magnification/mag_calibration_cache.h"
#include "IceBRG_lensing/magnification/mag_signal_integral_cache.h"
#include "IceBRG_lensing/magnification/mag_weight_integral_cache.h"
#include "IceBRG_lensing/magnification/mag_global_values.h"
#include "IceBRG_lensing/pair_binner.h"
#include "IceBRG_lensing/pair_bins_summary.h"
#include "IceBRG_lensing/source_galaxy.h"

#include "IceBRG_physics/astro.h"
#include "IceBRG_physics/astro_caches.h"
#include "IceBRG_physics/sky_obj/galaxy.h"

#include "get_data_directory.hpp"
#include "magic_values.hpp"

#include "gg_lensing_config.h"
#include "pass_configs_to_binner.h"

#undef USE_CALIBRATION_LENSES

#undef USE_MOCK_LENSES
#undef USE_MOCK_SOURCES

#undef PRINT_CACHES

// Magic values

#ifdef USE_CALIBRATION_LENSES

const std::string output_table = data_directory + "gg_calibration_lensing_signal.dat";
const std::string output_data = data_directory + "gg_calibration_lensing_data.dat";

const std::string lens_root = "_calibration_lens.dat";
const std::string lens_unmasked_frac_root = "_calibration_lens_mask_frac.dat";

const std::string lens_weight_file = data_directory + "field_lens_weights.dat";

#else // #ifdef USE_CALIBRATION_LENSES

#ifdef USE_MOCK_LENSES

const std::string output_table = data_directory + "gg_mock_lensing_signal.dat";
const std::string output_data = data_directory + "gg_mock_lensing_data.dat";

const std::string lens_root = "_small_mock_lens.dat";
const std::string lens_unmasked_frac_root = "_small_mock_lens_mask_frac.dat";

const std::string lens_weight_file = data_directory + "field_lens_weights.dat";

#else // #ifdef USE_MOCK_LENSES

const std::string lens_root = "_lens.dat";
const std::string lens_unmasked_frac_root = "_lens_mask_frac.dat";

#endif // #ifdef USE_MOCK_LENSES // #else

#endif // #ifdef USE_CALIBRATION_LENSES // #else

#ifdef USE_MOCK_SOURCES
const std::string source_root = "_small_mock_source.dat";
#else
const std::string source_root = "_source.dat";
#endif

int main( const int argc, const char *argv[] )
{
	using namespace IceBRG;

	std::ifstream fi;
	const std::string data_directory = get_data_directory(argc,argv,fi);

	const std::string fields_directory = join_path(data_directory,field_subdirectory);

	const std::string output_table = join_path(data_directory,gg_lensing_signal_filename);
	const std::string output_data = join_path(data_directory,gg_lensing_data_filename);

	const std::string expected_count_cache_output_file = join_path(data_directory,
			expected_count_cache_output_filename);
	const std::string expected_count_derivative_cache_output_file = join_path(data_directory,
			expected_count_derivative_cache_output_filename);
	const std::string mag_signal_integral_cache_output_file = join_path(data_directory,
			mag_signal_integral_cache_output_filename);
	const std::string mag_weight_integral_cache_output_file = join_path(data_directory,
			mag_weight_integral_cache_output_filename);
	const std::string mag_calibration_cache_output_file = join_path(data_directory,
			mag_calibration_cache_output_filename);

	// Get the configuration file from the command-line arguments
	const gg_lensing_config config(argc,argv);

	// Set up the caches before we get to the parallel section, so they can be calculated in parallel
	#ifdef PRINT_CACHES
	IceBRG::expected_count_cache().print(expected_count_cache_output_file);
	IceBRG::expected_count_derivative_cache().print(expected_count_derivative_cache_output_file);
	IceBRG::mag_weight_integral_cache().print(mag_weight_integral_cache_output_file);
	IceBRG::mag_signal_integral_cache().print(mag_signal_integral_cache_output_file);
	#else
	IceBRG::expected_count_cache().get(0.,0.);
	IceBRG::expected_count_derivative_cache().get(0.,0.);
	IceBRG::mag_weight_integral_cache().get(0.);
	IceBRG::mag_signal_integral_cache().get(0.);
	#endif
	IceBRG::add_cache().get(0.,0.);

	#ifdef USE_CALIBRATION_LENSES
	// Load the lens weight table
	const IceBRG::labeled_array<double> lens_weight_table(lens_weight_file);
	auto lens_weight_z_limits_builder = IceBRG::coerce<std::vector<double>>(lens_weight_table.at_label("z_bin_min"));
	lens_weight_z_limits_builder.push_back(2*lens_weight_z_limits_builder.back()-
										   lens_weight_z_limits_builder.at(lens_weight_z_limits_builder.size()-2));
	const IceBRG::limit_vector<double> lens_weight_z_limits(std::move(lens_weight_z_limits_builder));
	#else
	#ifdef USE_MOCK_LENSES
	// Load the lens weight table
	const IceBRG::labeled_array<double> lens_weight_table(lens_weight_file);
	auto lens_weight_z_limits_builder = IceBRG::coerce<std::vector<double>>(lens_weight_table.at_label("z_bin_min"));
	lens_weight_z_limits_builder.push_back(2*lens_weight_z_limits_builder.back()-
										   lens_weight_z_limits_builder.at(lens_weight_z_limits_builder.size()-2));
	const IceBRG::limit_vector<double> lens_weight_z_limits(std::move(lens_weight_z_limits_builder));
	#endif
	// Set up the calibration cache
	//IceBRG::mag_calibration_cache().print(mag_calibration_cache_output_file);
	#endif // #ifndef USE_CALIBRATION_LENSES

	constexpr size_t batch_size = 1000000; // Max number of lenses that can be added to the binner before forcing a flush

	// Set up the bins summary
	IceBRG::pair_bins_summary bins_summary(pass_configs_to_binner(config));

	// Check if we're using preloaded or saved data
	if(config.use_precalculated_data)
	{
		bins_summary.load(config.precalculated_data_filename);
	}
	else // if(config.use_precalculated_data)
	{

		// Open and read in the fields list
		std::vector<std::string> field_names;
		std::string field_name;

		while(fi>>field_name)
		{
			field_names.push_back(field_name);
		}

		fi.close();

		// Load each field in turn and process it

		size_t num_fields = field_names.size();
		size_t num_processed = 0;

		//num_fields = 1;

		#ifdef _OPENMP
		#pragma omp parallel for schedule(dynamic)
		#endif
		for(size_t field_i=0;field_i<num_fields;++field_i)
		{
			IceBRG::pair_bins_summary field_bins_summary(pass_configs_to_binner(config));
			IceBRG::pair_binner lens_binner(pass_configs_to_binner(config));
			std::string field_name_root = field_names[field_i].substr(0,6);

			#ifdef USE_CALIBRATION_LENSES
			// Get the weights for each redshift for this field
			const auto z_weights = lens_weight_table.at_label(field_name_root).raw();
			#else
			#ifdef USE_MOCK_LENSES
			const auto z_weights = lens_weight_table.at_label(field_name_root).raw();
			#endif // #ifdef USE_MOCK_LENSES
			#endif // #ifdef USE_CALIBRATION_LENSES

#ifdef _OPENMP
			try
#endif
			{
				// Get the lens and source file names
				std::stringstream ss("");
				ss << fields_directory << field_name_root << lens_root;
				std::string lens_input_name = ss.str();

				ss.str("");
				ss << fields_directory << field_name_root << lens_unmasked_frac_root;
				std::string lens_unmasked_name = ss.str();

				ss.str("");
				ss << fields_directory << field_name_root << source_root;
				std::string source_input_name = ss.str();

				// Set up vectors
				std::vector<IceBRG::galaxy> lens_galaxies;
				std::vector<IceBRG::source_galaxy> source_galaxies;

				// Load in lenses
				const IceBRG::table_map_t<double> lens_map(IceBRG::load_table_map<double>(lens_input_name));
				size_t num_lenses = lens_map.begin()->second.size();

				for(size_t i=0; i<num_lenses; ++i)
				{
					IceBRG::galaxy lens;
					double z = lens_map.at("Z_B").at(i);
					lens.set_z(z);
					lens.set_ra(lens_map.at("ra_radians").at(i) * rad);
					lens.set_dec(lens_map.at("dec_radians").at(i) * rad);
					lens.stellar_mass = lens_map.at("Mstel_kg").at(i) * kg;
					lens.imag = lens_map.at("MAG_i").at(i);
					lens.set_index(lens_map.at("SeqNr").at(i));

					// Check if the lens fits somewhere within the binner's limits
					if(!lens_binner.binnable(lens)) continue;

					#ifdef USE_CALIBRATION_LENSES
					// Get the weight for this lens
					double weight = z_weights(lens_weight_z_limits.get_bin_index(z-z_buffer));
					lens.set_weight(weight);
					#else
					#ifdef USE_MOCK_LENSES
					// Get the weight for this lens
					double weight = z_weights(lens_weight_z_limits.get_bin_index(z-z_buffer));
					//lens.set_weight(weight);
					#endif
					#endif // #ifdef USE_CALIBRATION_LENSES

					lens_galaxies.push_back(std::move(lens));
				}

				// Load the masked fraction table
				const IceBRG::table_map_t<double> lens_unmasked_frac_map(
						IceBRG::load_table_map<double>(lens_unmasked_name));
				IceBRG::limit_vector<distance_type> unmasked_frac_bin_limits;

				unmasked_frac_bin_limits.reconstruct_from_bin_mids(IceBRG::multiply(lens_unmasked_frac_map.at("bin_mid_kpc"),
																						IceBRG::unitconv::kpctom*m));

				// Load in sources
				const IceBRG::table_map_t<double> source_map(IceBRG::load_table_map<double>(source_input_name));
				size_t num_sources = source_map.begin()->second.size();

				for(size_t i=0; i<num_sources; ++i)
				{
					IceBRG::source_galaxy source(source_map.at("ra_radians").at(i)*rad,
										source_map.at("dec_radians").at(i)*rad,
										source_map.at("Z_B").at(i),
										source_map.at("e1").at(i),
										source_map.at("e2").at(i),
										0,
										source_map.at("Mstel_kg").at(i)*kg,
										source_map.at("MAG_r").at(i));

					source.set_weight(source_map.at("weight").at(i));
					source.set_index(source_map.at("SeqNr").at(i));

					// Check that this source is valid for either shear or magnification

					const auto & mag = source.mag();
					const auto & z = source.z();
					const auto & shear_weight = source.weight();

					if( (shear_weight > 0) || // If it's valid for shear
						((mag>=IceBRG::mag_m_min) && (mag<IceBRG::mag_m_max) &&
						 (z>=IceBRG::mag_z_min) && (z<IceBRG::mag_z_max)) ) // or it's valid for magnification
					{
						source_galaxies.push_back( std::move(source) ); // Add it to the list of sources
					}
				}

				// Find lens-source pairs and add them to the binner
				size_t lens_i = 0;
				for(const auto & lens : lens_galaxies)
				{

					// Get the unmasked fraction for this lens and set it
					lens_binner.set_unmasked_fractions( unmasked_frac_bin_limits,
							lens_unmasked_frac_map.at(boost::lexical_cast<std::string>(lens.index())));

					lens_binner.add_lens_id(lens.index(),lens.m(),lens.z(),lens.mag());

					const angle_type max_angle_sep = IceBRG::afd(config.R_max,lens.z());

					// And loop over all sources

					for(const auto & source : source_galaxies)
					{

						// Check that the lens is sufficiently in front of the source
						if(lens.z() > source.z() - z_buffer + std::numeric_limits<double>::epsilon()) continue;

						// Check against maximum angular separation in ra and dec simply first for speed
						auto ddec = abs(lens.dec()-source.dec());
						if(ddec>max_angle_sep) continue;
						double cosdec = cos((lens.dec()+source.dec())/2.);
						auto dra = abs(lens.ra()-source.ra())*cosdec;
						if(dra>max_angle_sep) continue;

						angle_type da = IceBRG::dist2d(dra,ddec);

						if(da <= max_angle_sep)
						{
							lens_binner.add_pair(IceBRG::lens_source_pair(&lens,&source));
						}

					}

					if((++lens_i) % batch_size == 0)
					{
						// Add this binner to the field summary and empty it
						field_bins_summary += lens_binner;
						lens_binner.empty();
					}
				}
				field_bins_summary += lens_binner;
			}
#ifdef _OPENMP
			catch (const std::exception &e)
			{

				#pragma omp critical(CFHTLenS_gg_lens_combine_fields)
				{
					std::cerr << "Error processing field " << field_name_root << " (#" <<
							++num_processed << "/" << num_fields << ")!\n"
							<< e.what() << std::endl;
				}

			}
#endif

#ifdef _OPENMP
			#pragma omp critical(CFHTLenS_gg_lens_combine_fields)
#endif
			{
				try
				{
					bins_summary += field_bins_summary;
					std::cout << "Field " << field_name_root << " (#" <<
							++num_processed << "/" << num_fields << ") complete.\n";
				}
				catch (const std::exception &e)
				{
					std::cerr << "Error combining data from field " << field_name_root << " (#" <<
							++num_processed << "/" << num_fields << ")!\n"
							<< e.what();
				}
			}

		}

		// Save the data so we can load it in again without needing to rerun
		bins_summary.save(output_data);
	} // if(config.use_precalculated_data) // else

	// Set up the units we want data to be output in
	IceBRG::unitconv_map u_map;

	u_map["R_min"] = u_map["R_max"] = u_map["shear_R_mean"] = u_map["magf_R_mean"] =
		IceBRG::unitconv::kpctom;
	u_map["m_min"] = u_map["m_max"] = u_map["shear_lens_m_mean"] = u_map["magf_lens_m_mean"] =
		IceBRG::unitconv::Msuntokg;
	u_map["dS_t_mean"] = u_map["dS_t_stddev"] = u_map["dS_t_stderr"] =
		u_map["dS_x_mean"] = u_map["dS_x_stddev"] = u_map["dS_x_stderr"] =
		u_map["model_dS_t"] = u_map["Sigma"] = u_map["Sigma_stderr"] =
		u_map["model_Sigma"] = u_map["shear_Sigma_crit"] = u_map["magf_Sigma_crit"] =
			IceBRG::unitconv::Msuntokg/IceBRG::square(IceBRG::unitconv::pctom);
	u_map["magf_area"] = IceBRG::square(IceBRG::unitconv::asectorad);

	bins_summary.print_bin_data(output_table,u_map);

	return 0;
}
