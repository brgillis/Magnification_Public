/**********************************************************************\
 @file main_source_counting.cpp
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
#include <string>
#include <vector>

#include <boost/lexical_cast.hpp>
#include <Eigen/Core>

#include "IceBRG_main/container/labeled_array.hpp"
#include "IceBRG_main/container/table_typedefs.hpp"

#include "IceBRG_main/external/sgsmooth.h"

#include "IceBRG_main/file_access/ascii_table_map.hpp"
#include "IceBRG_main/file_access/open_file.hpp"

#include "IceBRG_main/join_path.hpp"

#include "IceBRG_main/math/misc_math.hpp"
#include "IceBRG_main/math/calculus/integrate.hpp"
#include "IceBRG_main/math/solvers/solvers.hpp"

#include "IceBRG_main/vector/elementwise_functions.hpp"
#include "IceBRG_main/vector/limit_vector.hpp"
#include "IceBRG_main/vector/manipulations.hpp"

#include "IceBRG_lensing/magnification/mag_global_values.h"
#include "IceBRG_lensing/pair_binner.h"
#include "IceBRG_lensing/pair_bins_summary.h"
#include "IceBRG_lensing/source_galaxy.h"

#include "IceBRG_physics/astro.h"
#include "IceBRG_physics/sky_obj/galaxy.h"

#include "get_data_directory.hpp"
#include "magic_values.hpp"

// Magic values

#undef USE_MOCK_SOURCES

#undef USE_FIELD_WEIGHTING

#ifdef USE_MOCK_SOURCES
const std::string source_base = "_small_mock_source.dat";
#else
const std::string source_base = "_source.dat";
#endif

#ifdef USE_FIELD_WEIGHTING
const std::string count_column_to_use = "weighted_count";
#else
const std::string count_column_to_use = "count";
#endif

using namespace IceBRG;

constexpr double sg_mag_window = 0.4;
constexpr short unsigned sg_window = sg_mag_window/mag_m_step;
constexpr short unsigned sg_deg = 3;

int main( const int argc, const char *argv[] )
{

	// Get the desired location of the data directory

	std::ifstream fi;

	std::string data_directory = get_data_directory(argc,argv,fi);

	// Set up the locations of relevant files and directories
	const std::string field_directory = join_path(data_directory,field_subdirectory);
	const std::string lens_weight_file = join_path(data_directory,lens_weight_filename);
	const std::string mag_hist_table_base = join_path(data_directory,mag_hist_table_name_base);
	const std::string g_mag_hist_table_base = join_path(data_directory,g_mag_hist_table_name_base);
	const std::string field_size_file = join_path(data_directory,field_size_filename);

	// Open and read in the fields list

	// Set up the redshift bins
	limit_vector<double> z_bin_limits(mag_z_min,
			mag_z_max,round_int((mag_z_max-mag_z_min)/mag_z_step));

	limit_vector<double> mag_bin_limits(mag_m_counting_min,
			mag_m_counting_max,round_int((mag_m_counting_max-mag_m_counting_min)/mag_m_step));

	size_t num_z_bins = z_bin_limits.num_bins();
	size_t num_mag_bins = mag_bin_limits.num_bins();

	typedef std::vector<unsigned long> int_hist;
	typedef std::vector<long double> double_hist;

	// Hists for exactly in bin
	std::vector<int_hist> z_bin_hists(num_z_bins,
			int_hist(num_mag_bins,0));
	std::vector<double_hist> weighted_z_bin_hists(num_z_bins,
			double_hist(num_mag_bins,0));
	int_hist z_bin_counts(num_z_bins,0);
	double_hist z_tot_weight(num_z_bins,0);

	// Hists for in this bin or greater
	std::vector<int_hist> gz_bin_hists(num_z_bins,
			int_hist(num_mag_bins,0));
	std::vector<double_hist> weighted_gz_bin_hists(num_z_bins,
			double_hist(num_mag_bins,0));
	int_hist gz_bin_counts(num_z_bins,0);

	std::vector<std::string> field_names;
	std::string field_name;

	std::vector<double> field_sizes;
	const auto field_size_table = load_table_map<double>(field_size_file);

	while(fi>>field_name)
	{
		field_names.push_back(field_name);
	}

	fi.close();

	size_t num_fields = field_names.size();
	size_t num_processed = 0;


	// Load the lens weight table
	const labeled_array<double> lens_weight_table(lens_weight_file);
	auto lens_weight_z_limits_builder = coerce<std::vector<double>>(lens_weight_table.at_label("z_bin_min"));
	lens_weight_z_limits_builder.push_back(2*lens_weight_z_limits_builder.back()-
										   lens_weight_z_limits_builder.at(lens_weight_z_limits_builder.size()-2));
	const limit_vector<double> lens_weight_z_limits(std::move(lens_weight_z_limits_builder));


	// Load each field in turn and process it
	//num_fields = 2;

	#ifdef _OPENMP
	#pragma omp parallel for schedule(dynamic)
	#endif
	for(size_t field_i=0;field_i<num_fields;++field_i)
	{
		std::string field_name_base = field_names[field_i].substr(0,6);

		try
		{
			// Get the field size
			const auto size_measurements = field_size_table.at(field_name_base);

			const double field_size = size_measurements[1];

			// Get the weights for each redshift for this field
			const auto z_weights = lens_weight_table.at_label(field_name_base).raw();

			#ifdef _OPENMP
			#pragma omp critical(append_field_size)
			#endif
			{
				field_sizes.push_back(field_size);
			}

			// Append the weights for each z bin
			for(size_t j=0; j<num_z_bins; ++j)
			{
				const double implicit_lens_z = z_bin_limits.lower_limit(j) - z_buffer;

				double weight = 1;
				if(lens_weight_z_limits.inside_limits(implicit_lens_z))
				{
					weight = z_weights(lens_weight_z_limits.get_bin_index(implicit_lens_z));
				} // else weight = 1

				#ifdef _OPENMP
				#pragma omp critical(append_field_z_weight)
				#endif
				{
					z_tot_weight[j] += weight;
				}
			}

			// Get the source file names
			std::stringstream ss("");
			ss << field_directory << field_name_base << source_base;
			std::string source_input_name = ss.str();

			// Load in sources
			//const table_map_t<double> source_map(load_table_map<double>(source_input_name));
			table_map_t<double> source_map;
			source_map = (load_table_map<double>(source_input_name));
			size_t num_sources = source_map.begin()->second.size();

			for(size_t i=0; i<num_sources; ++i)
			{
				const double & source_z = source_map.at("Z_B").at(i);
				if(z_bin_limits.outside_limits(source_z)) continue;

				size_t z_i = z_bin_limits.get_bin_index(source_z);

				const double & mag = source_map.at("MAG_r").at(i);
				if(mag_bin_limits.outside_limits(mag)) continue;
				size_t mag_i = mag_bin_limits.get_bin_index(mag);

				const double implicit_lens_z = source_z - z_buffer;

				double weight = 1;
				if(lens_weight_z_limits.inside_limits(implicit_lens_z))
				{
					weight = z_weights(lens_weight_z_limits.get_bin_index(implicit_lens_z));
				} // else weight = 1

				#ifdef _OPENMP
				#pragma omp critical(increment_count_bins)
				#endif
				{

					// Add to the specific bin
					++z_bin_hists[z_i][mag_i];
					++z_bin_counts[z_i];
					weighted_z_bin_hists[z_i][mag_i]+=weight;

					// Add to the cumulative bins
					for(size_t j=0; j<=z_i; ++j)
					{
						++gz_bin_hists[j][mag_i];
						++gz_bin_counts[j];


						// Get the weight here
						const double bin_z = z_bin_limits.lower_limit(j);
						const double implicit_lens_z = bin_z - z_buffer;

						double weight = 0;
						if(lens_weight_z_limits.inside_limits(implicit_lens_z))
						{
							weight = z_weights(lens_weight_z_limits.get_bin_index(implicit_lens_z));
						} // else weight = 0

						weighted_gz_bin_hists[j][mag_i]+=weight;
					}
				}
			}

		}
		catch (const std::runtime_error &e)
		{
			std::cerr << "Error processing field " << field_name_base << " (#" <<
					++num_processed << "/" << num_fields << ")!\n"
					<< e.what();
			#ifdef _OPENMP
			continue;
			#else
			throw;
			#endif

		}
		#ifdef _OPENMP
		#pragma omp critical(report_field_complete)
		#endif
		{
			std::cout << "Field " << field_name_base << " (#" <<
				++num_processed << "/" << num_fields << ") complete.\n";
		}

	}

	// Get the total survey size
	const double survey_size = sum(field_sizes);

	// Set up and print histogram tables

	// Specific redshift tables

	auto z_hist_it = z_bin_hists.begin();
	auto z_w_hist_it = weighted_z_bin_hists.begin();
	auto z_count_it = z_bin_counts.begin();
	auto z_tot_weight_it = z_tot_weight.begin();
	for(auto z_it=z_bin_limits.begin();z_hist_it!=z_bin_hists.end(); ++z_it, ++z_hist_it, ++z_w_hist_it, ++z_count_it, ++z_tot_weight_it)
	{
		// Get the name for the table we'll output to
		std::string z_label = boost::lexical_cast<std::string>(round_int(1000 * *z_it));
		std::string mag_hist_file_name = mag_hist_table_base + z_label + mag_hist_table_tail;

		// Set up the data table, and make sure it has the needed columns
		table_map_t<double> data;
		data["mag_bin_lower"];
		data["count"];
		data["weighted_count"];

		const double count_factor = num_fields / *z_tot_weight_it;

		// Add each bin to the table
		auto h_it = z_hist_it->begin();
		auto w_h_it = z_w_hist_it->begin();
		auto mag_it = mag_bin_limits.begin();
		for(; h_it!=z_hist_it->end(); ++h_it, ++w_h_it, ++mag_it)
		{
			data["mag_bin_lower"].push_back(*mag_it);
			data["count"].push_back(*h_it / survey_size);
			data["weighted_count"].push_back(*w_h_it * count_factor / survey_size);
		}

		double mag_step = data["mag_bin_lower"].at(1)-data["mag_bin_lower"].at(0);

		// Replace any zeros with 0.1 so we can take the log safely
		auto rep_func = [] (const double & v) {if(v>0) return v; else return 0.1;};

		// Now calculate the smoothed count
		auto log_count = divide(log(apply(rep_func,data[count_column_to_use])),
				std::log(10.));
		auto smoothed_log = sg_smooth(log_count,sg_window,sg_deg);
		data["smoothed_count"] = pow(10.,smoothed_log);

		// Check for a systematic over or underestimation from smoothing
		double smooth_correction_factor = sum(data["count"])/
				sum(data["smoothed_count"]);

		data["smoothed_count"] = multiply(data["smoothed_count"],smooth_correction_factor);

		data["smoothed_alpha"] = add(multiply(
				sg_derivative(smoothed_log,sg_window,sg_deg),
				2.5/mag_step),0);

		print_table_map(mag_hist_file_name,data);

	}

	// Cumulative redshift tables

	z_hist_it = gz_bin_hists.begin();
	z_w_hist_it = weighted_gz_bin_hists.begin();
	z_count_it = gz_bin_counts.begin();
	z_tot_weight_it = z_tot_weight.begin();
	for(auto z_it=z_bin_limits.begin();z_hist_it!=gz_bin_hists.end(); ++z_it, ++z_hist_it, ++z_w_hist_it, ++z_count_it, ++z_tot_weight_it)
	{
		// Get the name for the table we'll output to
		std::string z_label = boost::lexical_cast<std::string>(round_int(1000 * *z_it));
		std::string mag_hist_file_name = g_mag_hist_table_base + z_label + mag_hist_table_tail;

		// Set up the data table, and make sure it has the needed columns
		table_map_t<double> data;
		data["mag_bin_lower"];
		data["count"];
		data["weighted_count"];

		const double count_factor = num_fields / *z_tot_weight_it;

		// Add each bin to the table
		auto h_it = z_hist_it->begin();
		auto w_h_it = z_w_hist_it->begin();
		auto mag_it = mag_bin_limits.begin();
		for(; h_it!=z_hist_it->end(); ++h_it, ++w_h_it, ++mag_it)
		{
			data["mag_bin_lower"].push_back(*mag_it);
			data["count"].push_back(*h_it / survey_size);
			data["weighted_count"].push_back(*w_h_it * count_factor / survey_size);
		}

		double mag_step = data["mag_bin_lower"].at(1)-data["mag_bin_lower"].at(0);

		// Replace any zeros with 0.1 so we can take the log safely
		auto rep_func = [] (const double & v) {if(v>0) return v; else return 0.1;};

		// Now calculate the smoothed count
		auto log_count = divide(log(apply(rep_func,data[count_column_to_use])),
				std::log(10.));
		auto smoothed_log = sg_smooth(log_count,sg_window,sg_deg);

		data["smoothed_count"] = pow(10.,smoothed_log);

		Eigen::Map<Eigen::ArrayXd> smoothed_count(data["smoothed_count"].data(),data["smoothed_count"].size());

		// Check for a systematic over or underestimation from smoothing and integration
		auto smooth_count_func = [&] (const double & mag = true)
		{
			return mag_bin_limits.interpolate_bins(mag,smoothed_count);
		};
		double integrated_count = integrate_Romberg(smooth_count_func,mag_m_counting_min,
				mag_m_counting_max,0.000001);
		double smooth_correction_factor = sum(data[count_column_to_use])*mag_m_step/
				integrated_count;

		smoothed_count *= smooth_correction_factor;

		data["smoothed_alpha"] = add(multiply(
				sg_derivative(smoothed_log,sg_window,sg_deg),
				2.5/mag_step),0);

//		auto unsmoothed_am1_count_func = [&] (const double & mag = true)
//		{
//			return data["count"][mag_bin_limits.get_bin_index(mag)] *
//				(mag_bin_limits.interpolate_bins(mag,data["smoothed_alpha"])-1);
//		};
//		double summed_am1_count = integrate_Romberg(&unsmoothed_am1_count_func,mag_m_min,
//				mag_m_max,0.000001);
//
//		auto diff_minimizer_func = [&] (const double & mag_shift = true)
//		{
//
//			auto am1s_count_func = [&] (const double & mag = true)
//			{
//				return mag_bin_limits.interpolate_bins(mag-mag_shift,data["count"]) *
//					square(mag_bin_limits.interpolate_bins(mag,data["smoothed_alpha"])-1);
//			};
//			double integrated_am1s_count = integrate_Romberg(&am1s_count_func,mag_m_min,
//					mag_m_max,0.000001);
//
//			auto am1am2_count_func = [&] (const double & mag = true)
//			{
//				return mag_bin_limits.interpolate_bins(mag-mag_shift,data["count"]) *
//					(mag_bin_limits.interpolate_bins(mag,data["smoothed_alpha"])-1) *
//					(mag_bin_limits.interpolate_bins(mag,data["smoothed_alpha"])-2);
//			};
//			double integrated_am1am2_count = integrate_Romberg(&am1am2_count_func,mag_m_min,
//					mag_m_max,0.000001);
//
//			double mu_test = (summed_am1_count+integrated_am1am2_count)/integrated_am1s_count;
//
//			return mu_test-1;
//
//		};
//
//		double mag_shift = solve_grid(&diff_minimizer_func,-10*mag_m_step,10*mag_m_step,0.,0.);
//
//		std::cout << "z = " << *z_it << ".\tTest mu diff: " << diff_minimizer_func(0) << ".\tBest shift: " << mag_shift << ",\twhich gives diff: "
//			<< diff_minimizer_func(mag_shift) << ".\n";
//
//		data["shifted_mag_bin_lower"] = add(data["mag_bin_lower"],mag_shift);

		print_table_map(mag_hist_file_name,data);

	}

	std::cout << "Done!\n";
	return 0;
}
