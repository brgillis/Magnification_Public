/**********************************************************************\
  @file cache.hpp

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

#ifndef _BRG_CACHE_HPP_INCLUDED_
#define _BRG_CACHE_HPP_INCLUDED_

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>
#include <sstream>
#include "IceBRG_main/common.h"

#include "IceBRG_main/error_handling.h"
#include "IceBRG_main/file_access/ascii_table.hpp"
#include "IceBRG_main/file_access/open_file.hpp"
#include "IceBRG_main/file_access/trim_comments.hpp"
#include "IceBRG_main/math/misc_math.hpp"
#include "IceBRG_main/math/safe_math.hpp"

#include "IceBRG_main/units/units.hpp"

// Macro definitions

// SPCP: "Static Polymorphic Const Pointer"
// The constness isn't actually enforced, but this is for the reader's understanding
#define SPCP(name) static_cast<const name*>(this)

// SPP: "Static Polymorphic Pointer"
#define SPP(name) static_cast<name*>(this)

#define DECLARE_BRG_CACHE_STATIC_VARS()		\
	static flt_type _min_1_, _max_1_, _step_1_; \
	static size_t _resolution_1_;      		\
	static std::vector< flt_type > _results_; \
											\
	static std::string _file_name_;         \
	static std::string _header_string_;     \
											\
	static bool _loaded_, _initialised_;    \
	static short_int_type _is_monotonic_;		\
											\
	static int_type _sig_digits_;
#define DEFINE_BRG_CACHE_STATIC_VARS(class_name,init_min,init_max,init_step) \
	flt_type IceBRG::class_name::_min_1_ = init_min;						\
	flt_type IceBRG::class_name::_max_1_ = init_max; 						\
	flt_type IceBRG::class_name::_step_1_ = init_step; 						\
	bool IceBRG::class_name::_loaded_ = false;							\
	bool IceBRG::class_name::_initialised_ = false;						\
	short_int_type IceBRG::class_name::_is_monotonic_ = 0;						\
	int_type IceBRG::class_name::_sig_digits_ = 8;					\
	size_t IceBRG::class_name::_resolution_1_ = 0;						\
	std::string IceBRG::class_name::_file_name_ = "";						\
	std::string IceBRG::class_name::_header_string_ = "";					\
	std::vector<flt_type> IceBRG::class_name::_results_;

namespace IceBRG
{

template<typename name>
class brg_cache
{
private:

	// Private variables
#if (1)

	DECLARE_BRG_CACHE_STATIC_VARS();

#endif // Private variables

	// Private methods
#if (1)
	void _init() const
	{
		// We check for initialisation twice due to the critical section here.
		// It's expensive to enter, and we don't want to do anything inside it more than once,
		// so we check whether we need to both once outside it and once inside it.
		if(SPCP(name)->_initialised_) return;

		#ifdef _OPENMP
		#pragma omp critical(init_brg_cache)
		#endif
		if(!SPCP(name)->_initialised_)
		{
			SPCP(name)->_resolution_1_ = (size_t) max( ( ( SPCP(name)->_max_1_ - SPCP(name)->_min_1_ ) / safe_d(SPCP(name)->_step_1_)) + 1, 2);
			SPCP(name)->_file_name_ = SPCP(name)->_name_base() + "_cache.dat";
			SPCP(name)->_header_string_ = "# " + SPCP(name)->_name_base() + "_cache v1.0";

			SPCP(name)->_initialised_ = true;
		}
	}
	void _load() const
	{
		if ( SPCP(name)->_loaded_ ) return;

		std::ifstream in_file;
		std::string file_data;
		bool need_to_calc = false;
		size_t i;
		int_type loop_counter = 0;

		do
		{
			if ( loop_counter >= 2 )
			{
				throw std::runtime_error("Infinite loop detected trying to load " + SPCP(name)->_file_name_ + " in IceBRG::brg_cache.\n");
			}
			else
			{
				loop_counter++;
			}
			need_to_calc = false;

			try
			{
				open_file_input( in_file, SPCP(name)->_file_name_ );
			}
			catch(const std::exception &e)
			{
				need_to_calc = true;
				SPCP(name)->_calc();
				SPCP(name)->_output();
				SPCP(name)->_unload();
				continue;
			}

			// Check that it has the right header
			getline( in_file, file_data );
			if ( file_data.compare( SPCP(name)->_header_string_ ) )
			{
				need_to_calc = true;
				SPCP(name)->_calc();
				SPCP(name)->_output();
				SPCP(name)->_unload();
				continue;
			}

			// Trim out any other commented lines
			trim_comments_all_at_top( in_file );

			// Load range parameters;
			if ( !( in_file >> SPCP(name)->_min_1_ >> SPCP(name)->_max_1_ >> SPCP(name)->_step_1_ ) )
			{
				need_to_calc = true;
				SPCP(name)->_calc();
				SPCP(name)->_output();
				SPCP(name)->_unload();
				continue;
			}

			// Set up data
			SPCP(name)->_resolution_1_ = (size_t) max( ( ( SPCP(name)->_max_1_ - SPCP(name)->_min_1_ ) / safe_d(SPCP(name)->_step_1_)) + 1, 2);
			SPCP(name)->_results_.resize(SPCP(name)->_resolution_1_ );

			// Read in data

			flt_type temp_data;
			flt_type last_data=0;

			i = 0;
			SPCP(name)->_is_monotonic_ = 0;
			while ( ( !in_file.eof() ) && ( i < SPCP(name)->_resolution_1_ ) )
			{
				in_file >> temp_data;
				SPCP(name)->_results_.at(i) = temp_data;
				if(i==1)
				{
					// First monotonic check, so we don't compare to its past values
					if(temp_data > last_data)
					{
						SPCP(name)->_is_monotonic_ = 1;
					}
					else if(temp_data < last_data)
					{
						SPCP(name)->_is_monotonic_ = -1;
					}
					else
					{
						SPCP(name)->_is_monotonic_ = 0;
					}
				}
				else if(i>1)
				{
					// Check for monotonic increase/decrease
					if(temp_data > last_data)
					{
						if(SPCP(name)->_is_monotonic_ != 1)
							SPCP(name)->_is_monotonic_ = 0;
					}
					else if(temp_data < last_data)
					{
						if(SPCP(name)->_is_monotonic_ != -1)
							SPCP(name)->_is_monotonic_ = 0;
					}
					else
					{
						SPCP(name)->_is_monotonic_ = 0;
					}
				}
				last_data = temp_data;
				i++;
			}

			// Check that it was all read properly
			if ( i < SPCP(name)->_resolution_1_ )
			{
				need_to_calc = true;
				SPCP(name)->_calc();
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
		if ( ( SPCP(name)->_max_1_ <= SPCP(name)->_min_1_ ) || ( SPCP(name)->_step_1_ <= 0 ) )
		{
			throw std::runtime_error("ERROR: Bad range passed to brg_cache::_calc() for " + SPCP(name)->_name_base() + "\n");
		}

		// Print a message that we're generating the cache
		handle_notification("Generating " + SPCP(name)->_file_name_ + ". This may take some time.");

		// Set up data
		SPCP(name)->_resolution_1_ = (size_t) max( ( ( SPCP(name)->_max_1_ - SPCP(name)->_min_1_ ) / safe_d(SPCP(name)->_step_1_)) + 1, 2);
		SPCP(name)->_results_.resize(SPCP(name)->_resolution_1_ );

		// Calculate data
		bool bad_result = false;

		#ifdef _OPENMP
		#pragma omp parallel for schedule(dynamic)
		#endif
		for ( size_t i = 0; i < SPCP(name)->_resolution_1_; i++ )
		{
			flt_type result = 0;
			flt_type x = SPCP(name)->_min_1_ + i*SPCP(name)->_step_1_;
			try
			{
				result = SPCP(name)->_calculate(x);
			}
			catch(const std::exception &e)
			{
				handle_error_message(e.what());
				bad_result = true;
			}
			SPCP(name)->_results_[i] = result;
		}

		if(bad_result) throw std::runtime_error("One or more calculations failed in generating cache " + SPCP(name)->_name_base());
		SPCP(name)->_loaded_ = true;

		// Print a message that we've finished generating the cache
		handle_notification("Finished generating " + SPCP(name)->_file_name_ + "!");
	}
	void _output() const
	{

		std::ofstream out_file;
		std::string file_data;

		if ( !SPCP(name)->_loaded_ )
		{
			SPCP(name)->_calc();
		}

		open_file_output( out_file, SPCP(name)->_file_name_ );

		// Output header
		out_file << SPCP(name)->_header_string_ << "\n#\n";

		// Set number of significant digits
		out_file.precision(SPCP(name)->_sig_digits_);

		// Output range
		out_file << SPCP(name)->_min_1_ << "\t" << SPCP(name)->_max_1_ << "\t" << SPCP(name)->_step_1_ << "\n";

		// Output data
		for ( size_t i = 0; i < SPCP(name)->_resolution_1_; i++ )
		{
			out_file << SPCP(name)->_results_.at(i) << "\n";
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

	/// Gets the result in the proper units; must be overloaded if units are used.
	any_units_type _units( const flt_type & v ) const
	{
		return v;
	}
	/// Gets the inverse result in the proper units; must be overloaded if units are used.
	any_units_type _inverse_units(const flt_type & v) const
	{
		return v;
	}

#endif // _BRG_USE_UNITS_

	/// Long calculation function, which is used to generate the cache; must be overloaded by each
	/// child.
	flt_type _calculate(const flt_type & x) const;

	/// The default name (without extension) for the cache file; should be unique for each cache.
	std::string _name_base() const;

	/// This function should be overloaded to call each cache of the same dimensionality as
	/// this cache, which this depends upon in calculation. This is necessary in order to avoid critical
	/// sections of the same name being called recursively.
	void _load_cache_dependencies() const
	{
	}

#endif // Protected methods

public:

	// Public methods
#if (1)

	/**
	 * Set the name of the cache file to use.
	 *
	 * @param new_name The name of the cache file to use
	 */
	void set_file_name( const std::string & new_name )
	{
		SPP(name)->_file_name_ = new_name;
		if ( SPCP(name)->_loaded_ )
		{
			SPCP(name)->_unload();
		}
	} // set_file_name()

	/**
	 * Set the range of the independent parameter for wish you want values to be
	 * cached.
	 *
	 * @param new_min The new minimum value
	 * @param new_max The new maximum value
	 * @param new_step The number of points at which to cache the results
	 */
	void set_range( const flt_type & new_min, const flt_type & new_max,
			const flt_type & new_step)
	{
		// First we try to load, so we can see if there are any changes from
		// the existing cache
		if ( !SPCP(name)->_loaded_ )
			SPCP(name)->_load( true );

		// Go through variables, check if any are actually changed. If so, recalculate cache
		if ( ( SPCP(name)->_min_1_ != new_min ) || ( SPCP(name)->_max_1_ != new_max )
				|| ( SPCP(name)->_step_1_ != new_step ) )
		{
			SPP(name)->_min_1_ = new_min;
			SPP(name)->_max_1_ = new_max;
			SPP(name)->_step_1_ = new_step;

			SPCP(name)->_unload();
			SPCP(name)->_calc();
		}
	} // const int_type set_range()

	/**
	 * Set the precision you wish the values stored in the cache to have.
	 *
	 * @param new_precision The desired precision
	 */
	void set_precision( const size_t & new_precision)
	{
		if ( new_precision > 0 )
		{
			SPP(name)->_sig_digits_ = min( new_precision, DBL_MAX_PRECISION );
		}
		else
		{
			throw std::runtime_error("Precision for dfa_cache must be > 0.\n");
		}
	} // const int_type set_precision()

	/**
	 * Print the cached input and output values to an output stream.
	 *
	 * @param out The output stream you wish to print the cached values to.
	 */
	template<typename otype>
	void print( otype & out ) const
	{
		// Load if necessary
		if ( !SPCP(name)->_loaded_ )
		{
			// Do a test get to make sure it's loaded (and take advantage of the critical section there,
			// so we don't get collisions from loading within two different critical sections at once)
			SPCP(name)->get(SPCP(name)->_min_1_);
		}

		// Fill up header
		std::vector< std::string > header(3);
		header[0] = "#";
		header[1] = "x_1";
		header[2] = "y";

		// Fill up data
		std::vector< std::vector<std::string> > data(3);
		std::stringstream ss;
		for(size_t i_1=0; i_1<SPCP(name)->_resolution_1_; ++i_1)
		{
			data[0].push_back("");
			ss.str("");
			ss << SPCP(name)->_min_1_ + i_1*SPCP(name)->_step_1_;
			data[1].push_back(ss.str());
			ss.str("");
			ss << SPCP(name)->_results_[i_1];
			data[2].push_back(ss.str());
		}

		print_table(out,data,header);
	}

	/**
	 * Get the result of the cached function for a given value.
	 *
	 * @param init_x The value for which you desired the cached result.
	 * @return The cached result for the input value.
	 */
	template<typename Tx>
	const any_units_type get( const Tx & init_x ) const
	{
		flt_type x = value_of(init_x);

		flt_type xlo, xhi;
		size_t x_i; // Lower nearby array point
		flt_type result = 0;

		// Load if necessary
		if ( !SPCP(name)->_loaded_ )
		{
			// Load any caches we depend upon before the critical section
			SPCP(name)->_load_cache_dependencies();

			// Critical section here, since we can't load multiple times simultaneously
			#ifdef _OPENMP
			#pragma omp critical(load_brg_cache)
			#endif
			{
				try
				{
					SPCP(name)->_load();
				}
				catch(const std::exception &e)
				{
					handle_error_message(e.what());
					result = -1;
				}
			}
			if ( result == -1 )
			{
				throw std::runtime_error("ERROR: Could neither load " + SPCP(name)->_file_name_ + " nor calculate in brg_cache::get()\n");
			}
		}

		x_i = (size_t)bound(0,
				( ( x - SPCP(name)->_min_1_ ) / SPCP(name)->_step_1_ ),
				SPCP(name)->_resolution_1_ - 2 );

		xlo = SPCP(name)->_min_1_ + SPCP(name)->_step_1_ * x_i;
		xhi = SPCP(name)->_min_1_ + SPCP(name)->_step_1_ * ( x_i + 1 );

		result = ( ( x - xlo ) * SPCP(name)->_results_.at(x_i + 1) + ( xhi - x ) * SPCP(name)->_results_.at(x_i) )
				/ SPCP(name)->_step_1_;

#ifdef _BRG_USE_UNITS_
		return SPCP(name)->_units(result);
#else
		return result;
#endif

	} // get()

	/**
	 * Get the independent parameter "x" from the corresponding dependent parameter "y". This only
	 * works if the function is monotonic increasing or monotonic decreasing.
	 *
	 * @param y The independent parameter of the cached function
	 * @return The corresponding independent parameter of the cached function
	 */
	const any_units_type inverse_get( const flt_type & y ) const
	{
		// Check if it's possible to do an inverse get
		if((SPCP(name)->_is_monotonic_!=1)&&((SPCP(name)->_is_monotonic_!=-1)))
		{
			// Not a monotonic function. Inverse get isn't possible
			std::string err = "ERROR: Attempt to use inverse_get in cache for " + SPCP(name)->_file_name_ + " for function which isn't monotonic.\n";
			throw std::runtime_error(err);
		}


		flt_type xlo, xhi, ylo, yhi;
		flt_type result = 0;

		if ( !SPCP(name)->_loaded_ )
		{
			// Do a test get to make sure it's loaded (and take advantage of the critical section there,
			// so we don't get collisions from loading within two different critical sections at once)
			SPCP(name)->get(SPCP(name)->_min_1_);
		}
		if ( result == -1 )
		{
			std::string err = "ERROR: Could neither load " + SPCP(name)->_file_name_ + " nor calculate in brg_cache::inverse_get()\n";
			throw std::runtime_error(err);
		}

		if(SPCP(name)->_is_monotonic_==1)
		{

			for ( size_t x_i = 0; x_i < SPCP(name)->_resolution_1_ - 1; x_i++ )
			{
				// Loop through till we find the proper y or reach the end
				yhi = SPCP(name)->_results_.at(x_i);
				if ( ( yhi > y ) || (x_i >= SPCP(name)->_resolution_1_ - 2) )
				{
					ylo = SPCP(name)->_results_.at(x_i + 1);

					xlo = SPCP(name)->_min_1_ + SPCP(name)->_step_1_ * x_i;
					xhi = SPCP(name)->_min_1_ + SPCP(name)->_step_1_ * ( x_i + 1 );
					result = xlo + ( xhi - xlo ) * ( y - ylo ) / safe_d( yhi - ylo );
					break;
				}
			}
		} // if(_is_monotonic_==1)
		else
		{

			for ( size_t x_i = 0; x_i < SPCP(name)->_resolution_1_ - 1; x_i++ )
			{
				// Loop through till we find the proper y or reach the end
				ylo = SPCP(name)->_results_.at(x_i);
				if ( ( ylo < y ) || (x_i >= SPCP(name)->_resolution_1_ - 2) )
				{
					yhi = SPCP(name)->_results_.at(x_i + 1);

					xlo = SPCP(name)->_min_1_ + SPCP(name)->_step_1_ * x_i;
					xhi = SPCP(name)->_min_1_ + SPCP(name)->_step_1_ * ( x_i + 1 );
					result = xlo + ( xhi - xlo ) * ( y - ylo ) / safe_d( yhi - ylo );
					break;
				}
			}

		} // _is_monotonic == -1

#ifdef _BRG_USE_UNITS_
		return SPCP(name)->_inverse_units(result);
#else
		return result;
#endif

	}

	/// Load the cache, calculating if necessary
	void load() const
	{
		_load();
	}

	/// Reload the cache, calculating if necessary.
	void reload() const
	{
		_unload();
		_load();
	}

	/// Recalculate function. Call if you want to overwrite a cache when something's changed in the code
	/// (for instance, the _calculate() function has been altered)
	void recalc() const
	{
		SPCP(name)->_unload();
		SPCP(name)->_calc();
		SPCP(name)->_output();
		SPCP(name)->_unload();
		SPCP(name)->_load();
	}

	// Constructor
	brg_cache()
	{
		if(!SPCP(name)->_initialised_) SPP(name)->_init();
	}

	// Deconstructor
	virtual ~brg_cache()
	{
	}

#endif // Public methods

}; // class brg_cache

} // namespace IceBRG

// Undef macros
#undef SPP
#undef SPCP

#endif // __BRG_CACHE_HPP_INCLUDED__
