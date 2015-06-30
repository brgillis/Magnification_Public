/**********************************************************************\
 @file pair_bins_summary.h
 ------------------

 A class which stores summary data on a grid of pair bins.

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

// body file: pair_binner.cpp

#ifndef _BRG_PAIR_BINS_SUMMARY_H_INCLUDED_
#define _BRG_PAIR_BINS_SUMMARY_H_INCLUDED_

#include <limits>
#include <iostream>
#include <string>
#include <vector>

#include <boost/serialization/vector.hpp>

#include "IceBRG_main/common.h"

#include "IceBRG_main/units/unitconv_map.hpp"
#include "IceBRG_main/units/units.hpp"
#include "IceBRG_main/vector/limit_vector.hpp"

#include "IceBRG_lensing/pair_bin_summary.h"

namespace IceBRG {

// Forward declare the pair_binner child
class pair_binner;

/**
 *
 */
class pair_bins_summary {
private:

	// Limits for the bins
#if(1)

	IceBRG::limit_vector< distance_type > _R_bin_limits_;
	IceBRG::limit_vector< mass_type > _m_bin_limits_;
	IceBRG::limit_vector< flt_type > _z_bin_limits_;
	IceBRG::limit_vector< flt_type > _mag_bin_limits_;

	bool _valid_limits_;

#endif // Limits for the bins

	// Private methods
#if(1)

	void _check_limits();
	void _reconstruct() const;

	// Private implementations of setting/clearing limits
	// Unlike the public versions, these don't check for valid
	// limits at the end.
#if(1)

	// Set specific limits through a limits vector
	void _set_R_limits(IceBRG::limit_vector< distance_type > R_bin_limits);
	void _set_m_limits(IceBRG::limit_vector< mass_type > m_bin_limits);
	void _set_z_limits(IceBRG::limit_vector< flt_type > z_bin_limits);
	void _set_mag_limits(IceBRG::limit_vector< flt_type > mag_bin_limits);

	// Set specific limits through a linear spacing
	void _set_linear_R_limits(const distance_type & R_min,
			const distance_type & R_max,
			const ssize_t & R_bins);
	void _set_linear_m_limits(const mass_type & m_min,
			const mass_type & m_max,
			const ssize_t & m_bins);
	void _set_linear_z_limits(const flt_type & z_min,
			const flt_type & z_max,
			const ssize_t & z_bins);
	void _set_linear_mag_limits(const flt_type & mag_min,
			const flt_type & mag_max,
			const ssize_t & maag_bins);

	// Set specific limits through a log spacing
	void _set_log_R_limits(const distance_type & R_min,
			const distance_type & R_max,
			const ssize_t & R_num_bins=1);
	void _set_log_m_limits(const mass_type & m_min,
			const mass_type & m_max,
			const ssize_t & m_num_bins=1);
	void _set_log_z_limits(const flt_type & z_min,
			const flt_type & z_max,
			const ssize_t & z_num_bins=1);
	void _set_log_mag_limits(const flt_type & mag_min,
			const flt_type & mag_max,
			const ssize_t & mag_num_bins=1);

	// Clear limits. That is, make them unbound - one bin from neg infinity to pos infinity
	void _clear_R_limits();
	void _clear_m_limits();
	void _clear_z_limits();
	void _clear_mag_limits();

#endif

	// Serialization (to allow it to be saved)
#if(1)
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const int_type version)
    {
    	ar & _R_bin_limits_;
    	ar & _m_bin_limits_;
    	ar & _z_bin_limits_;
    	ar & _mag_bin_limits_;

    	ar & _valid_limits_;

    	ar & _pair_bin_summaries_;
    }
#endif

#endif

protected:

	// The bins, contained in a 4D vector, for one dimension for each parameter
	mutable std::vector< std::vector< std::vector< std::vector<pair_bin_summary> > > > _pair_bin_summaries_;

	void _construct() const
	{
		if(_pair_bin_summaries_.empty()) _reconstruct();
	}

public:

	// Constructors and destructor
