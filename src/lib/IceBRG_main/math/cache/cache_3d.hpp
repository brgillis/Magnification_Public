/**********************************************************************\
  @file cache_3d.hpp

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

#ifndef _BRG_CACHE_3D_HPP_INCLUDED_
#define _BRG_CACHE_3D_HPP_INCLUDED_

#ifndef BRG_CACHE_ND_NAME_SIZE
#define BRG_CACHE_ND_NAME_SIZE 9 // Needs an end character, so will only actually allow 8 chars
#endif

#include <cstdlib>
#include <exception>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

#include "IceBRG_main/common.h"

#include "IceBRG_main/error_handling.h"
#include "IceBRG_main/file_access/ascii_table.hpp"
#include "IceBRG_main/file_access/open_file.hpp"
#include "IceBRG_main/math/misc_math.hpp"
#include "IceBRG_main/math/safe_math.hpp"
#include "IceBRG_main/units/units.hpp"
#include "IceBRG_main/utility.hpp"

// Macro definitions

// SPCP: "Static Polymorphic Const Pointer"
// The constness isn't actually enforced, but this is for the reader's understanding
#define SPCP(name) static_cast<const name*>(this)

// SPP: "Static Polymorphic Pointer"
#define SPP(name) static_cast<name*>(this)

#define DECLARE_BRG_CACHE_3D_STATIC_VARS()		               \
	static flt_type _min_1_, _max_1_, _step_1_;                  \
	static size_t _resolution_1_;                        \
	static flt_type _min_2_, _max_2_, _step_2_;                  \
	static size_t _resolution_2_;                        \
	static flt_type _min_3_, _max_3_, _step_3_;                  \
	static size_t _resolution_3_;                        \
	static std::vector< std::vector< std::vector< flt_type > > > _results_;     \
											                   \
	static std::string _file_name_;                            \
	static int_type _version_number_;                      \
											                   \
	static bool _loaded_, _initialised_;

#define DEFINE_BRG_CACHE_3D_STATIC_VARS(class_name,init_min_1,init_max_1,init_step_1, \
                                        init_min_2, init_max_2, init_step_2,\
                                        init_min_3, init_max_3, init_step_3)\
	flt_type IceBRG::class_name::_min_1_ = init_min_1;						\
	flt_type IceBRG::class_name::_max_1_ = init_max_1; 						\
	flt_type IceBRG::class_name::_step_1_ = init_step_1;					\
	size_t IceBRG::class_name::_resolution_1_ = 0;					\
	flt_type IceBRG::class_name::_min_2_ = init_min_2;						\
	flt_type IceBRG::class_name::_max_2_ = init_max_2; 						\
	flt_type IceBRG::class_name::_step_2_ = init_step_2;					\
	size_t IceBRG::class_name::_resolution_2_ = 0;					\
	flt_type IceBRG::class_name::_min_3_ = init_min_3;						\
	flt_type IceBRG::class_name::_max_3_ = init_max_3; 						\
	flt_type IceBRG::class_name::_step_3_ = init_step_3;					\
	size_t IceBRG::class_name::_resolution_3_ = 0;					\
	bool IceBRG::class_name::_loaded_ = false;							\
	bool IceBRG::class_name::_initialised_ = false;						\
	std::string IceBRG::class_name::_file_name_ = "";						\
	int_type IceBRG::class_name::_version_number_ = 2;		        \
	std::vector< std::vector< std::vector< flt_type > > > IceBRG::class_name::_results_;

namespace IceBRG
{

template<typename name>
class brg_cache_3d
{
private:

	// Private variables
#if (1)

	DECLARE_BRG_CACHE_3D_STATIC_VARS();

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
		#pragma omp critical(init_brg_cache_3d)
		#endif
		if(!SPCP(name)->_initialised_)
		{
			SPCP(name)->_resolution_1_ = (size_t) max( ( ( SPCP(name)->_max_1_ - SPCP(name)->_min_1_ ) / safe_d(SPCP(name)->_step_1_)) + 1, 2);
			SPCP(name)->_resolution_2_ = (size_t) max( ( ( SPCP(name)->_max_2_ - SPCP(name)->_min_2_ ) / safe_d(SPCP(name)->_step_2_)) + 1, 2);
			SPCP(name)->_resolution_3_ = (size_t) max( ( ( SPCP(name)->_max_3_ - SPCP(name)->_min_3_ ) / safe_d(SPCP(name)->_step_3_)) + 1, 2);
			SPCP(name)->_file_name_ = SPCP(name)->_name_base() + "_cache.bin";
			SPCP(name)->_version_number_ = 2; // This should be changed when there are changes to this code

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
				throw std::runtime_error("Infinite loop detected trying to load " + SPCP(name)->_file_name_ + " in IceBRG::brg_cache_2.\n");
			}
			else
			{
				loop_counter++;
			}
			need_to_calc = false;

			try
			{
				open_bin_file_input( in_file, SPCP(name)->_file_name_ );
			}
			catch(const std::exception &e)
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
			in_file.read((char *)&(SPCP(name)->_min_1_),sizeof(SPCP(name)->_min_1_));
			in_file.read((char *)&(SPCP(name)->_max_1_),sizeof(SPCP(name)->_max_1_));
			in_file.read((char *)&(SPCP(name)->_step_1_),sizeof(SPCP(name)->_step_1_));
			in_file.read((char *)&(SPCP(name)->_min_2_),sizeof(SPCP(name)->_min_2_));
			in_file.read((char *)&(SPCP(name)->_max_2_),sizeof(SPCP(name)->_max_2_));
			in_file.read((char *)&(SPCP(name)->_step_2_),sizeof(SPCP(name)->_step_2_));
			in_file.read((char *)&(SPCP(name)->_min_3_),sizeof(SPCP(name)->_min_3_));
			in_file.read((char *)&(SPCP(name)->_max_3_),sizeof(SPCP(name)->_max_3_));
			in_file.read((char *)&(SPCP(name)->_step_3_),sizeof(SPCP(name)->_step_3_));

			// Set up data
			SPCP(name)->_resolution_1_ = (size_t) max( ( ( SPCP(name)->_max_1_ - SPCP(name)->_min_1_ ) / safe_d(SPCP(name)->_step_1_)) + 1, 2);
			SPCP(name)->_resolution_2_ = (size_t) max( ( ( SPCP(name)->_max_2_ - SPCP(name)->_min_2_ ) / safe_d(SPCP(name)->_step_2_)) + 1, 2);
			SPCP(name)->_resolution_3_ = (size_t) max( ( ( SPCP(name)->_max_3_ - SPCP(name)->_min_3_ ) / safe_d(SPCP(name)->_step_3_)) + 1, 2);
			make_vector_default( SPCP(name)->_results_, SPCP(name)->_resolution_1_, SPCP(name)->_resolution_2_,
					SPCP(name)->_resolution_3_ );

			// Read in data

			// Initialise
			const std::streamsize size = sizeof(SPCP(name)->_results_[0][0][0]); // Store the size for speed
			size_t i_1=0, i_2=0, i_3=0;

			while ( ( !in_file.eof() ) && ( i_3 < SPCP(name)->_resolution_3_ )
					&& (in_file) )
			{
				in_file.read((char *)&(SPCP(name)->_results_[i_1][i_2][i_3]),size);

				++i_1;
				if(i_1==SPCP(name)->_resolution_1_)
				{
					++i_2;
					if(i_2==SPCP(name)->_resolution_2_)
					{
						++i_3;
						i_2=0;
					}
					i_1=0;
				}
			}

			// Check that it was all read properly
			if ( (i_3 != SPCP(name)->_resolution_3_) || (i_2 != 0) || (i_1 != 0) || (!in_file) )
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
		if ( ( SPCP(name)->_max_1_ <= SPCP(name)->_min_1_ ) || ( SPCP(name)->_step_1_ <= 0 ) ||
				 ( SPCP(name)->_max_2_ <= SPCP(name)->_min_2_ ) || ( SPCP(name)->_step_2_ <= 0 ) ||
				 ( SPCP(name)->_max_3_ <= SPCP(name)->_min_3_ ) || ( SPCP(name)->_step_3_ <= 0 ) )
		{
			throw std::runtime_error("ERROR: Bad range passed to brg_cache_3d::_calc() for " + SPCP(name)->_name_base() + "\n");
		}

		// Print a message that we're generating the cache
		handle_notification("Generating " + SPCP(name)->_file_name_ + ". This may take some time.");

		// Set up data
		SPCP(name)->_resolution_1_ = (size_t) max( ( ( SPCP(name)->_max_1_ - SPCP(name)->_min_1_ ) / safe_d(SPCP(name)->_step_1_)) + 1, 2);
		SPCP(name)->_resolution_2_ = (size_t) max( ( ( SPCP(name)->_max_2_ - SPCP(name)->_min_2_ ) / safe_d(SPCP(name)->_step_2_)) + 1, 2);
		SPCP(name)->_resolution_3_ = (size_t) max( ( ( SPCP(name)->_max_3_ - SPCP(name)->_min_3_ ) / safe_d(SPCP(name)->_step_3_)) + 1, 2);
		make_vector_default( SPCP(name)->_results_, SPCP(name)->_resolution_1_, SPCP(name)->_resolution_2_,
				SPCP(name)->_resolution_3_ );

		// Calculate data
		bool bad_result = false;
		#ifdef _OPENMP
		#pragma omp parallel for schedule(dynamic)
		#endif
		for ( size_t i_1 = 0; i_1 < SPCP(name)->_resolution_1_; ++i_1 )
		{
			flt_type x_1 = SPCP(name)->_min_1_ + i_1*SPCP(name)->_step_1_;
			for( size_t i_2 = 0; i_2 < SPCP(name)->_resolution_2_; ++i_2)
			{
				flt_type x_2 = SPCP(name)->_min_2_ + i_2*SPCP(name)->_step_2_;
				for( size_t i_3 = 0; i_3 < SPCP(name)->_resolution_3_; ++i_3)
				{
					flt_type x_3 = SPCP(name)->_min_3_ + i_3*SPCP(name)->_step_3_;
					flt_type result = 0;
					try
					{
						result = SPCP(name)->_calculate(x_1, x_2, x_3);
					}
					catch(const std::exception &e)
					{
						bad_result = true;
					}
					SPCP(name)->_results_[i_1][i_2][i_3] = result;
				}
			}
		}

		if(bad_result) throw std::runtime_error("One or more calculations in generating cache " + SPCP(name)->_file_name_ + " failed.");
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


		open_bin_file_output( out_file, SPCP(name)->_file_name_ );

		// Output name and version

		std::string file_name = SPCP(name)->_name_base();
		int_type file_version = SPCP(name)->_version_number_;

		out_file.write(file_name.c_str(),BRG_CACHE_ND_NAME_SIZE);
		out_file.write((char *)&file_version,sizeof(file_version));

		// Output range parameters
		out_file.write((char *)&(SPCP(name)->_min_1_),sizeof(SPCP(name)->_min_1_));
		out_file.write((char *)&(SPCP(name)->_max_1_),sizeof(SPCP(name)->_max_1_));
		out_file.write((char *)&(SPCP(name)->_step_1_),sizeof(SPCP(name)->_step_1_));
		out_file.write((char *)&(SPCP(name)->_min_2_),sizeof(SPCP(name)->_min_2_));
		out_file.write((char *)&(SPCP(name)->_max_2_),sizeof(SPCP(name)->_max_2_));
		out_file.write((char *)&(SPCP(name)->_step_2_),sizeof(SPCP(name)->_step_2_));
		out_file.write((char *)&(SPCP(name)->_min_3_),sizeof(SPCP(name)->_min_3_));
		out_file.write((char *)&(SPCP(name)->_max_3_),sizeof(SPCP(name)->_max_3_));
		out_file.write((char *)&(SPCP(name)->_step_3_),sizeof(SPCP(name)->_step_3_));

		// Output data

		// Initialize
		const std::streamsize size = sizeof(SPCP(name)->_results_[0][0][0]);
		size_t i_1=0, i_2=0, i_3=0;

		while ( i_3<SPCP(name)->_resolution_3_ )
		{
			out_file.write((char *)&(SPCP(name)->_results_[i_1][i_2][i_3]),size);

			++i_1;
			if(i_1==(SPCP(name)->_resolution_1_))
			{
				++i_2;
				if(i_2==(SPCP(name)->_resolution_2_))
				{
					++i_3;
					i_2 = 0;
				}
				i_1 = 0;
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

	/// Long calculation function, which is used to generate the cache; must be overloaded by each
	/// child.
	flt_type _calculate(const flt_type & x_1, const flt_type & x_2, const flt_type & x_3) const;

	/// The default name (without extension) for the cache file; should be unique for each cache.
	const std::string _name_base() const;

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
	void set_file_name( const std::string new_name )
	{
		if(!SPCP(name)->_initialised_) SPP(name)->_init();
		SPP(name)->_file_name_ = new_name;
		if ( SPCP(name)->_loaded_ )
		{
			SPCP(name)->_unload();
		}
	} // void set_file_name()

	/**
	 * Set the range of the independent parameter for wish you want values to be
	 * cached.
	 *
	 * @param new_min_1 The new minimum value, dimension 1.
	 * @param new_max_1 The new maximum value, dimension 1.
	 * @param new_step_1 The number of points at which to cache the results, dimension 1.
	 * @param new_min_1 The new minimum value, dimension 2.
	 * @param new_max_1 The new maximum value, dimension 2.
	 * @param new_step_1 The number of points at which to cache the results, dimension 2.
	 * @param new_min_1 The new minimum value, dimension 3.
	 * @param new_max_1 The new maximum value, dimension 3.
	 * @param new_step_1 The number of points at which to cache the results, dimension 3.
	 */
	void set_range( const flt_type & new_min_1, const flt_type & new_max_1, const flt_type & new_step_1,
	         const flt_type & new_min_2, const flt_type & new_max_2, const flt_type & new_step_2,
 	         const flt_type & new_min_3, const flt_type & new_max_3, const flt_type & new_step_3)
	{
		if(!SPCP(name)->_initialised_) SPP(name)->_init();

		// First we try to load, so we can see if there are any changes from
		// the existing cache
		if ( !SPCP(name)->_loaded_ )
			SPCP(name)->_load( true );

		// Go through variables, check if any are actually changed. If so, recalculate cache
		if ( ( SPCP(name)->_min_1_ != new_min_1 ) || ( SPCP(name)->_max_1_ != new_max_1 )
				|| ( SPCP(name)->_step_1_ != new_step_1 ) ||
			 ( SPCP(name)->_min_2_ != new_min_2 ) || ( SPCP(name)->_max_2_ != new_max_2 )
				|| ( SPCP(name)->_step_2_ != new_step_2 ) ||
			 ( SPCP(name)->_min_3_ != new_min_3 ) || ( SPCP(name)->_max_3_ != new_max_3 )
				|| ( SPCP(name)->_step_3_ != new_step_3 ))
		{
			SPP(name)->_min_1_ = new_min_1;
			SPP(name)->_max_1_ = new_max_1;
			SPP(name)->_step_1_ = new_step_1;
			SPP(name)->_min_2_ = new_min_2;
			SPP(name)->_max_2_ = new_max_2;
			SPP(name)->_step_2_ = new_step_2;
			SPP(name)->_min_3_ = new_min_3;
			SPP(name)->_max_3_ = new_max_3;
			SPP(name)->_step_3_ = new_step_3;

			SPCP(name)->_unload();
			SPCP(name)->_calc();
		}
	} // void set_range()

	/**
	 * Print the cached input and output values to an output stream.
	 *
	 * @param out The output stream you wish to print the cached values to.
	 */
	template<typename otype>
	void print( otype & out) const
	{

		if(!SPCP(name)->_initialised_) SPCP(name)->_init();

		// Load if necessary
		if ( !SPCP(name)->_loaded_ )
		{
			// Do a test get to make sure it's loaded (and take advantage of the critical section there,
			// so we don't get collisions from loading within two different critical sections at once)
			SPCP(name)->get(SPCP(name)->_min_1_,SPCP(name)->_min_2_,SPCP(name)->_min_3_);
		}

		// Fill up header
		std::vector< std::string > header(5);
		header[0] = "#";
		header[1] = "x_1";
		header[2] = "x_2";
		header[3] = "x_3";
		header[4] = "y";

		// Fill up data
		std::vector< std::vector<std::string> > data(5);
		std::stringstream ss;
		for(size_t i_1=0; i_1<SPCP(name)->_resolution_1_; ++i_1)
		{
			for(size_t i_2=0; i_2<SPCP(name)->_resolution_2_; ++i_2)
			{
				for(size_t i_3=0; i_3<SPCP(name)->_resolution_3_; ++i_3)
				{
					data[0].push_back("");
					ss.str("");
					ss << SPCP(name)->_min_1_ + i_1*SPCP(name)->_step_1_;
					data[1].push_back(ss.str());
					ss.str("");
					ss << SPCP(name)->_min_2_ + i_2*SPCP(name)->_step_2_;
					data[2].push_back(ss.str());
					ss.str("");
					ss << SPCP(name)->_min_3_ + i_3*SPCP(name)->_step_3_;
					data[3].push_back(ss.str());
					ss.str("");
					ss << SPCP(name)->_results_[i_1][i_2][i_3];
					data[4].push_back(ss.str());
				}
			}
		}

		print_table(out,data,header);
	}


	/**
	 * Get the result of the cached function for a given value.
	 *
	 * @param init_x_1 The value for which you desired the cached result, dimension 1.
	 * @param init_x_2 The value for which you desired the cached result, dimension 2.
	 * @param init_x_3 The value for which you desired the cached result, dimension 3.
	 * @return The cached result for the input value.
	 */
	template< typename Tx1, typename Tx2, typename Tx3 >
	any_units_type get( const Tx1 & init_x_1, const Tx2 & init_x_2,
			const Tx3 & init_x_3) const
	{

		flt_type x_1 = value_of(init_x_1);
		flt_type x_2 = value_of(init_x_2);
		flt_type x_3 = value_of(init_x_3);

		flt_type xlo_1, xhi_1;
		size_t xi_1; // Lower nearby array point
		flt_type xlo_2, xhi_2;
		size_t xi_2; // Lower nearby array point
		flt_type xlo_3, xhi_3;
		size_t xi_3; // Lower nearby array point
		flt_type result = 0;
		flt_type total_weight = 0;

		if(!SPCP(name)->_initialised_) SPCP(name)->_init();

		// Load if necessary
		if ( !SPCP(name)->_loaded_ )
		{
			// Load any caches we depend upon before the critical section
			SPCP(name)->_load_cache_dependencies();

			// Critical section here, since we can't load multiple times simultaneously

			#ifdef _OPENMP
			#pragma omp critical(load_brg_cache_3d)
			#endif
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
				throw std::runtime_error("ERROR: Could neither load " + SPCP(name)->_file_name_ + " nor calculate in brg_cache_3d::get()\n");
			}
		}

		xi_1 = (size_t)bound(0,
				( ( x_1 - SPCP(name)->_min_1_ ) / SPCP(name)->_step_1_ ),
				SPCP(name)->_resolution_1_ - 2 );
		xi_2 = (size_t)bound(0,
				( ( x_2 - SPCP(name)->_min_2_ ) / SPCP(name)->_step_2_ ),
				SPCP(name)->_resolution_2_ - 2 );
		xi_3 = (size_t)bound(0,
				( ( x_3 - SPCP(name)->_min_3_ ) / SPCP(name)->_step_3_ ),
				SPCP(name)->_resolution_3_ - 2 );

		xlo_1 = SPCP(name)->_min_1_ + SPCP(name)->_step_1_ * xi_1;
		xhi_1 = SPCP(name)->_min_1_ + SPCP(name)->_step_1_ * ( xi_1 + 1 );
		xlo_2 = SPCP(name)->_min_2_ + SPCP(name)->_step_2_ * xi_2;
		xhi_2 = SPCP(name)->_min_2_ + SPCP(name)->_step_2_ * ( xi_2 + 1 );
		xlo_3 = SPCP(name)->_min_3_ + SPCP(name)->_step_3_ * xi_3;
		xhi_3 = SPCP(name)->_min_3_ + SPCP(name)->_step_3_ * ( xi_3 + 1 );

		total_weight = (xhi_1-xlo_1)*(xhi_2-xlo_2)*(xhi_3-xlo_3);

		result += SPCP(name)->_results_[xi_1][xi_2][xi_3] * (xhi_1-x_1)*(xhi_2-x_2)*(xhi_3-x_3);
		result += SPCP(name)->_results_[xi_1+1][xi_2][xi_3] * (x_1-xlo_1)*(xhi_2-x_2)*(xhi_3-x_3);

		result += SPCP(name)->_results_[xi_1][xi_2+1][xi_3] * (xhi_1-x_1)*(x_2-xlo_2)*(xhi_3-x_3);
		result += SPCP(name)->_results_[xi_1+1][xi_2+1][xi_3] * (x_1-xlo_1)*(x_2-xlo_2)*(xhi_3-x_3);


		result += SPCP(name)->_results_[xi_1][xi_2][xi_3+1] * (xhi_1-x_1)*(xhi_2-x_2)*(x_3-xlo_3);
		result += SPCP(name)->_results_[xi_1+1][xi_2][xi_3+1] * (x_1-xlo_1)*(xhi_2-x_2)*(x_3-xlo_3);

		result += SPCP(name)->_results_[xi_1][xi_2+1][xi_3+1] * (xhi_1-x_1)*(x_2-xlo_2)*(x_3-xlo_3);
		result += SPCP(name)->_results_[xi_1+1][xi_2+1][xi_3+1] * (x_1-xlo_1)*(x_2-xlo_2)*(x_3-xlo_3);

		result /= safe_d(total_weight);

#ifdef _BRG_USE_UNITS_
		return SPCP(name)->_units(result);
#else
		return result;
#endif

	} // get()

	/// Load the cache, calculating if necessary
	void load() const
	{
		SPCP(name)->_load();
	}

	/// Reload the cache, calculating if necessary.
	void reload() const
	{
		SPCP(name)->_unload();
		SPCP(name)->_load();
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
	brg_cache_3d()
	{
	}

	// Deconstructor
	virtual ~brg_cache_3d()
	{
	}

#endif // Public methods

}; // class brg_cache

} // namespace IceBRG

// Undef macros
#undef SPP
#undef SPCP

#endif // __BRG_CACHE_2D_HPP_INCLUDED__
