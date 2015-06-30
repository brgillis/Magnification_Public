/**********************************************************************\
 @file common.h
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


#ifndef _LMF_COMMON_H_INCLUDED_
#define _LMF_COMMON_H_INCLUDED_

constexpr const char * R_min_label = "R_min";
constexpr const char * R_max_label = "R_max";
constexpr const char * m_min_label = "m_min";
constexpr const char * m_max_label = "m_max";
constexpr const char * z_min_label = "z_min";
constexpr const char * z_max_label = "z_max";
constexpr const char * mag_min_label = "z_min";
constexpr const char * mag_max_label = "z_max";

constexpr const char * shear_R_mean_label = "shear_R_mean";
constexpr const char * shear_lens_m_mean_label = "shear_lens_m_mean";
constexpr const char * shear_lens_z_mean_label = "shear_lens_z_mean";
constexpr const char * shear_lens_mag_mean_label = "shear_lens_mag_mean";

constexpr const char * dS_t_mean_label = "dS_t_mean";
constexpr const char * dS_t_std_label = "dS_t_stddev";
constexpr const char * dS_t_stderr_label = "dS_t_stderr";
constexpr const char * dS_x_mean_label = "dS_x_mean";
constexpr const char * dS_x_std_label = "dS_x_stddev";
constexpr const char * dS_x_stderr_label = "dS_x_stderr";

constexpr const char * model_dS_t_label = "model_dS_t";

constexpr const char * shear_Sigma_crit_label = "shear_Sigma_crit";

constexpr const char * magf_R_mean_label = "magf_R_mean";
constexpr const char * magf_lens_m_mean_label = "magf_lens_m_mean";
constexpr const char * magf_lens_z_mean_label = "magf_lens_z_mean";
constexpr const char * magf_lens_mag_mean_label = "magf_lens_mag_mean";

constexpr const char * magf_area_label = "magf_area";

constexpr const char * Sigma_mean_label = "Sigma";
constexpr const char * Sigma_std_label = "Sigma_stddev";
constexpr const char * Sigma_stderr_label = "Sigma_stderr";

constexpr const char * model_Sigma_label = "model_Sigma";

constexpr const char * magf_Sigma_crit_label = "magf_Sigma_crit";
constexpr const char * Sigma_crit_label = magf_Sigma_crit_label;

constexpr const char * best_fit_shear_model_dS_t_label = "bf_shear_model_dS_t";
constexpr const char * best_fit_shear_model_Sigma_label = "bf_shear_model_Sigma";
constexpr const char * best_fit_magf_model_dS_t_label = "bf_magf_model_dS_t";
constexpr const char * best_fit_magf_model_Sigma_label = "bf_magf_model_Sigma";
constexpr const char * best_fit_magf_Sigma_offset_label = "bf_magf_Sigma_offset";
constexpr const char * best_fit_overall_model_dS_t_label = "bf_overall_model_dS_t";
constexpr const char * best_fit_overall_model_Sigma_label = "bf_overall_model_Sigma";
constexpr const char * best_fit_overall_Sigma_offset_label = "bf_overall_Sigma_offset";




#endif // _LMF_COMMON_H_INCLUDED_
