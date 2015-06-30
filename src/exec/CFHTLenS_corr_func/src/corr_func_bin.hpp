/**********************************************************************\
 @file corr_func_bin.hpp
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

#ifndef _BRG_SRC_CORR_FUNC_BIN_HPP_INCLUDED_
#define _BRG_SRC_CORR_FUNC_BIN_HPP_INCLUDED_

#include <boost/optional.hpp>

#include "IceBRG_main/vector/limit_vector.hpp"
#include "IceBRG_main/units/units.hpp"

#include "IceBRG_physics/astro.h"
#include "IceBRG_physics/correlation_function_estimator.h"
#include "IceBRG_physics/lensing_correlation_function_estimator.h"

#include "IceBRG_lensing/magnification/mag_correlation_function_estimator.h"

/**
 *
 */
struct corr_func_bin
{
	typedef std::vector<std::tuple<IceBRG::angle_type,IceBRG::angle_type,flt_type,flt_type>> pos_vec;
	boost::optional<pos_vec> lens_positions;
	const pos_vec * source_positions;
	const pos_vec * mock_lens_positions;
	const pos_vec * mock_source_positions;

	short lensing_style;
	double z_buffer;

	typedef Eigen::ArrayXd array;
	array monopole_corr_func_sum;
	array corr_func_weight_sum;
	array dipole_1_corr_func_sum;
	array dipole_2_corr_func_sum;
	array quadrupole_1_corr_func_sum;
	array quadrupole_2_corr_func_sum;
	array octopole_1_corr_func_sum;
	array octopole_2_corr_func_sum;

	void initialize(const size_t & size)
	{
		monopole_corr_func_sum = array::Zero(size);
		corr_func_weight_sum = array::Zero(size);
		dipole_1_corr_func_sum = array::Zero(size);
		dipole_2_corr_func_sum = array::Zero(size);
		quadrupole_1_corr_func_sum = array::Zero(size);
		quadrupole_2_corr_func_sum = array::Zero(size);
		octopole_1_corr_func_sum = array::Zero(size);
		octopole_2_corr_func_sum = array::Zero(size);
	}

	void initialize(const IceBRG::limit_vector<IceBRG::distance_type> & R_limits)
	{
		initialize(R_limits.num_bins());
	}

	void add_monopole(const IceBRG::limit_vector<IceBRG::distance_type> & phys_R_limits,
			const double & mean_z, const double & weight=1.)
	{
		IceBRG::limit_vector<IceBRG::angle_type> R_limits =
				phys_R_limits.get_multiplied(IceBRG::afd(mean_z));

		if(lensing_style==2)
		{
			IceBRG::mag_correlation_function_estimator estimator(R_limits,*lens_positions,
				*source_positions,*mock_lens_positions,*mock_source_positions,z_buffer);

			estimator.calculate();
			array weights = estimator.weights()*weight;

			#pragma omp critical(combine_corr_funcs)
			{
				monopole_corr_func_sum += estimator.calculate()*weights;
				corr_func_weight_sum += weights;
			}
		}
		else if(lensing_style==1)
		{
			IceBRG::lensing_correlation_function_estimator estimator(R_limits,*lens_positions,
				*source_positions,*mock_lens_positions,*mock_source_positions,z_buffer);

			estimator.calculate();
			array weights = estimator.weights()*weight;

			#pragma omp critical(combine_corr_funcs)
			{
				monopole_corr_func_sum += estimator.calculate()*weights;
				corr_func_weight_sum += weights;
			}
		}
		else
		{
			IceBRG::correlation_function_estimator estimator(R_limits,*lens_positions,
				*source_positions,*mock_lens_positions,*mock_source_positions);

			estimator.calculate();
			array weights = estimator.weights()*weight;

			#pragma omp critical(combine_corr_funcs)
			{
				monopole_corr_func_sum += estimator.calculate()*weights;
				corr_func_weight_sum += weights;
			}
		}
	}

