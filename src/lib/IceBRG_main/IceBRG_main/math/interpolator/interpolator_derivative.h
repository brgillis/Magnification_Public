/**********************************************************************\
  @file interpolator_derivative.h

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

// body file: brg/math/interpolator/interpolator_derivative.cpp

#ifndef _INTERPOLATOR_DERIVATIVE_H_INCLUDED_
#define _INTERPOLATOR_DERIVATIVE_H_INCLUDED_

#include <vector>

#include "IceBRG_main/common.h"

#include "IceBRG_main/math/interpolator/interpolator.h"
#include "IceBRG_main/utility.hpp"

namespace IceBRG {

class interpolator_derivative
{
	/************************************************************
	 interpolator_derivative
	 -----------------

	 This class operates like an interpolator with added
	 features. The same functions can be used as with the basic
	 interpolator class, but this class also has the ability to point
	 to a different interpolator, which this is intended to be
	 the derivative of. Then, "unknown" domain points can be
	 passed to this class, and it will calculate the derivative of
	 the other interpolator at those points to help fill in gaps.

	 The points passed to this can all be "known" (domain and range
	 passed), all "unknown" (only domain passed, range calculated),
	 or a mix of the two.

	 Unknown points are calculated using a smoothing kernel to help
	 handle noise in the pointed-to interpolator. This must be adjusted
	 by hand based on how noisy the interpolator's points are for optimal
	 results. The noisier it is, the wider the kernel should be.
	 Use the set_sample_scale(...) and set_sample_max_width(...)
	 functions to adjust the kernel size (the scale is the sigma of
	 a Gaussian, the max_width is the limits of integration). Both
	 take in values as representing a fraction of t_max - t_min.

	 \************************************************************/
private:
	IceBRG::interpolator *_interpolator_ptr_;
	mutable IceBRG::interpolator _known_interpolator_, _estimated_interpolator_;
	bool _interpolator_ptr_set_up_;
	mutable bool _calculated_;

	std::vector< flt_type > _unknown_t_list_;

	static flt_type _default_sample_scale_, _default_sample_max_width_,
			_default_sample_precision_;
	flt_type _sample_scale_, _sample_max_width_, _sample_precision_;
	mutable flt_type _t_min_, _t_max_;

	IceBRG::interpolator::allowed_interpolation_type _interpolation_type_;

public:

	// Swap functions
	void swap(interpolator_derivative &other);
	friend void swap(interpolator_derivative &same, interpolator_derivative &other) {same.swap(other);}

	// Constructors
	interpolator_derivative();
	interpolator_derivative( const interpolator_derivative &other);
	interpolator_derivative( IceBRG::interpolator *init_interpolator_ptr );

	// Destructors
	virtual ~interpolator_derivative()
	{
	}

	// Operator=
	interpolator_derivative & operator=(interpolator_derivative other);

	// Set functions
	void set_spline_ptr( IceBRG::interpolator *new_interpolator_ptr );
	void clear_spline_ptr();
	void set_default_sample_scale( flt_type new_default_sample_scale );
	void set_default_sample_max_width(
			flt_type new_default_sample_max_width );
	void set_sample_scale( flt_type new_sample_scale );
	void set_sample_max_width( flt_type new_sample_max_width );
	void reset_sample_scale(); // Sets it to default
	void reset_sample_max_width(); // Sets it to default
	void set_interpolation_type(
			IceBRG::interpolator::allowed_interpolation_type new_interpolation_type);
	void reset_interpolation_type();

	// Functions for adding/clearing points
	void add_point( const flt_type & t, const flt_type & x );
	void add_unknown_point( const flt_type & t );

	void clear_known_points();
	void clear_unknown_points();
	void clear_points(); // Clears all points

	// Full clear function
	void clear();

	// Get functions
	flt_type operator()( flt_type xval = false ) const;
	ssize_t size() const
	{
		return ssize(_known_interpolator_) + ssize(_unknown_t_list_);
	}
};
// class interpolator_derivative

} // end namespace IceBRG

#endif /* _INTERPOLATOR_DERIVATIVE_H_INCLUDED_ */
