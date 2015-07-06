/**********************************************************************\
  @file cache_nd.hpp

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

#ifndef _BRG_CACHE_ND_HPP_INCLUDED_
#define _BRG_CACHE_ND_HPP_INCLUDED_

#ifndef BRG_CACHE_ND_NAME_SIZE
#define BRG_CACHE_ND_NAME_SIZE 9 // Needs an end character, so will only actually allow 8 chars
#endif

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <exception>

#include "IceBRG_main/common.h"

#include "IceBRG_main/error_handling.h"
#include "IceBRG_main/file_access/ascii_table.hpp"
#include "IceBRG_main/file_access/open_file.hpp"
#include "IceBRG_main/units/units.hpp"
#include "IceBRG_main/utility.hpp"
#include "IceBRG_main/vector/multi_vector.hpp"
#include "IceBRG_main/vector/elementwise_functions.hpp"

// Macro definitions

// SPCP: "Static Polymorphic Const Pointer"
// The constness isn't actually enforced, but this is for the reader's understanding
#define SPCP(name) static_cast<const name*>(this)

// SPP: "Static Polymorphic Pointer"
#define SPP(name) static_cast<name*>(this)

#define DECLARE_BRG_CACHE_ND_STATIC_VARS()		       \
	static IceBRG::multi_vector<flt_type> _mins_, _maxes_, _steps_;      \
	static IceBRG::multi_vector<ssize_t> _resolutions_;           \
	static IceBRG::multi_vector<flt_type> _results_;                     \
											                       \
	static std::string _file_name_;                                \
	static int_type _version_number_;                          \
										                           \
	static bool _loaded_, _initialised_;                           \
											                       \
	static int_type _num_dim_;

// Be careful when using this not to use the default constructor for init_steps, which would result in
// divide-by-zero errors
#define DEFINE_BRG_CACHE_ND_STATIC_VARS(class_name,init_mins,init_maxes,init_steps,init_num_dim) \
	IceBRG::vector<flt_type> IceBRG::class_name::_mins_ = init_mins;	                         \
	IceBRG::vector<flt_type> IceBRG::class_name::_maxes_ = init_maxes;                         \
	IceBRG::vector<flt_type> IceBRG::class_name::_steps_ = init_steps;                         \
	bool IceBRG::class_name::_loaded_ = false;							                     \
	bool IceBRG::class_name::_initialised_ = false;					                         \
	IceBRG::vector<ssize_t> IceBRG::class_name::_resolutions_ =                         \
		max( (((IceBRG::class_name::_maxes_-IceBRG::class_name::_mins_) /                    \
				safe_d(IceBRG::class_name::_steps_))+1), 1);                                   \
	std::string IceBRG::class_name::_file_name_ = "";					     	                 \
	int_type IceBRG::class_name::_version_number_ = 1;		     		                 \
	IceBRG::vector<flt_type> IceBRG::class_name::_results_;                                    \
	int_type IceBRG::class_name::_num_dim_ = init_num_dim;

namespace IceBRG
{

template<typename name>
class brg_cache_nd
{
private:

	// Private variables
#if (1)

	DECLARE_BRG_CACHE_ND_STATIC_VARS();

#endif // Private variables

	// Private methods
#if (1)
	void _init() const
	{
		// We check for initialisation twice due to the critical section here.
		// It's expensive to enter, and we don't want to do anything inside it more than once,
		// so we check whether we need to both once outside it and once inside it.
		if(SPCP(name)->_initialised_) return;

		#pragma omp critical(init_brg_cache_nd)
		if(!SPCP(name)->_initialised_)
		{
			SPCP(name)->_resolutions_ = max( (((SPCP(name)->_maxes_-SPCP(name)->_mins_) / safe_d(SPCP(name)->_steps_))+1), 1);
			SPCP(name)->_file_name_ = SPCP(name)->_name_base() + "_cache.bin";
			SPCP(name)->_version_number_ = 1; // This should be changed when there are changes to this code

			SPCP(name)->_initialised_ = true;
		}
	}
	void _load() const
	{
		std::ifstream in_file;
		std::string file_data;
		bool need_to_calc = false;
		int_type loop_counter = 0;

		if ( SPCP(name)->_loaded_ )
			return;

		do
		{
			if ( loop_counter >= 2 )
			{
				throw std::runtime_error("Infinite loop detected trying to load " + SPCP(name)->_file_name_ + " in IceBRG::brg_cache_nd.\n");
			}
			else
			{
				loop_counter++;
			}
			need_to_calc = false;

			if ( open_bin_file_input( in_file, SPCP(name)->_file_name_ ) )
			{
				need_to_calc = true;
				SPCP(name)->_calc();
				SPCP(name)->_output();
				SPCP(name)->_unload();
				continue;
			}

			// Check that it has the right name and version

			char file_name[BRG_CACHE_ND_NAME_SIZE];
			int_type file_version = std::numeric_limits<int_type>::max();

			in_file.read(file_name,BRG_CACHE_ND_NAME_SIZE);
			in_file.read((char *)&file_version,sizeof(file_version));

			if( (!in_file) || (((std::string)file_name) != SPCP(name)->_name_base()) ||
					(file_version != SPCP(name)->_version_number_) )
			{
				need_to_calc = true;
				SPCP(name)->_calc();
				SPCP(name)->_output();
				SPCP(name)->_unload();
				continue;
			}

			// Load range parameters;
			SPCP(name)->_mins_.resize(SPCP(name)->_num_dim_,0);
			SPCP(name)->_maxes_.resize(SPCP(name)->_num_dim_,0);
			SPCP(name)->_steps_.resize(SPCP(name)->_num_dim_,0);
			for(ssize_t i = 0; i < SPCP(name)->_num_dim_; i++)
			{
				in_file.read((char *)&(SPCP(name)->_mins_[i]),sizeof(SPCP(name)->_mins_[i]));
				in_file.read((char *)&(SPCP(name)->_maxes_[i]),sizeof(SPCP(name)->_maxes_[i]));
				in_file.read((char *)&(SPCP(name)->_steps_[i]),sizeof(SPCP(name)->_steps_[i]));
			}
			if ( !(in_file) )
			{
				need_to_calc = true;
				SPCP(name)->_calc();
				SPCP(name)->_output();
				SPCP(name)->_unload();
				continue;
			}

			// Set up data
			SPCP(name)->_resolutions_ = max( (((SPCP(name)->_maxes_-SPCP(name)->_mins_) / safe_d(SPCP(name)->_steps_))+1.), 1.);
			SPCP(name)->_results_.reshape(SPCP(name)->_resolutions_.v(),0);

			// Read in data

			// Initialize
			ssize_t i = 0;
			const std::streamsize size = sizeof(SPCP(name)->_results_[0]); // Store the size for speed
			IceBRG::multi_vector<ssize_t> position(SPCP(name)->_num_dim_,0);

			while ( ( !in_file.eof() ) && ( i < product(SPCP(name)->_resolutions_) ) && (in_file) )
			{
				in_file.read((char *)&(SPCP(name)->_results_(position)),size);
				i++;

				for(ssize_t d=0; d<SPCP(name)->_num_dim_; d++)
				{
					position[d]++;
					if(position[d] != SPCP(name)->_resolutions_[d])
						break;
					position[d] = 0;
					// If we get here, we'll go on to increase the next index by 1
				}
			}

			// Check that it was all read properly
			if ( (i != product(SPCP(name)->_resolutions_)) || (!in_file) )
			{
				need_to_calc = true;
				SPCP(name)->_calc();
				SPCP(name)->_output();
				SPCP(name)->_unload();
				continue;
			}

		} while ( need_to_calc );

		// Finish up
		in_file.close();
		in_file.clear();
		SPCP(name)->_loaded_ = true;
	}
	void _unload() const
	{
		SPCP(name)->_loaded_ = false;
		SPCP(name)->_results_.clear();
	}
	void _calc() const
	{
		// Test that range is sane
		for(ssize_t i = 0; i < SPCP(name)->_num_dim_; i++)
		{
			try {
				if ( ( SPCP(name)->_maxes_.at(i) <= SPCP(name)->_mins_.at(i) ) || ( SPCP(name)->_steps_.at(i) <= 0 ) )
				{
					throw std::runtime_error("ERROR: Bad range passed to brg_cache::_calc() for " + static_cast<const name*>(this)->_name_base() + "\n");
				}
			} catch (std::exception &e) {
				throw std::runtime_error("ERROR: Bad range passed to brg_cache::_calc() for " + static_cast<const name*>(this)->_name_base() + "\n");
			}
		}

		// Print a message that we're generating the cache
		handle_notification("Generating " + SPCP(name)->_file_name_ + ". This may take some time.");

		// Set up data
		SPCP(name)->_resolutions_.resize(SPCP(name)->_num_dim_);
		SPCP(name)->_resolutions_ = max( (((SPCP(name)->_maxes_-SPCP(name)->_mins_) / safe_d(SPCP(name)->_steps_))+1.), 1.);
		SPCP(name)->_results_.reshape(SPCP(name)->_resolutions_.v() );

		IceBRG::multi_vector<ssize_t> position(SPCP(name)->_num_dim_,0);
		IceBRG::multi_vector<flt_type> x(SPCP(name)->_num_dim_,0);
		for ( ssize_t i = 0; i < size(SPCP(name)->_results_); i++ )
		{
			x = SPCP(name)->_mins_ + SPCP(name)->_steps_*position;
			flt_type result = 0;
			SPCP(name)->_results_(position) = SPCP(name)->_calculate(x);

			for(ssize_t d=0; d<SPCP(name)->_num_dim_; d++)
			{
				position[d]++;
				if(position[d] != SPCP(name)->_resolutions_[d])
					break;
				position[d] = 0;
				// If we get here, we'll go on to increase the next index by 1
			}
		}

		// Print a message that we've finished generating the cache
		handle_notification("Finished generating " << SPCP(name)->_file_name_ << "!");

		SPCP(name)->_loaded_ = true;
	}
	void _output() const
	{

		std::ofstream out_file;
		std::string file_data;

		if ( !SPCP(name)->_loaded_ )
		{
			SPCP(name)->_calc();;
		}

		open_bin_file_output( out_file, SPCP(name)->_file_name_ );

		// Output name and version

		std::string file_name = SPCP(name)->_name_base();
		int_type file_version = SPCP(name)->_version_number_;

		out_file.write(file_name.c_str(),BRG_CACHE_ND_NAME_SIZE);
		out_file.write((char *)&file_version,sizeof(file_version));

		// Output range parameters
		for(ssize_t i = 0; i < SPCP(name)->_num_dim_; i++)
		{
			out_file.write((char *)&(SPCP(name)->_mins_[i]),sizeof(SPCP(name)->_mins_[i]));
			out_file.write((char *)&(SPCP(name)->_maxes_[i]),sizeof(SPCP(name)->_maxes_[i]));
			out_file.write((char *)&(SPCP(name)->_steps_[i]),sizeof(SPCP(name)->_steps_[i]));
		}

		// Output data

		// Initialize
		ssize_t i = 0;
		const std::streamsize size = sizeof(SPCP(name)->_results_[0]);
		IceBRG::multi_vector<ssize_t> position(SPCP(name)->_num_dim_,0);

		while ( i < product(SPCP(name)->_resolutions_) )
		{
			out_file.write((char *)&(SPCP(name)->_results_(position)),size);
			i++;

			for(ssize_t d=0; d<SPCP(name)->_num_dim_; d++)
			{
				position[d]++;
				if(position[d] != SPCP(name)->_resolutions_[d])
					break;
				position[d] = 0;
				// If we get here, we'll go on to increase the next index by 1
			}
		}

		out_file.close();
		out_file.clear();
	}
#endif // Private methods

protected:

	// Protected methods
	// These are made protected instead of private so base classes can overload them
#if (1)

#ifdef _BRG_USE_UNITS_

	// Gets the result in the proper units
	any_units_type _units( const flt_type & v ) const
	{
		return any_units_cast<any_units_type>(v);
	}

#endif // _BRG_USE_UNITS_

	// Long calculation function, which is used to generate the cache
	flt_type _calculate(const IceBRG::multi_vector<flt_type> & x) const;

	// This function should be overloaded to provide a unique name for this cache
	std::string _name_base() const;

	// This function should be overloaded to call each cache of the same dimensionality
	// this cache depends upon in calculation. This is necessary in order to avoid critical
	// sections of the same name being called recursively.
	void _load_cache_dependencies() const
	{
	}

#endif // Protected methods

public:

	// Public methods
#if (1)

	void set_file_name( const std::string new_name )
	{
		if(!SPCP(name)->_initialised_) SPP(name)->_init();
		SPP(name)->_file_name_ = new_name;
		if ( SPCP(name)->_loaded_ )
		{
			SPCP(name)->_unload();
		}
	} // void set_file_name()

	void set_range( const IceBRG::multi_vector<flt_type> & new_mins, const IceBRG::multi_vector<flt_type> & new_maxes,
			const IceBRG::multi_vector<flt_type> & new_steps,)
	{
		if(!SPCP(name)->_initialised_) SPP(name)->_init();

		// First we try to load, so we can see if there are any changes from
		// the existing cache
		if ( !SPCP(name)->_loaded_ )
			SPCP(name)->_load( true );

		// Check sizes of passed vectors
		if( (ssize(new_mins) != SPCP(name)->_num_dim_) || (ssize(new_maxes) != SPCP(name)->_num_dim_) ||
				(ssize(new_steps) != SPCP(name)->_num_dim_) )
		{
			throw std::runtime_error("ERROR: Incorrect sizes of vectors passed to set_range.\n");
		}

		// Go through variables, check if any are actually changed. If so, recalculate cache
		for(ssize_t i = 0; i < SPCP(name)->_num_dim_; i++)
		{
			if ( ( SPCP(name)->_mins_.at(i) != new_mins.at(i) ) || ( SPCP(name)->_maxes_.at(i) != new_maxes.at(i) )
					|| ( SPCP(name)->_steps_.at(i) != new_steps.at(i) ) )
			{
				SPP(name)->_mins_ = new_mins;
				SPP(name)->_maxes_ = new_maxes;
				SPP(name)->_steps_ = new_steps;

				SPCP(name)->_unload();
				SPCP(name)->_calc();
				break;
			}
		}
	} // void set_range()

	const any_units_type get( const IceBRG::multi_vector<flt_type> & x,) const
	{

		IceBRG::multi_vector<flt_type> xlo, xhi;
		IceBRG::multi_vector<ssize_t> x_i; // Lower nearby array points
		flt_type result = 0;

		if(!SPCP(name)->_initialised_) SPCP(name)->_init();

		// Load if necessary
		if ( !SPCP(name)->_loaded_ )
		{
			// Load any caches we depend upon before the critical section
			SPCP(name)->_load_cache_dependencies();

			// Critical section here, since we can't load multiple times simultaneously
			#pragma omp critical(load_brg_cache_nd)
			{
				try
				{
					SPCP(name)->_load();
				}
				catch(const std::exception &e)
				{
					result = -1;
				}
			}
			if ( result == -1 )
			{
				throw std::runtime_error("ERROR: Could neither load " + SPCP(name)->_file_name_ + " nor calculate in brg_cache_nd::get()\n");
			}
		}


		x_i = bound(0,
				( ( x - SPCP(name)->_min_1_ ) / SPCP(name)->_step_1_ ),
				SPCP(name)->_resolution_1_ - 2 );

		xlo = SPCP(name)->_mins_ + SPCP(name)->_steps_ * x_i;
		xhi = SPCP(name)->_mins_ + SPCP(name)->_steps_ * ( x_i + 1 );

		ssize_t num_surrounding_points = 2;
		for(int_type i=0; i<SPCP(name)->_num_dim_-1; ++i ) num_surrounding_points*=2;

		result = 0;
		flt_type total_weight = 0;
		IceBRG::multi_vector<ssize_t> position(SPCP(name)->_num_dim_,0);

		for(ssize_t j=0; j < num_surrounding_points; j++)
		{
			flt_type weight = 1;
			int_type divisor = 1;
			for(ssize_t i=0; i < SPCP(name)->_num_dim_; i++)
			{
				if(divisible(j/divisor,2))
				{
					position[i] = x_i[i]+1;
					weight *= x[i]-xlo[i];
				}
				else
				{
					position[i] = x_i[i];
					weight *= xhi[i]-x[i];
				}
				divisor *= 2;
			}
			result += SPCP(name)->_results_.at(position) * weight;
			total_weight += weight;
		}

		result /= safe_d(total_weight);

#ifdef _BRG_USE_UNITS_
		return SPCP(name)->_units(result);
#else
		return result;
#endif

	} // get()

	// Recalculate function. Call if you want to overwrite a cache when something's changed in the code
	// (for instance, the _calculate() function has been altered)
	void recalc() const
	{
		SPCP(name)->_unload();
		SPCP(name)->_calc();
		SPCP(name)->_output();
		SPCP(name)->_unload();
		SPCP(name)->_load();
	}

	// Constructor
	brg_cache_nd()
	{
		if(!SPCP(name)->_initialised_) SPP(name)->_init();
	}

	// Deconstructor
	~brg_cache_nd()
	{
	}

#endif // Public methods

}; // class brg_cache

} // namespace IceBRG

// Undef macros
#undef SPP
#undef SPCP

#endif // __BRG_CACHE_ND_HPP_INCLUDED__