#if(1)
	// Default constructor
	pair_bins_summary()
	: _valid_limits_(false)
	{
	}

	// Set limits by vectors
	pair_bins_summary(IceBRG::limit_vector< distance_type > R_bin_limits,
			IceBRG::limit_vector< mass_type > m_bin_limits=std::vector<flt_type>(),
			IceBRG::limit_vector< flt_type > z_bin_limits=std::vector<flt_type>(),
			IceBRG::limit_vector< flt_type > mag_bin_limits=std::vector<flt_type>());

	// Set limits by min, max, and step
	pair_bins_summary(const distance_type & R_min,
				const distance_type & R_max,
				const ssize_t & R_bins=1,
				const mass_type & m_min=-std::numeric_limits<flt_type>::infinity()*kg,
				const mass_type & m_max=std::numeric_limits<flt_type>::infinity()*kg,
				const ssize_t & m_bins=1,
				const flt_type & z_min=-std::numeric_limits<flt_type>::infinity(),
				const flt_type & z_max=std::numeric_limits<flt_type>::infinity(),
				const ssize_t & z_bins=1,
				const flt_type & mag_min=-std::numeric_limits<flt_type>::infinity(),
				const flt_type & mag_max=std::numeric_limits<flt_type>::infinity(),
				const ssize_t & mag_bins=1);

	// Load from archive
	pair_bins_summary( std::istream & in );
	pair_bins_summary( const std::string & filename );

	// Construct from pair_bins
	pair_bins_summary( const pair_binner & bins);
	pair_bins_summary & operator=(const pair_binner & bins)
	{
		*this = pair_bins_summary(bins);
		return *this;
	}

	virtual ~pair_bins_summary()
	{
	}
#endif

	// Set/change limits
#if(1)

	// Set specific limits through a limits vector
	void set_R_limits(IceBRG::limit_vector< distance_type > R_bin_limits);
	void set_m_limits(IceBRG::limit_vector< mass_type > m_bin_limits);
	void set_z_limits(IceBRG::limit_vector< flt_type > z_bin_limits);
	void set_mag_limits(IceBRG::limit_vector< flt_type > mag_bin_limits);

	// Set specific limits through a linear spacing
	void set_linear_R_limits(const distance_type & R_min,
			const distance_type & R_max,
			const ssize_t & R_bins);
	void set_linear_m_limits(const mass_type & m_min,
			const mass_type & m_max,
			const ssize_t & m_bins);
	void set_linear_z_limits(const flt_type & z_min,
			const flt_type & z_max,
			const ssize_t & z_bins);
	void set_linear_mag_limits(const flt_type & mag_min,
			const flt_type & mag_max,
			const ssize_t & mag_bins);

	// Set specific limits through a log spacing
	void set_log_R_limits(const distance_type & R_min,
			const distance_type & R_max,
			const ssize_t & R_num_bins=1);
	void set_log_m_limits(const mass_type & m_min,
			const mass_type & m_max,
			const ssize_t & m_num_bins=1);
	void set_log_z_limits(const flt_type & z_min,
			const flt_type & z_max,
			const ssize_t & z_num_bins=1);
	void set_log_mag_limits(const flt_type & mag_min,
			const flt_type & mag_max,
			const ssize_t & mag_num_bins=1);

	// Clear limits. That is, make them unbound - one bin from neg infinity to pos infinity
	void clear_R_limits();
	void clear_m_limits();
	void clear_z_limits();
	void clear_mag_limits();

	void set_limits(IceBRG::limit_vector< distance_type > R_bin_limits,
			IceBRG::limit_vector< mass_type > m_bin_limits=std::vector<flt_type>(),
			IceBRG::limit_vector< flt_type > z_bin_limits=std::vector<flt_type>(),
			IceBRG::limit_vector< flt_type > mag_bin_limits=std::vector<flt_type>());

	void set_linear_limits(const distance_type & R_min,
				const distance_type & R_max,
				const ssize_t & R_num_bins=1,
				const mass_type & m_min=-std::numeric_limits<flt_type>::infinity()*kg,
				const mass_type & m_max=std::numeric_limits<flt_type>::infinity()*kg,
				const ssize_t & m_num_bins=1,
				const flt_type & z_min=-std::numeric_limits<flt_type>::infinity(),
				const flt_type & z_max=std::numeric_limits<flt_type>::infinity(),
				const ssize_t & z_num_bins=1,
				const flt_type & mag_min=-std::numeric_limits<flt_type>::infinity(),
				const flt_type & mag_max=std::numeric_limits<flt_type>::infinity(),
				const ssize_t & mag_num_bins=1);

	void set_log_limits(const distance_type & R_min,
				const distance_type & R_max,
				const ssize_t & R_num_bins=1,
				const mass_type & m_min=-std::numeric_limits<flt_type>::infinity()*kg,
				const mass_type & m_max=std::numeric_limits<flt_type>::infinity()*kg,
				const ssize_t & m_num_bins=1,
				const flt_type & z_min=-std::numeric_limits<flt_type>::infinity(),
				const flt_type & z_max=std::numeric_limits<flt_type>::infinity(),
				const ssize_t & z_num_bins=1,
				const flt_type & mag_min=-std::numeric_limits<flt_type>::infinity(),
				const flt_type & mag_max=std::numeric_limits<flt_type>::infinity(),
				const ssize_t & mag_num_bins=1);

