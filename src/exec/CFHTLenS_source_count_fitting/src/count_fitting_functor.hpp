/**********************************************************************\
 @file count_fitting_functor.hpp
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

// body file: count_fitting_functor.cpp

#ifndef _BRG_COUNT_FITTING_FUNCTOR_H_INCLUDED_
#define _BRG_COUNT_FITTING_FUNCTOR_H_INCLUDED_

#include "IceBRG_main/common.h"

#include "IceBRG_main/math/functor/functor.hpp"
#include "IceBRG_main/units/unit_conversions.hpp"
#include "IceBRG_main/units/units.hpp"

namespace IceBRG {

/**
 *
 */
template< typename Tf >
class count_fitting_functor {
public:

	typedef Tf functor_type;

private:

	square_angle_type _field_size_;
	functor_type *_f_;
	std::string _filename_;
	double _z_bin_size_;

	mutable std::vector<double> _mag_bin_limits_;
	mutable std::vector<double> _mag_bin_counts_;
	mutable bool _loaded_;

	void _load() const
	{
		if(_loaded_) return;
		auto map = load_table_map<double>(_filename_);

		const auto count_label = "count";
		const auto bin_lower_label = "mag_bin_lower";

		size_t num_bins = map[count_label].size();
		_mag_bin_counts_.reserve(num_bins-2);
		_mag_bin_limits_.reserve(num_bins-1);

		// Fill in all but the overflow bins

		for(size_t i=1; i<num_bins-1; ++i)
		{
			_mag_bin_counts_.push_back(map[count_label][i]);
			_mag_bin_limits_.push_back(map[bin_lower_label][i]);
		}
		_mag_bin_limits_.push_back(map[bin_lower_label][num_bins-1]);

		_loaded_ = true;
	}

public:
	count_fitting_functor()
	: _field_size_(1*radian*radian),
	  _f_(NULL),
	  _filename_(""),
	  _z_bin_size_(1),
	  _loaded_(false)

	{
	}
	count_fitting_functor(functor_type *init_f = nullptr,
			std::string init_filename="",
			square_angle_type init_field_size=1*radian*radian,
			double init_z_bin_size=1)
	: _field_size_(init_field_size),
	  _f_(init_f),
	  _filename_(init_filename),
	  _z_bin_size_(init_z_bin_size),
	  _loaded_(false)
	{
	}
	virtual ~count_fitting_functor()
	{
	}

	// Setters
#if (1)

	void set_function(functor_type *new_f)
	{
		_f_ = new_f;
	}
	void set_filename(std::string new_filename)
	{
		_filename_ = new_filename;
	}
	void set_field_size(const square_angle_type & new_field_size)
	{
		assert(new_field_size>0.*rad*rad);
		_field_size_ = new_field_size;
	}
	void set_z_bin_size_size(double new_z_bin_size)
	{
		_z_bin_size_ = new_z_bin_size;
	}

#endif

	// Accessors
#if (1)
	const functor_type *function()
	{
		return _f_;
	}
	const std::string & filename()
	{
		return _filename_;
	}
	const square_angle_type & field_size() const
	{
		return _field_size_;
	}
	double z_bin_size() const
	{
		return _z_bin_size_;
	}
#endif

	// Function method

	double operator()( const typename functor_type::params_type & in_params ) const
	{
		assert(_f_!=nullptr);

		_load();

		_f_->set_params(in_params);

		double chi_sq = 0;
		for(size_t i=0; i<_mag_bin_counts_.size(); ++i)
		{
			if(_mag_bin_counts_[i]<=0) continue;
			double mid = (_mag_bin_limits_[i]+_mag_bin_limits_[i+1])/2;
			square_angle_type size = _z_bin_size_*field_size()*(_mag_bin_limits_[i+1]-_mag_bin_limits_[i]);
			double estimate = (*_f_)(mid)*size;
			double error = std::sqrt(estimate);
			chi_sq += square( (estimate-_mag_bin_counts_[i]) / error);
		}

		if(chi_sq<=0) throw std::runtime_error("No observed galaxies in this redshift bin.");

		return chi_sq;
	}
};

} // namespace IceBRG

#endif // _BRG_COUNT_FITTING_FUNCTOR_H_INCLUDED_