	void add_all(IceBRG::limit_vector<IceBRG::distance_type> phys_R_limits, const double & mean_z, const double & weight=1.)
	{
		IceBRG::limit_vector<IceBRG::angle_type> R_limits =
				phys_R_limits.get_multiplied(IceBRG::afd(mean_z));

		if(lensing_style==2)
		{
			IceBRG::mag_correlation_function_estimator estimator(R_limits,*lens_positions,
				*source_positions,*mock_lens_positions,*mock_source_positions,z_buffer);

			estimator.calculate();
			array weights = estimator.weights()*weight;


			#pragma omp critical(combine_corr_funcs)
			{
				monopole_corr_func_sum += estimator.calculate()*weights;
				corr_func_weight_sum += weights;

				dipole_1_corr_func_sum = estimator.calculate_dipole(0)*weights;
				dipole_2_corr_func_sum = estimator.calculate_dipole(0.5)*weights;
				quadrupole_1_corr_func_sum = estimator.calculate_quadrupole(0)*weights;
				quadrupole_2_corr_func_sum = estimator.calculate_quadrupole(0.5)*weights;
				octopole_1_corr_func_sum = estimator.calculate_octopole(0)*weights;
				octopole_2_corr_func_sum = estimator.calculate_octopole(0.5)*weights;
			}
		}
		else if(lensing_style==1)
		{
			IceBRG::lensing_correlation_function_estimator estimator(R_limits,*lens_positions,
				*source_positions,*mock_lens_positions,*mock_source_positions,z_buffer);

			estimator.calculate();
			array weights = estimator.weights()*weight;


			#pragma omp critical(combine_corr_funcs)
			{
				monopole_corr_func_sum += estimator.calculate()*weights;
				corr_func_weight_sum += weights;

				dipole_1_corr_func_sum = estimator.calculate_dipole(0)*weights;
				dipole_2_corr_func_sum = estimator.calculate_dipole(0.5)*weights;
				quadrupole_1_corr_func_sum = estimator.calculate_quadrupole(0)*weights;
				quadrupole_2_corr_func_sum = estimator.calculate_quadrupole(0.5)*weights;
				octopole_1_corr_func_sum = estimator.calculate_octopole(0)*weights;
				octopole_2_corr_func_sum = estimator.calculate_octopole(0.5)*weights;
			}
		}
		else
		{
			IceBRG::correlation_function_estimator estimator(R_limits,*lens_positions,
				*source_positions,*mock_lens_positions,*mock_source_positions);

			estimator.calculate();
			array weights = estimator.weights()*weight;

			#pragma omp critical(combine_corr_funcs)
			{
				monopole_corr_func_sum += estimator.calculate()*weights;
				corr_func_weight_sum += weights;

				dipole_1_corr_func_sum = estimator.calculate_dipole(0)*weights;
				dipole_2_corr_func_sum = estimator.calculate_dipole(0.5)*weights;
				quadrupole_1_corr_func_sum = estimator.calculate_quadrupole(0)*weights;
				quadrupole_2_corr_func_sum = estimator.calculate_quadrupole(0.5)*weights;
				octopole_1_corr_func_sum = estimator.calculate_octopole(0)*weights;
				octopole_2_corr_func_sum = estimator.calculate_octopole(0.5)*weights;
			}
		}
	}

	void normalize()
	{
		monopole_corr_func_sum /= corr_func_weight_sum;
		dipole_1_corr_func_sum /= corr_func_weight_sum;
		dipole_2_corr_func_sum /= corr_func_weight_sum;
		quadrupole_1_corr_func_sum /= corr_func_weight_sum;
		quadrupole_2_corr_func_sum /= corr_func_weight_sum;
		octopole_1_corr_func_sum /= corr_func_weight_sum;
		octopole_2_corr_func_sum /= corr_func_weight_sum;

		corr_func_weight_sum /= corr_func_weight_sum;
	}


	corr_func_bin(const int & R_steps = 0)
	: lens_positions(boost::none),
	  source_positions(nullptr),
	  mock_lens_positions(nullptr),
	  mock_source_positions(nullptr),
	  lensing_style(true),
	  z_buffer(0),
	  monopole_corr_func_sum(array::Zero(R_steps)),
	  corr_func_weight_sum(array::Zero(R_steps)),
	  dipole_1_corr_func_sum(array::Zero(R_steps)),
	  dipole_2_corr_func_sum(array::Zero(R_steps)),
	  quadrupole_1_corr_func_sum(array::Zero(R_steps)),
	  quadrupole_2_corr_func_sum(array::Zero(R_steps)),
	  octopole_1_corr_func_sum(array::Zero(R_steps)),
	  octopole_2_corr_func_sum(array::Zero(R_steps))
	{
	}