#endif // Set/change limits

	// Fix bad values
	void fixbad();

	// Accessors to limit vectors and valid_limits
#if(1)

	const IceBRG::limit_vector< distance_type > & R_limits() const
	{
		return _R_bin_limits_;
	}
	const IceBRG::limit_vector< mass_type > & m_limits() const
	{
		return _m_bin_limits_;
	}
	const IceBRG::limit_vector< flt_type > & z_limits() const
	{
		return _z_bin_limits_;
	}
	const IceBRG::limit_vector< flt_type > & mag_limits() const
	{
		return _mag_bin_limits_;
	}
	bool valid_limits() const
	{
		return _valid_limits_;
	}

#endif

	// Adding, sorting, and clearing data
#if(1)

	virtual void sort() const
	{
		_construct();
	}
	virtual void clear();

#endif // Adding and clearing data

	// Accessing summary data for bins
#if(1)

	// Accessing full vector
	const std::vector< std::vector< std::vector< std::vector<pair_bin_summary> > > > &
		pair_bin_summaries() const
	{
		_construct();
		return _pair_bin_summaries_;
	}

	// Access by index (will throw if out of bounds)
#if(1)
	virtual surface_density_type delta_Sigma_t_mean_for_bin(ssize_t R_i, ssize_t m_i, ssize_t z_i, ssize_t mag_i) const;
	virtual surface_density_type delta_Sigma_x_mean_for_bin(ssize_t R_i, ssize_t m_i, ssize_t z_i, ssize_t mag_i) const;

	virtual surface_density_type delta_Sigma_t_std_for_bin(ssize_t R_i, ssize_t m_i, ssize_t z_i, ssize_t mag_i) const;
	virtual surface_density_type delta_Sigma_x_std_for_bin(ssize_t R_i, ssize_t m_i, ssize_t z_i, ssize_t mag_i) const;

	virtual surface_density_type delta_Sigma_t_stderr_for_bin(ssize_t R_i, ssize_t m_i, ssize_t z_i, ssize_t mag_i) const;
	virtual surface_density_type delta_Sigma_x_stderr_for_bin(ssize_t R_i, ssize_t m_i, ssize_t z_i, ssize_t mag_i) const;
#endif // Access by index

	// Access by position
#if(1)
	virtual surface_density_type delta_Sigma_t_mean_for_bin(const distance_type & R, const mass_type & m,
			flt_type z, flt_type mag) const;
	virtual surface_density_type delta_Sigma_x_mean_for_bin(const distance_type & R, const mass_type & m,
			flt_type z, flt_type mag) const;

	virtual surface_density_type delta_Sigma_t_std_for_bin(const distance_type & R, const mass_type & m,
			flt_type z, flt_type mag) const;
	virtual surface_density_type delta_Sigma_x_std_for_bin(const distance_type & R, const mass_type & m,
			flt_type z, flt_type mag) const;

	virtual surface_density_type delta_Sigma_t_stderr_for_bin(const distance_type & R, const mass_type & m,
			flt_type z, flt_type mag) const;
	virtual surface_density_type delta_Sigma_x_stderr_for_bin(const distance_type & R, const mass_type & m,
			flt_type z, flt_type mag) const;
#endif // Access by index

#endif // Accessing summary data for bins

	// Print data for all bins
#if (1)

	void print_bin_data(std::ostream &out,
			const unitconv_map & u_map = unitconv_map());
	void print_bin_data(const std::string & file_name,
			const unitconv_map & u_map = unitconv_map());

#endif

	// Operators to combine data
#if (1)

	pair_bins_summary & operator+=( const pair_bins_summary & other);
	pair_bins_summary operator+( pair_bins_summary other ) const
	{
		return other += *this;
	}

#endif

	// Saving/loading data
#if(1)

	void save(std::ostream &) const;
	void save(const std::string &) const;
	void save(std::ostream &);
	void save(const std::string &);
	void load(std::istream &);
	void load(const std::string &);

#endif

};

} // end namespace IceBRG

#endif // _BRG_PAIR_BINS_SUMMARY_H_INCLUDED_
