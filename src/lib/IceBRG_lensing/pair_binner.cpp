/**********************************************************************\
 @file pair_binner.cpp
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

#include <algorithm>
#include <fstream>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <vector>

#include "IceBRG_main/common.h"

#include "IceBRG_main/utility.hpp"
#include "IceBRG_main/vector/limit_vector.hpp"
#include "IceBRG_main/vector/make_vector.hpp"
#include "IceBRG_main/vector/summary_functions.hpp"

#include "pair_binner.h"
#include "IceBRG_main/file_access/ascii_table.hpp"
#include "IceBRG_main/units/units.hpp"

namespace IceBRG {

flt_type lens_id::unmasked_frac(const distance_type & R_proj) const
{
	try
	{
		return unmasked_frac_bin_limits.interpolate_bins(R_proj,unmasked_fracs);
	}
	catch(const std::logic_error &e)
	{
		return 1;
	}
}

	// Private methods
#if(1)

void pair_binner::_sort() const
{
	if(_sorted_) return;

	if(!valid_limits()) throw std::logic_error("Pairs can't be sorted without valid bin limits.");

	// Check if we have a lens to sort
	if(!_buffering_lens_id_)
	{
		_empty_buffer(); // Anything in the buffer must be junk
		return;
	}

	// Check if the lens is within bounds first
	const mass_type & m = _buffering_lens_id_->m;
	const flt_type & z = _buffering_lens_id_->z;
	const flt_type & mag = _buffering_lens_id_->mag;
	if(m_limits().outside_limits(m) || z_limits().outside_limits(z) || mag_limits().outside_limits(mag))
	{
		// This lens can't be binned, and therefore no pairs currently buffered can be binned, so empty
		// the buffer and return
		_empty_buffer();
		return;
	}

	// Get the bin indices for this lens
	const ssize_t m_i = m_limits().get_bin_index(m);
	const ssize_t z_i = z_limits().get_bin_index(z);
	const ssize_t mag_i = mag_limits().get_bin_index(mag);

	_pair_bins_buffer_.resize(R_limits().num_bins());

	ssize_t R_i = 0;
	for( auto & bin : _pair_bins_buffer_ )
	{
		bin.clear();
		bin.set_limits(R_limits().lower_limit(R_i),R_limits().upper_limit(R_i),
				m_limits().lower_limit(m_i),m_limits().upper_limit(m_i),
				z_limits().lower_limit(z_i),z_limits().upper_limit(z_i),
				mag_limits().lower_limit(mag_i),mag_limits().upper_limit(mag_i));
		bin.set_z_buffer(_z_buffer_);
		bin.add_lens(*_buffering_lens_id_);
		++R_i;
	}

	// Now loop through and sort each pair into the proper bin
	// Note that we don't zero the sorting index here. This is intentional, so
	// we don't have to resort previously sorted pairs when new ones are added.

	for(const auto & pair : _pairs_)
	{

		// Check bounds first
		distance_type R_proj = pair.R_proj();
		if(R_limits().outside_limits(R_proj)) continue;

		// Check lens-source z separation against buffer
		if(pair.z_diff()<_z_buffer_-std::numeric_limits<flt_type>::epsilon()) continue;

		// Add the pair to the proper bin
		_pair_bins_buffer_[R_limits().get_bin_index(R_proj)].add_pair(pair);
	}

	// Add the buffer to the summaries
	_construct();
	for(ssize_t R_i = 0; R_i<R_limits().num_bins(); ++R_i)
	{
		_pair_bin_summaries_[R_i][m_i][z_i][mag_i] += _pair_bins_buffer_[R_i];
	}

	_past_lens_ids_.insert(*_buffering_lens_id_);
	_empty_buffer();
}


void pair_binner::_empty_buffer() const
{
	_buffering_lens_id_ = boost::none;
	_pairs_.clear();
	_sorted_ = true;
}

#endif // Private functions

// Adding and clearing data
#if(1)

bool pair_binner::binnable( const galaxy & lens ) const
{
	if(!valid_limits()) return false;
	if(m_limits().outside_limits(lens.m())) return false;
	if(z_limits().outside_limits(lens.z())) return false;
	if(mag_limits().outside_limits(lens.mag())) return false;
	return true;
}
void pair_binner::add_pair( const lens_source_pair & new_pair)
{
	add_lens_id(new_pair.lens()->index(),new_pair.m_lens(),new_pair.z_lens(),new_pair.mag_lens(),new_pair.weight_lens());
	_pairs_.push_back(new_pair);
	_sorted_ = false;
}
void pair_binner::add_lens_id( const ssize_t & new_id, const mass_type & m, const flt_type & z,
		const flt_type & mag, const flt_type & weight)
{
	// Check if we're already buffering a different lens
	if(_buffering_lens_id_)
	{
		if(_buffering_lens_id_->id!=new_id)
		{
			// If we get here, this is a new lens, so sort any existing data, which will empty the buffer
			_sort();
		}
		else
		{
			// If we get here, we're already buffering this lens, so just return
			return;
		}
	}

	auto new_lens_id = lens_id(new_id,m,z,mag,_unmasked_frac_bin_limits_,_unmasked_fracs_, weight);

	// Check if this id has been used in the past
	if(_past_lens_ids_.find(new_lens_id)!=_past_lens_ids_.end())
	{
		// If we've used it before, throw an exception. This implementation can't handle that
		throw std::runtime_error("Lens has been added to binner multiple times with another lens in between. This implementation is optimized\nand only allows each lens to be binned once.");
	}

	_buffering_lens_id_ = boost::optional<lens_id>(new_lens_id);

	_sorted_ = false;
}
void pair_binner::clear()
{
	_empty_buffer();
	_pair_bin_summaries_.clear();
}
void pair_binner::empty()
{
	_empty_buffer();
	_pair_bin_summaries_.empty();
}
void pair_binner::sort() const
{
	_sort();
}

#endif // Adding and clearing data

// Accessing summary data for bins
#if(1)

// Access by index (will throw if out of bounds)
#if(1)
surface_density_type pair_binner::delta_Sigma_t_mean_for_bin(ssize_t R_i, ssize_t m_i, ssize_t z_i, ssize_t mag_i)
{
	_sort();
	return _pair_bin_summaries_.at(R_i).at(m_i).at(z_i).at(mag_i).delta_Sigma_t_mean();
}
surface_density_type pair_binner::delta_Sigma_x_mean_for_bin(ssize_t R_i, ssize_t m_i, ssize_t z_i, ssize_t mag_i)
{
	_sort();
	return _pair_bin_summaries_.at(R_i).at(m_i).at(z_i).at(mag_i).delta_Sigma_x_mean();
}

surface_density_type pair_binner::delta_Sigma_t_std_for_bin(ssize_t R_i, ssize_t m_i, ssize_t z_i, ssize_t mag_i)
{
	_sort();
	return _pair_bin_summaries_.at(R_i).at(m_i).at(z_i).at(mag_i).delta_Sigma_t_std();
}
surface_density_type pair_binner::delta_Sigma_x_std_for_bin(ssize_t R_i, ssize_t m_i, ssize_t z_i, ssize_t mag_i)
{
	_sort();
	return _pair_bin_summaries_.at(R_i).at(m_i).at(z_i).at(mag_i).delta_Sigma_x_std();
}

surface_density_type pair_binner::delta_Sigma_t_stderr_for_bin(ssize_t R_i, ssize_t m_i, ssize_t z_i, ssize_t mag_i)
{
	_sort();
	return _pair_bin_summaries_.at(R_i).at(m_i).at(z_i).at(mag_i).delta_Sigma_t_stderr();
}
surface_density_type pair_binner::delta_Sigma_x_stderr_for_bin(ssize_t R_i, ssize_t m_i, ssize_t z_i, ssize_t mag_i)
{
	_sort();
	return _pair_bin_summaries_.at(R_i).at(m_i).at(z_i).at(mag_i).delta_Sigma_t_stderr();
}
#endif // Access by index

// Access by position
#if(1)
surface_density_type pair_binner::delta_Sigma_t_mean_for_bin(const distance_type & R, const mass_type & m,
												  const flt_type & z, const flt_type & mag)
{
	_sort();
	ssize_t R_i = R_limits().get_bin_index(R);
	ssize_t m_i = m_limits().get_bin_index(m);
	ssize_t z_i = z_limits().get_bin_index(z);
	ssize_t mag_i = mag_limits().get_bin_index(mag);
	return _pair_bin_summaries_.at(R_i).at(m_i).at(z_i).at(mag_i).delta_Sigma_t_mean();
}
surface_density_type pair_binner::delta_Sigma_x_mean_for_bin(const distance_type & R, const mass_type & m,
												  const flt_type & z, const flt_type & mag)
{
	_sort();
	ssize_t R_i = R_limits().get_bin_index(R);
	ssize_t m_i = m_limits().get_bin_index(m);
	ssize_t z_i = z_limits().get_bin_index(z);
	ssize_t mag_i = mag_limits().get_bin_index(mag);
	return _pair_bin_summaries_.at(R_i).at(m_i).at(z_i).at(mag_i).delta_Sigma_x_mean();
}

surface_density_type pair_binner::delta_Sigma_t_std_for_bin(const distance_type & R, const mass_type & m,
												 const flt_type & z, const flt_type & mag)
{
	_sort();
	ssize_t R_i = R_limits().get_bin_index(R);
	ssize_t m_i = m_limits().get_bin_index(m);
	ssize_t z_i = z_limits().get_bin_index(z);
	ssize_t mag_i = mag_limits().get_bin_index(mag);
	return _pair_bin_summaries_.at(R_i).at(m_i).at(z_i).at(mag_i).delta_Sigma_t_std();
}
surface_density_type pair_binner::delta_Sigma_x_std_for_bin(const distance_type & R, const mass_type & m,
												 const flt_type & z, const flt_type & mag)
{
	_sort();
	ssize_t R_i = R_limits().get_bin_index(R);
	ssize_t m_i = m_limits().get_bin_index(m);
	ssize_t z_i = z_limits().get_bin_index(z);
	ssize_t mag_i = mag_limits().get_bin_index(mag);
	return _pair_bin_summaries_.at(R_i).at(m_i).at(z_i).at(mag_i).delta_Sigma_x_std();
}

surface_density_type pair_binner::delta_Sigma_t_stderr_for_bin(const distance_type & R, const mass_type & m,
													const flt_type & z, const flt_type & mag)
{
	_sort();
	ssize_t R_i = R_limits().get_bin_index(R);
	ssize_t m_i = m_limits().get_bin_index(m);
	ssize_t z_i = z_limits().get_bin_index(z);
	ssize_t mag_i = mag_limits().get_bin_index(mag);
	return _pair_bin_summaries_.at(R_i).at(m_i).at(z_i).at(mag_i).delta_Sigma_t_stderr();
}
surface_density_type pair_binner::delta_Sigma_x_stderr_for_bin(const distance_type & R, const mass_type & m,
													const flt_type & z, const flt_type & mag)
{
	_sort();
	ssize_t R_i = R_limits().get_bin_index(R);
	ssize_t m_i = m_limits().get_bin_index(m);
	ssize_t z_i = z_limits().get_bin_index(z);
	ssize_t mag_i = mag_limits().get_bin_index(mag);
	return _pair_bin_summaries_.at(R_i).at(m_i).at(z_i).at(mag_i).delta_Sigma_x_stderr();
}
#endif // Access by index

#endif // Accessing summary data for bins

} // end namespace IceBRG
