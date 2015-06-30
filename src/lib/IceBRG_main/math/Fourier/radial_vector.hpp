/**********************************************************************\
 @file radial_vector.hpp
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

#ifndef BRG_MATH_FOURIER_RADIAL_VECTOR_HPP_
#define BRG_MATH_FOURIER_RADIAL_VECTOR_HPP_

#include <cassert>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <type_traits>
#include <utility>

#include <boost/optional.hpp>

#include "IceBRG_main/common.h"

#include "IceBRG_main/Eigen.hpp"

#include "IceBRG_main/container/is_eigen_container.hpp"
#include "IceBRG_main/error_handling.h"
#include "IceBRG_main/math/interpolator/interpolator.h"
#include "IceBRG_main/math/misc_math.hpp"

namespace IceBRG
{
namespace Fourier
{

/**
 * An array representing some physical value which is spherically-symmetric. It stores information
 * on the physical scale as well as Fourier-transform data.
 */
class radial_vector
{
private:

	error_behavior_type _error_behavior_ = error_behavior_type::THROW;

	size_type _N_; ///< The number of points in the array
	flt_type _scale_; ///< The physical size of the array in real-space (_scale_/_N_ is the scale of one step)

	mutable boost::optional<flt_array_type> _r_array_; ///< The values in a real-space representation
	mutable boost::optional<flt_array_type> _k_array_; ///< The values in a Fourier-space representation

	mutable boost::optional<fftw_wisdom_accumulator &> _wisdom_;

	void _calculate_k_array() const
	{
		assert(is_good());
		_k_array_ = spherical_Fourier_transform(*_r_array_,_wisdom_);
	}

	void _calculate_r_array() const
	{
		assert(is_good());
		_r_array_ = inverse_spherical_Fourier_transform(*_k_array_,_wisdom_);
	}

	// Basic rescaling-related methods
#if(1)

	/// Determine whether we should be using the r or k array for rescaling
	bool _use_r_array_for_rescale(const flt_type & new_scale, const int_type & new_N) const
	{
		assert(new_scale>0.);
		assert(new_N>=2);

		// Determine which of the r and k arrays has better coverage of the new space

		// First, get the min and max values covered by each array presently
		flt_type r_min_old = r_min();
		flt_type r_max_old = r_max();

		flt_type r_min_new = r_min(new_scale,new_N);
		flt_type r_max_new = r_max(new_scale,new_N);

		flt_type k_min_old = k_min();
		flt_type k_max_old = k_max();

		flt_type k_min_new = k_min(new_scale,new_N);
		flt_type k_max_new = k_max(new_scale,new_N);

		// Determine which of the old/new min and max pairs are larger or smaller, as needed
		flt_type larger_r_min = IceBRG::max(r_min_old,r_min_new);
		flt_type smaller_r_max = IceBRG::min(r_max_old,r_max_new);
		flt_type larger_k_min = IceBRG::max(k_min_old,k_min_new);
		flt_type smaller_k_max = IceBRG::min(k_max_old,k_max_new);

		// Get the coverage for r and k
		flt_type r_coverage = (smaller_r_max-larger_r_min)/(r_max_new-r_min_new);
		flt_type k_coverage = (smaller_k_max-larger_k_min)/(k_max_new-k_min_new);

		if(r_coverage>k_coverage) return true;
		if(r_coverage<k_coverage) return false;

		// Okay, they have equal coverage. Is there a way we can avoid having to do a
		// transform first?
		if(!_k_array_) return true;
		if(!_r_array_) return false;

		// Everything's equal. So go with real-space, as that's likely more relevant
		return true;
	}

	/// Private implementation of how to rescale the r array
	void _rescale_r_array( const flt_type & new_scale, const size_type & new_N )
	{
		// Generate the r array if necessary
		if(!_r_array_) _calculate_r_array();

		// Cache the values of the new and old r_mins, r_maxes and r_steps
		flt_type old_r_min = r_min();
		flt_type new_r_min = r_min(new_scale, new_N);

		flt_type old_r_step = _scale_/_N_;
		flt_type new_r_step = new_scale/new_N;

		// Get the new array
		flt_array_type new_r_array = _interpolate_new_array(old_r_min,
				old_r_step, new_N, new_r_min, new_r_step, *_r_array_);

		// Swap in the new r_array and uncache the k_array, and we're done
		std::swap(*_r_array_,new_r_array);
		_k_array_ = boost::none;
		_scale_ = new_scale;
		_N_ = new_N;

	}

