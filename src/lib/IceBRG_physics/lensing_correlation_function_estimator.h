/**********************************************************************\
 @file lensing_correlation_function_estimator.h
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

// body file: correlation_function_estimator.cpp
#ifndef _BRG_PHYSICS_LENSING_CORRELATION_FUNCTION_ESTIMATOR_H_INCLUDED_
#define _BRG_PHYSICS_LENSING_CORRELATION_FUNCTION_ESTIMATOR_H_INCLUDED_

#include <functional>
#include <limits>
#include <tuple>
#include <vector>

#include "IceBRG_main/common.h"

#include "IceBRG_main/Eigen.hpp"

#include "IceBRG_main/vector/limit_vector.hpp"

namespace IceBRG {

/**
 *
 */
class lensing_correlation_function_estimator {
private:

	typedef std::tuple<angle_type,angle_type,flt_type,flt_type> position;
	typedef std::vector<position> position_list;

	position_list _D1_pos_list_, _D2_pos_list_, _R1_pos_list_, _R2_pos_list_;

	IceBRG::limit_vector<angle_type> _r_bin_limits_;

	flt_type _z_buffer_;

	mutable Eigen::ArrayXd _unweighted_cached_value_;
	mutable Eigen::ArrayXd _unweighted_cached_error_;

	bool _set_up() const;

public:

	// Constructors
#if(1)

	/// Default constructor. Will not be set up for calculation
	lensing_correlation_function_estimator()
	: _z_buffer_(0)
	{
	}

	/// Construct with only bin limits. Will not set up for calculation
	template<typename T>
	lensing_correlation_function_estimator(T && D_bin_limits, const flt_type & z_buffer=0)
			: _r_bin_limits_(std::forward<T>(D_bin_limits)),
			  _z_buffer_(z_buffer)
	{
	}

	/// Construct with only position lists
	template<typename TR1, typename TR2, typename TM1, typename TM2>
	lensing_correlation_function_estimator(TR1 && D1_pos_list, TR2 && D2_pos_list,
			TM1 && R1_pos_list, TM2 && R2_pos_list, const flt_type & z_buffer=0)
			: _D1_pos_list_(std::forward<TR1>(D1_pos_list)),
			  _D2_pos_list_(std::forward<TR2>(D2_pos_list)),
			  _R1_pos_list_(std::forward<TM1>(R1_pos_list)),
			  _R2_pos_list_(std::forward<TM2>(R2_pos_list)),
			  _z_buffer_(z_buffer)
	{
	}

	/// Construct with bin limits and position lists
	template<typename T, typename TR1, typename TR2, typename TM1, typename TM2>
	lensing_correlation_function_estimator(T && D_bin_limits,
			TR1 && D1_pos_list, TR2 && D2_pos_list,
			TM1 && R1_pos_list, TM2 && R2_pos_list, const flt_type & z_buffer=0)
			: _D1_pos_list_(std::forward<TR1>(D1_pos_list)),
			  _D2_pos_list_(std::forward<TR2>(D2_pos_list)),
			  _R1_pos_list_(std::forward<TM1>(R1_pos_list)),
			  _R2_pos_list_(std::forward<TM2>(R2_pos_list)),
			  _r_bin_limits_(std::forward<T>(D_bin_limits)),
			  _z_buffer_(z_buffer)
	{
	}

#endif

	// Virtual destructor
	virtual ~lensing_correlation_function_estimator()
	{
	}

	// Methods to set position lists
	// TODO fill in

	// Calculation methods
#if(1)
	/// Standard calculation function, using Hamilton estimator
	Eigen::ArrayXd calculate() const;

	/// Get bin weights for standard calculation function
	Eigen::ArrayXd weights() const;

	/// Get errors for standard calculation function
	Eigen::ArrayXd errors() const;

	/// Weighted calculation function, using a Hamilton-like estimator. This assumes the weight
	/// function passed here has an expected value of zero.
	Eigen::ArrayXd calculate_weighted(const std::function<flt_type(angle_type)> &
			weight_function = [] (const angle_type & theta) {return 1.;}) const;

	/// Calculation function for dipole correlation function with a given offset. The offset should
	/// vary from [0,1) or [-0.5,0.5) for unique results.
	Eigen::ArrayXd calculate_dipole(const flt_type & offset=0) const;

	/// Calculation function for quadrupole correlation function with a given offset. The offset should
	/// vary from [0,1) or [-0.5,0.5) for unique results.
	Eigen::ArrayXd calculate_quadrupole(const flt_type & offset=0) const;

	/// Calculation function for octopole correlation function with a given offset. The offset should
	/// vary from [0,1) or [-0.5,0.5) for unique results.
	Eigen::ArrayXd calculate_octopole(const flt_type & offset=0) const;
#endif
};

} // end namespace IceBRG

#endif // _BRG_PHYSICS_LENSING_CORRELATION_FUNCTION_ESTIMATOR_H_INCLUDED_
