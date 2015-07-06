/**********************************************************************\
 @file magic_values.hpp
 ------------------

 TODO <Insert file description here>

 **********************************************************************

 Copyright (C) 2015 brg

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

#ifndef MAGIC_VALUES_HPP_
#define MAGIC_VALUES_HPP_

#include "IceBRG_main/math/misc_math.hpp"
#include "IceBRG_main/units/units.hpp"
#include "IceBRG_main/units/unit_conversions.hpp"

// Directories
const std::string unfiltered_field_subdirectory = "full_tables/";
const std::string field_subdirectory = "filtered_tables/";
const std::string mask_subdirectory = "masks/";

// File names
const std::string count_fitting_results_filename = "count_fitting_results.dat";
const std::string lens_weight_filename = "field_lens_weights.dat";
const std::string field_size_filename = "masks/field_sizes.dat";
const std::string z_cal_filename = "/disk2/brg/git/CFHTLenS_cat/Data/redshift_calibration_table.dat";
const std::string gg_lensing_signal_filename = "gg_lensing_signal.dat";
const std::string gg_lensing_data_filename = "gg_lensing_serialization.dat";
const std::string expected_count_cache_output_filename = "ex_count_cache.dat";
const std::string expected_count_derivative_cache_output_filename = "alpha_cache.dat";
const std::string mag_signal_integral_cache_output_filename = "mag_sig_integral_cache.dat";
const std::string mag_weight_integral_cache_output_filename = "mag_W_integral_cache.dat";
const std::string mag_calibration_cache_output_filename = "mag_calibration_cache.dat";

// File name bases
const std::string mag_hist_table_name_base = "magnitude_hist_z";
const std::string g_mag_hist_table_name_base = "magnitude_hist_gz";

// File name tails
const std::string data_file_tail = ".dat";
const std::string lens_tail = "_lens" + data_file_tail;
const std::string source_tail = "_source" + data_file_tail;
const std::string pixel_map_tail = "_lens_good_pixels.bin";
const std::string count_table_tail = data_file_tail;
const std::string mag_hist_table_tail = data_file_tail;

// Analysis parameters
constexpr double lens_z_min = 0.2;
constexpr double lens_z_max = 1.3;
constexpr double z_bin_min = lens_z_min;
constexpr double z_bin_max = lens_z_max;
constexpr double z_buffer = 0.2;

const IceBRG::mass_type lens_m_min = 1e9*IceBRG::unitconv::Msuntokg*IceBRG::kg;
const IceBRG::mass_type lens_m_max = 1e12*IceBRG::unitconv::Msuntokg*IceBRG::kg;

// Physical values
constexpr double rad_per_px = 0.185965*IceBRG::unitconv::asectorad;
const IceBRG::square_angle_type field_size(130.98*IceBRG::square((M_PI/180 * IceBRG::radian)));



#endif // MAGIC_VALUES_HPP_