	/// Private implementation of how to rescale the k array
	void _rescale_k_array( const flt_type & new_scale, const size_type & new_N )
	{
		// Generate the k array if necessary
		if(!_k_array_) _calculate_k_array();

		// Cache the values of the new and old r_mins, r_maxes and r_steps
		flt_type old_k_min = k_min();
		flt_type new_k_min = k_min(new_scale, new_N);

		flt_type old_k_step = 1./_scale_;
		flt_type new_k_step = 1./new_scale;

		// Get the new array
		flt_array_type new_k_array = _interpolate_new_array(old_k_min,
				old_k_step, new_N, new_k_min, new_k_step, *_k_array_);

		// Swap in the new k_array and uncache the r_array, and we're done
		std::swap(*_k_array_,new_k_array);
		_r_array_ = boost::none;
		_scale_ = new_scale;
		_N_ = new_N;
	}

	/// Interpolate a new, expanded or contracted array given an existing one and the dimensions of both
	flt_array_type _interpolate_new_array(const flt_type & old_x_min,
			const flt_type & old_x_step, const size_type & new_N,
			const flt_type & new_x_min, const flt_type & new_x_step, const flt_array_type & x_array)
	{
		flt_type new_x_max = new_x_min + (new_N-1) * new_x_step;
		flt_type old_x_max = old_x_min + (_N_-1) * old_x_step;

		// Set up interpolators using the current data
		interpolator linear_interpolator;
		interpolator log_interpolator;
		for (size_type i = 0; i < _N_; ++i)
		{
			flt_type value = x_array[i];
			linear_interpolator.add_point(old_x_min + i * old_x_step, value);

			// Get the value for the log interpolator
			flt_type log_value;
			if (value > 0.)
				log_value = std::log(value);
			else if (value < 0.)
				log_value = std::log(-value);
			else
				log_value = std::log(1e-99); // Special handling for zero values

			log_interpolator.add_point(old_x_min + i * old_x_step, log_value);
		}

		// Make sure the log interpolator is decreasing at large x, if necessary
		if(new_x_max>old_x_max)
		{
			if(log_interpolator(old_x_max) >
				log_interpolator(old_x_max - old_x_step))
			{
				// If this is the case, it's increasing at the end. Let's try to fix it

				// First, let's see if we can approximate the decay by using the max value of the
				// log array
				size_type max_i;
				flt_type max_value = x_array.abs().maxCoeff(&max_i);

				// Is this the final value?
				if(max_i==_N_-1)
				{
					// If so, it's badly divergent, and we can't handle it
					_divergent_data_error();
				}
				else
				{
					// It isn't, so we can use it to estimate the decay
					flt_type decay_rate = (log_interpolator(old_x_max) - max_value)
									/((_N_-1-max_i)*old_x_step);
					assert(decay_rate<0);

					// Add one more point to the log interpolator to make sure it decays at the end
					log_interpolator.add_point(new_x_max,
							log_interpolator(old_x_max) +
							decay_rate*(new_x_max-old_x_max));
				}
			}
		}

		// Create the new r array, using the linear interpolator for all points less than the old max,
		// and the log interpolator for all points greater
		flt_array_type new_x_array(new_N);
		for (size_type i = 0; i < new_N; ++i)
		{
			flt_type x = new_x_min + i * new_x_step;
			if (x <= old_x_max)
			{
				new_x_array[i] = linear_interpolator(x);
			}
			else
			{
				// For values beyond the end of the old array, we'll use the log interpolator, which
				// essentially results in constant exponential decay in most normal cases

				// Make sure we match the sign of the last value
				short_int_type sign = IceBRG::sign(new_x_array[i - 1]);
				new_x_array[i] = sign * std::exp(log_interpolator(x));
			}
		}
		return new_x_array;
	}