	corr_func_bin( const int & R_steps, pos_vec && new_lens_positions, const pos_vec & new_source_positions,
				   const pos_vec & new_mock_lens_positions, const pos_vec & new_mock_source_positions,
				   const bool & new_lensing_style = true, const double & new_z_buffer = 0)
	: lens_positions(std::move(new_lens_positions)),
	  source_positions(&new_source_positions),
	  mock_lens_positions(&new_mock_lens_positions),
	  mock_source_positions(&new_mock_source_positions),
	  lensing_style(new_lensing_style),
	  z_buffer(new_z_buffer),
	  monopole_corr_func_sum(array::Zero(R_steps)),
	  corr_func_weight_sum(array::Zero(R_steps)),
	  dipole_1_corr_func_sum(array::Zero(R_steps)),
	  dipole_2_corr_func_sum(array::Zero(R_steps)),
	  quadrupole_1_corr_func_sum(array::Zero(R_steps)),
	  quadrupole_2_corr_func_sum(array::Zero(R_steps)),
	  octopole_1_corr_func_sum(array::Zero(R_steps)),
	  octopole_2_corr_func_sum(array::Zero(R_steps))
	{
	}

	corr_func_bin( const int & R_steps, const pos_vec & new_lens_positions, const pos_vec & new_source_positions,
				   const pos_vec & new_mock_lens_positions, const pos_vec & new_mock_source_positions,
				   const bool & new_lensing_style = true, const double & new_z_buffer = 0)
	: lens_positions(new_lens_positions),
	  source_positions(&new_source_positions),
	  mock_lens_positions(&new_mock_lens_positions),
	  mock_source_positions(&new_mock_source_positions),
	  lensing_style(new_lensing_style),
	  z_buffer(new_z_buffer),
	  monopole_corr_func_sum(array::Zero(R_steps)),
	  corr_func_weight_sum(array::Zero(R_steps)),
	  dipole_1_corr_func_sum(array::Zero(R_steps)),
	  dipole_2_corr_func_sum(array::Zero(R_steps)),
	  quadrupole_1_corr_func_sum(array::Zero(R_steps)),
	  quadrupole_2_corr_func_sum(array::Zero(R_steps)),
	  octopole_1_corr_func_sum(array::Zero(R_steps)),
	  octopole_2_corr_func_sum(array::Zero(R_steps))
	{
	}

	operator bool()
	{
		return lens_positions && source_positions && mock_lens_positions && mock_source_positions;
	}

	corr_func_bin & operator+=(const corr_func_bin & other)
	{
		auto new_weight = corr_func_weight_sum + other.corr_func_weight_sum;
		monopole_corr_func_sum = (monopole_corr_func_sum*corr_func_weight_sum +
			other.monopole_corr_func_sum*other.corr_func_weight_sum)/new_weight;
		dipole_1_corr_func_sum = (dipole_1_corr_func_sum*corr_func_weight_sum +
			other.dipole_1_corr_func_sum*other.corr_func_weight_sum)/new_weight;
		dipole_2_corr_func_sum = (dipole_2_corr_func_sum*corr_func_weight_sum +
			other.dipole_2_corr_func_sum*other.corr_func_weight_sum)/new_weight;
		quadrupole_1_corr_func_sum = (quadrupole_1_corr_func_sum*corr_func_weight_sum +
			other.quadrupole_1_corr_func_sum*other.corr_func_weight_sum)/new_weight;
		quadrupole_2_corr_func_sum = (quadrupole_2_corr_func_sum*corr_func_weight_sum +
			other.quadrupole_2_corr_func_sum*other.corr_func_weight_sum)/new_weight;
		octopole_1_corr_func_sum = (octopole_1_corr_func_sum*corr_func_weight_sum +
			other.octopole_1_corr_func_sum*other.corr_func_weight_sum)/new_weight;
		octopole_2_corr_func_sum = (octopole_2_corr_func_sum*corr_func_weight_sum +
			other.octopole_2_corr_func_sum*other.corr_func_weight_sum)/new_weight;

		corr_func_weight_sum = new_weight;

		return *this;
	}

	corr_func_bin operator+(corr_func_bin other)
	{
		return other+=*this;
	}
};

#endif // _BRG_SRC_CORR_FUNC_BIN_HPP_INCLUDED_
