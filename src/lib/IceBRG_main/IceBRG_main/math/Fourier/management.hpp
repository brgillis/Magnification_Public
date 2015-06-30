/**********************************************************************\
 @file management.hpp
 ------------------

 A header file containing various classes, structures, and typedefs
 used for management of data created for use with fftw.

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
#ifndef BRG_MATH_FOURIER_MANAGEMENT_HPP_
#define BRG_MATH_FOURIER_MANAGEMENT_HPP_

#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

#include <fftw3.h>

#include "IceBRG_main/utility.hpp"

namespace IceBRG {

namespace Fourier {

constexpr const char * default_fftw_wisdom_filename = ".fftw_wisdom";

/**
 * A class which manages an fftw_plan, making sure it is properly deleted when
 * this object goes out of scope. This class is movable but not copyable.
 */
class managed_fftw_plan
{
private:
	fftw_plan _p_;
public:
	// Default constructor
	managed_fftw_plan() : _p_(nullptr) {}

	// Copy constructor
	managed_fftw_plan(managed_fftw_plan && other) : managed_fftw_plan()
	{
		std::swap(_p_,other._p_);
	}
	managed_fftw_plan(const managed_fftw_plan & other) = delete;

	// Construct from fftw_plan
	managed_fftw_plan(fftw_plan && p) : _p_(std::move(p)) {}
	managed_fftw_plan(const fftw_plan & p) : _p_(p) {}

	// Destructor/destroyer
	~managed_fftw_plan() { fftw_destroy_plan(_p_); }
	void destroy()
	{
		fftw_destroy_plan(_p_);
		_p_ = nullptr;
	}

	// Operator=
	managed_fftw_plan & operator=(const managed_fftw_plan &) = delete;
	managed_fftw_plan & operator=(managed_fftw_plan && other)
	{
		std::swap(_p_,other._p_);
		return *this;
	}

	// Execute
	void execute() {return fftw_execute(_p_);}

	// Convert to fftw_plan
	fftw_plan & get_plan() noexcept {return _p_;}
	operator fftw_plan &() noexcept {return _p_;}
}; // class managed_fftw_plan

/**
 * A class for managing fftw_wisdom stored in a file. It reads in from a file on construction
 * and outputs any gained wisdom back to that file on destruction.
 */
class fftw_wisdom_accumulator
{
private:
	std::string _filename_;
public:
	fftw_wisdom_accumulator(std::string && filename)
	: _filename_(std::move(filename))
	{
		// Load wisdom unless we have a null filename
		if(_filename_.size()>0)
		{
			fftw_import_wisdom_from_filename(_filename_.c_str()); // Ignore failure
		}
	}
	fftw_wisdom_accumulator(const std::string & filename)
	: fftw_wisdom_accumulator(std::string(filename))
	{
	}
	fftw_wisdom_accumulator()
	: fftw_wisdom_accumulator(std::string(default_fftw_wisdom_filename))
	{
	}

	// Make sure it's movable but not copyable

	// Copy constructor
	fftw_wisdom_accumulator(fftw_wisdom_accumulator && other)
	{
		std::swap(_filename_,other._filename_);
	}
	fftw_wisdom_accumulator(const fftw_wisdom_accumulator & other) = delete;

	// Operator=
	fftw_wisdom_accumulator & operator=(const fftw_wisdom_accumulator &) = delete;
	fftw_wisdom_accumulator & operator=(fftw_wisdom_accumulator && other)
	{
		std::swap(_filename_,other._filename_);
		return *this;
	}

	void load(const std::string & filename)
	{
		short res;

		#pragma omp critical(load_or_save_fftw_wisdom)
		res = fftw_import_wisdom_from_filename(filename.c_str());

		// Throw an exception if we couldn't load
		if(!res)
		{
			throw std::runtime_error("Could not load wisdom from " + filename + ".");
		}
	}

	void save(const std::string & filename)
	{
		short res;

		#pragma omp critical(load_or_save_fftw_wisdom)
		res = fftw_export_wisdom_to_filename(filename.c_str());

		// Throw an exception if we couldn't load
		if(!res)
		{
			throw std::runtime_error("Could not save wisdom to " + filename + ".");
		}
	}

	~fftw_wisdom_accumulator()
	{
		// When this goes out of scope, save it unless it has a null filename
		if(_filename_.size()>0)
		{
			#pragma omp critical(load_or_save_fftw_wisdom)
			fftw_export_wisdom_to_filename(_filename_.c_str()); // Ignore failure
		}
	}
}; // class fftw_wisdom_accumulator

/**
 * A deleter for an array created by fftw_malloc, so we can put these arrays into managed
 * pointers.
 */
template< typename T >
struct fftw_array_deleter
{
	void operator()(T* p)
	{
		fftw_free(p);
	}
};

// Typedefs

#if(1)

typedef Eigen::Map<flt_array_type> flt_array_map_type;
typedef Eigen::Map<complex_array_type> complex_array_map_type;

typedef std::unique_ptr<flt_type,fftw_array_deleter<flt_type>> fftw_flt_ptr;
typedef std::unique_ptr<fftw_complex,fftw_array_deleter<fftw_complex>> fftw_complex_ptr;

#endif

} // namespace Fourier

// Pull out the wisdom accumulator, which the user will likely want to use,
// into the brgastro namespace
using Fourier::fftw_wisdom_accumulator;

} // namespace IceBRG


#endif // BRG_MATH_FOURIER_MANAGEMENT_HPP_