	/// How we handle an error due to divergent data
	void _divergent_data_error()
	{
		// We have a potential problem. What do we do about it?
		std::string error_message(
				"Cannot rescale array due to divergent data.");
		if (_error_behavior_ == error_behavior_type::THROW)
		{
			throw std::runtime_error(error_message);
		}
		else if (_error_behavior_ == error_behavior_type::WARN)
		{
			std::cerr << "WARNING: " << error_message << std::endl;
		}
		else if (_error_behavior_ == error_behavior_type::LOG)
		{
			std::cerr << "WARNING: " << error_message << std::endl;
		}
		else
		{
			// Do nothing
		}
	}

#endif // Rescaling functions

	// Methods for rescaling to match up with another radial_vector
#if(1)

	bool _this_is_better_for_r_rescaling(const radial_vector & other)
	{
		return r_max() <= other.r_max();
	}

	bool _this_is_better_for_k_rescaling(const radial_vector & other)
	{
		return k_max() <= other.k_max();
	}

#endif // Methods for rescaling to match up with another radial_vector

	/// Necessary factor to multiply by for convolution
	flt_type _convolution_factor()
	{
		return 2*IceBRG::cube(get_scale());
	}

public:

	// Constructors
#if(1)
	/// Default constructor
	radial_vector()
	: _N_(0), _scale_(1.)
	{
	}

	/// Copy constructor
	radial_vector(const radial_vector & other) = default;

	/// Move constructor
	radial_vector(radial_vector && other) = default;

	/// Copy/move from Eigen array - this loads into real-space array
	template< typename To, typename Ts,
		typename std::enable_if<std::is_same<typename std::decay<To>::type,flt_type>::value,char>::type = 0,
		typename std::enable_if<std::is_convertible<Ts,flt_type>::value,char>::type = 0>
	explicit radial_vector(To && other, Ts && scale=1.,
			boost::optional<fftw_wisdom_accumulator &> wisdom = boost::none)
	: _N_(other.size()), _scale_(std::forward<Ts>(scale)), _r_array_(std::forward<To>(other))
	{
		_wisdom_ = std::move(wisdom);
		assert(is_good());
	}

	/// Convert from Eigen-like array - this loads into real-space array
	template< typename To, typename Ts,
		typename std::enable_if<!std::is_same<typename std::decay<To>::type,flt_type>::value,char>::type = 0,
		typename std::enable_if<!std::is_convertible<To,flt_type>::value,char>::type = 0,
		typename std::enable_if<!std::is_convertible<Ts,flt_type>::value,char>::type = 0>
	explicit radial_vector(const To & other, Ts && scale=1.,
			boost::optional<fftw_wisdom_accumulator &> wisdom = boost::none)
	: _N_(other.size()), _scale_(std::forward<Ts>(scale)), _r_array_(other)
	{
		_wisdom_ = std::move(wisdom);
		assert(is_good());
	}

	/// Convert from container - this loads into real-space array
	template< typename To, typename Ts,
		typename std::enable_if<!std::is_same<typename std::decay<To>::type,flt_type>::value,char>::type = 0,
		typename std::enable_if<!std::is_convertible<To,flt_type>::value,char>::type = 0,
		typename std::enable_if<IceBRG::is_container<To>::value,char>::type = 0>
	explicit radial_vector(const To & other, Ts && scale=1.,
			boost::optional<fftw_wisdom_accumulator &> wisdom = boost::none)
	: _N_(other.size()), _scale_(std::forward<Ts>(scale)),
	  _r_array_(IceBRG::coerce<flt_array_type>(other))
	{
		_wisdom_ = std::move(wisdom);
		assert(is_good());
	}

	/// Set up with scale only
	template< typename Ts,
		typename std::enable_if<std::is_convertible<Ts,flt_type>::value,char>::type = 0,
		typename std::enable_if<!IceBRG::is_container<Ts>::value,char>::type = 0>
	explicit radial_vector(Ts && scale=1.,
			boost::optional<fftw_wisdom_accumulator &> wisdom = boost::none)
	: _N_(0), _scale_(std::forward<Ts>(scale))
	{
		_wisdom_ = std::move(wisdom);
		assert(_scale_>0.);
	}

#endif // Constructors

	/// Destructor
	virtual ~radial_vector()
	{
	}

	/// Full check that this object is in a valid state
	bool is_good() const
	{
		if(_N_<2) return false;
		if(_scale_<=0) return false;
		if(!_r_array_ && !_k_array_) return false;

		return true;
	}

	// Assignment operator - note that none of these change _scale_
#if(1)

	/// Copy assignment
	radial_vector & operator=(const radial_vector & other) = default;

	/// Move assignment
	radial_vector & operator=(radial_vector && other) = default;

	/// Copy/move from Eigen array - this loads into real-space array
	template< typename To, typename Ts,
		typename std::enable_if<std::is_same<typename std::decay<To>::type,flt_type>::value,char>::type = 0>
	radial_vector & operator=(To && other)
	{
		_N_ = other.size();
		// Don't change _scale_
		_r_array_ = std::forward<To>(other);
		_k_array_ = boost::none;
		assert(is_good());
		return *this;
	}

	/// Convert from Eigen-like array - this loads into real-space array
	template< typename To, typename Ts,
		typename std::enable_if<!std::is_same<typename std::decay<To>::type,flt_type>::value,char>::type = 0,
		typename std::enable_if<std::is_convertible<To,flt_type>::value,char>::type = 0>
	radial_vector & operator=(const To & other)
	{
		_N_ = other.size();
		// Don't change _scale_
		_r_array_ = other;
		_k_array_ = boost::none;
		assert(is_good());
		return *this;
	}

	/// Convert from container - this loads into real-space array
	template< typename To, typename Ts,
		typename std::enable_if<!std::is_same<typename std::decay<To>::type,flt_type>::value,char>::type = 0,
		typename std::enable_if<!std::is_convertible<To,flt_type>::value,char>::type = 0,
		typename std::enable_if<IceBRG::is_container<To>::value,char>::type = 0>
	radial_vector & operator=(const To & other)
	{
		_N_ = other.size();
		// Don't change _scale_
		_r_array_ = IceBRG::coerce<flt_array_type>(other);
		_k_array_ = boost::none;
		assert(is_good());
		return *this;
	}

#endif // Assignment operators

	// Getters
#if(1)

	const flt_array_type & get_k_array() const
	{
		assert(is_good());
		if(!_k_array_) _calculate_k_array();
		return *_k_array_;
	}

	const flt_array_type & get_r_array() const
	{
		assert(is_good());
		if(!_r_array_) _calculate_r_array();
		return *_r_array_;
	}

	const size_type & get_N() const noexcept
	{
		return _N_;
	}

	const flt_type & get_scale() const noexcept
	{
		return _scale_;
	}

	boost::optional<fftw_wisdom_accumulator &> & get_wisdom() const noexcept
	{
		return _wisdom_;
	}

#endif // Getters

	// Setters
#if(1)

	void set_k_array(const flt_array_type & array)
	{
		_k_array_ = array;
		_r_array_ = boost::none;
		_N_ = _k_array_->size();
	}

	void set_r_array(const flt_array_type & array)
	{
		_r_array_ = array;
		_k_array_ = boost::none;
		_N_ = _r_array_->size();
	}

	void set_scale(const flt_type & scale)
	{
		_scale_ = scale;
	}

	void set_wisdom(const boost::optional<fftw_wisdom_accumulator &> & wisdom)
	{
		_wisdom_ = boost::none; // First set it to none so the destructor is triggered
		_wisdom_ = wisdom;
	}

#endif

	// Get min/max r and k
#if(1)
	flt_type r_min() const
	{
		return r_min(_scale_,_N_);
	}
	flt_type r_max() const
	{
		return r_max(_scale_,_N_);
	}
	flt_type k_min() const
	{
		return k_min(_scale_,_N_);
	}
	flt_type k_max() const
	{
		return k_max(_scale_,_N_);
	}
#endif // Get minimum/maximum r or k for arbitrary scale and N

	// Get min/max r and k for arbitrary scale and N
#if(1)
	static flt_type r_min(const flt_type & scale, const int_type & N)
	{
		return scale/(2*N);
	}
	static flt_type r_max(const flt_type & scale, const int_type & N)
	{
		return scale - r_min(scale,N);
	}
	static flt_type k_min(const flt_type & scale, const int_type & N)
	{
		return 1./scale;
	}
	static flt_type k_max(const flt_type & scale, const int_type & N)
	{
		return N/scale - k_min(scale,N);
	}
#endif // Get min/max r and k for arbitrary scale and N

	// Rescale
#if(1)

	/**
	 * Rescale - change scale and conserving data so it represents the same
	 * physical data as well as possible.
	 */
	void rescale( const flt_type & new_scale, size_type new_N=-1 )
	{
		// If new_N wasn't given or is invalid, don't change _N_
		if(new_N<2) new_N = _N_;

		// Check that there's actually a change
		if((new_scale==_scale_) && (new_N==_N_)) return;

		// Fork depending on whether we want to rescale using the
		// r array or the k array
		if(_use_r_array_for_rescale(new_scale,new_N))
		{
			_rescale_r_array(new_scale,new_N);
		}
		else
		{
			_rescale_k_array(new_scale,new_N);
		}
	}

	void rescale_r_array( const flt_type & new_scale, size_type new_N )
	{
		if(new_N<2) new_N = _N_;

		// Check that there's actually a change
		if((new_scale==_scale_) && (new_N==_N_)) return;

		_rescale_r_array(new_scale,new_N);

	}

	void rescale_k_array( const flt_type & new_scale, size_type new_N )
	{
		if(new_N<2) new_N = _N_;

		// Check that there's actually a change
		if((new_scale==_scale_) && (new_N==_N_)) return;

		_rescale_k_array(new_scale,new_N);
	}


	/**
	 * Get a rescaled copy of this. The rescale
	 */
	radial_vector get_rescaled( const flt_type & new_scale, size_type new_N=-1 )
	{
		radial_vector new_radial_vector(*this);

		new_radial_vector.rescale(new_scale,new_N);

		return new_radial_vector;
	}

#endif // Rescale

	// Methods for rescaling to match up with another radial_vector
#if(1)

	void rescale_r_to_match(const radial_vector & other)
	{
		assert(is_good() && other.is_good());
		rescale_r_array(other.get_scale(), other.get_N());
	}

	void rescale_k_to_match(const radial_vector & other)
	{
		assert(is_good() && other.is_good());
		rescale_k_array(other.get_scale(), other.get_N());
	}

	void rescale_to_match(const radial_vector & other)
	{
		assert(is_good() && other.is_good());
		rescale(other.get_scale(), other.get_N());
	}

	radial_vector get_rescaled_r_to_match(const radial_vector & other) const
	{
		radial_vector copy(*this);
		copy.rescale_r_to_match(other);
		return copy;
	}

	radial_vector get_rescaled_k_to_match(const radial_vector & other) const
	{
		radial_vector copy(*this);
		copy.rescale_k_to_match(other);
		return copy;
	}

	radial_vector get_rescaled_to_match(const radial_vector & other) const
	{
		radial_vector copy(*this);
		copy.rescale_to_match(other);
		return copy;
	}

#endif // Methods for rescaling to match up with another radial_vector

	// Convolution methods
#if(1)

	void convolve_with(const radial_vector & other)
	{
		assert(is_good() && other.is_good());

		// Make sure they're both of the same scale
		if((get_scale()==other.get_scale()) && (get_N()==other.get_N()))
		{
			set_k_array(_convolution_factor() * get_k_array() * other.get_k_array());
		}
		else
		{

			if(_this_is_better_for_r_rescaling(other))
			{
				rescale_r_to_match(other);

				set_k_array(_convolution_factor() * get_k_array() * other.get_k_array());
			}
			else
			{
				radial_vector rescaled_other(other.get_rescaled_r_to_match(*this));

				set_k_array(_convolution_factor() * get_k_array() * rescaled_other.get_k_array());
			}

		}
	}

	radial_vector get_convolved_with(const radial_vector & other) const
	{
		radial_vector copy(*this);
		copy.convolve_with(other);
		return copy;
	}

#endif
};

} // namespace Fourier

// Pull the radial vector out into the brgastro namespace

using Fourier::radial_vector;

} // namespace IceBRG

#endif // BRG_MATH_FOURIER_RADIAL_VECTOR_HPP_
