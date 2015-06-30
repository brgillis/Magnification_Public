/**********************************************************************\
 @file transform.hpp
 ------------------

 TODO <Insert file description here>

 **********************************************************************

 Copyright (C) 2015  Bryan R. Gillis

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

#ifndef BRG_MATH_FOURIER_TRANSFORM_HPP_
#define BRG_MATH_FOURIER_TRANSFORM_HPP_

#include <type_traits>
#include <utility>

#include <boost/optional.hpp>
#include <fftw3.h>

#include "IceBRG_main/Eigen.hpp"
#include "IceBRG_main/container/coerce.hpp"
#include "IceBRG_main/container/is_eigen_container.hpp"
#include "IceBRG_main/math/misc_math.hpp"
#include "IceBRG_main/utility.hpp"

#include "management.hpp"

namespace IceBRG {

namespace Fourier {

// Fourier transform of discrete values
#if(1)
template< typename array_type,
typename std::enable_if<IceBRG::is_container<array_type>::value,char>::type = 0 >
complex_array_type Fourier_transform(const array_type & vals,
		const boost::optional<fftw_wisdom_accumulator &> & wisdom = boost::none )
{
	int_type N = ssize(vals);
	fftw_complex_ptr out((fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N));

	managed_fftw_plan plan;

	// Check if we have any wisdom. If not, estimate. If so, measure.
	if(!wisdom)
	{
		// For the estimating case, it's safe to use the vals array as input - it won't be overwritten.
		// We do have to use a const_cast so it can go into the function though
		#pragma omp critical(make_fftw_plan)
		plan = fftw_plan_dft_r2c_1d(N, const_cast<flt_type *>(vals.data()), out.get(), FFTW_ESTIMATE);

		plan.execute();
	}
	else
	{
		fftw_flt_ptr in((flt_type*) fftw_malloc(sizeof(flt_type) * N));

		#pragma omp critical(make_fftw_plan)
		plan = fftw_plan_dft_r2c_1d(N, in.get(), out.get(), FFTW_MEASURE);

		for(int_type i=0; i<N; ++i) in.get()[i] = vals[i]; // Need to initialize after planning in this case

		plan.execute(); // Execute before in is allocated
	}

	complex_array_type result(N);

	for(int_type i=0; i<N; ++i)
	{
		result(i) = complex_type(out.get()[i][0],out.get()[i][1]);
	}

	return result;
}

#endif // Fourier transform of discrete values

// Inverse Fourier transform of discrete values
#if(1)
template< typename array_type,
typename std::enable_if<IceBRG::is_container<array_type>::value,char>::type = 0 >
flt_array_type inverse_Fourier_transform(const array_type & vals,
		const boost::optional<fftw_wisdom_accumulator &> & wisdom = boost::none )
{
	int_type N = ssize(vals);

	fftw_complex_ptr in((fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N));
	fftw_flt_ptr out((flt_type*) fftw_malloc(sizeof(flt_type) * N));

	managed_fftw_plan plan;

	// Check if we have any wisdom. If not, estimate. If so, measure.
	if(!wisdom)
	{
		#pragma omp critical(make_fftw_plan)
		plan = fftw_plan_dft_c2r_1d(N, in.get(), out.get(), FFTW_ESTIMATE);
	}
	else
	{
		#pragma omp critical(make_fftw_plan)
		plan = fftw_plan_dft_c2r_1d(N, in.get(), out.get(), FFTW_MEASURE);
	}

	// Initialize after planning
	for(int_type i=0; i<N; ++i)
	{
		in.get()[i][0] = vals[i].real();
		in.get()[i][1] = vals[i].imag();
	}

	plan.execute();

	flt_array_type result = flt_array_map_type(out.get(),N)/N;

	return result;
}

#endif // Inverse Fourier transform of discrete values

// Fourier sin transform of discrete values
#if(1)
template< typename array_type,
typename std::enable_if<IceBRG::is_container<array_type>::value,char>::type = 0 >
flt_array_type Fourier_sin_transform(const array_type & vals,
		const boost::optional<fftw_wisdom_accumulator &> & wisdom = boost::none )
{
	int_type N = ssize(vals);
	fftw_flt_ptr out((flt_type*) fftw_malloc(sizeof(flt_type) * N));

	managed_fftw_plan plan;

	// Check if we have any wisdom. If not, estimate. If so, measure.
	if(!wisdom)
	{
		// For the estimating case, it's safe to use the vals array as input - it won't be overwritten.
		// We do have to use a const_cast so it can go into the function though
		#pragma omp critical(make_fftw_plan)
		plan = fftw_plan_r2r_1d(N, const_cast<flt_type *>(vals.data()), out.get(),
				FFTW_RODFT10, FFTW_ESTIMATE);

		plan.execute();
	}
	else
	{
		fftw_flt_ptr in((flt_type*) fftw_malloc(sizeof(flt_type) * N));

		#pragma omp critical(make_fftw_plan)
		plan = fftw_plan_r2r_1d(N, in.get(), out.get(), FFTW_RODFT10, FFTW_MEASURE);

		for(int_type i=0; i<N; ++i) in.get()[i] = vals[i]; // Need to initialize after planning in this case

		plan.execute(); // Execute before in is deallocated
	}

	flt_array_type result = flt_array_map_type(out.get(),N);

	return result;
}

#endif // Fourier sin transform of discrete values

// Inverse Fourier sin transform of discrete values
#if(1)

template< typename array_type,
typename std::enable_if<IceBRG::is_container<array_type>::value,char>::type = 0 >
flt_array_type inverse_Fourier_sin_transform(const array_type & vals,
		const boost::optional<fftw_wisdom_accumulator &> & wisdom = boost::none )
{
	int_type N = ssize(vals);

	fftw_flt_ptr out((flt_type*) fftw_malloc(sizeof(flt_type) * N));

	managed_fftw_plan plan;

	// Check if we have any wisdom. If not, estimate. If so, measure.
	if(!wisdom)
	{
		// For the estimating case, it's safe to use the vals array as input - it won't be overwritten.
		// We do have to use a const_cast so it can go into the function though
		#pragma omp critical(make_fftw_plan)
		plan = fftw_plan_r2r_1d(N, const_cast<flt_type *>(vals.data()), out.get(),
				FFTW_RODFT01, FFTW_ESTIMATE);

		plan.execute();
	}
	else
	{
		fftw_flt_ptr in((flt_type*) fftw_malloc(sizeof(flt_type) * N));

		#pragma omp critical(make_fftw_plan)
		plan = fftw_plan_r2r_1d(N, in.get(), out.get(), FFTW_RODFT01, FFTW_MEASURE);

		for(int_type i=0; i<N; ++i) in.get()[i] = vals[i]; // Need to initialize after planning in this case

		plan.execute(); // Execute before in is deallocated
	}

	flt_array_type result = flt_array_map_type(out.get(),N)/(2*N);

	return result;
}

#endif // Inverse Fourier sin transform of discrete values

// spherical Fourier transform of discrete values
#if(1)
template< typename array_type,
typename std::enable_if<IceBRG::is_container<array_type>::value,char>::type = 0 >
flt_array_type spherical_Fourier_transform(array_type vals,
		const boost::optional<fftw_wisdom_accumulator &> & wisdom = boost::none )
{
	int_type N = ssize(vals);
	fftw_flt_ptr out((flt_type*) fftw_malloc(sizeof(flt_type) * N));

	vals *= flt_array_type::LinSpaced(N,0.5,N-0.5);

	managed_fftw_plan plan;

	// Check if we have any wisdom. If not, estimate. If so, measure.
	if(!wisdom)
	{
		// For the estimating case, it's safe to use the vals array as input - it won't be overwritten.
		// We do have to use a const_cast so it can go into the function though
		#pragma omp critical(make_fftw_plan)
		plan = fftw_plan_r2r_1d(N, const_cast<flt_type *>(vals.data()), out.get(),
				FFTW_RODFT10, FFTW_ESTIMATE);

		plan.execute();
	}
	else
	{
		fftw_flt_ptr in((flt_type*) fftw_malloc(sizeof(flt_type) * N));

		#pragma omp critical(make_fftw_plan)
		plan = fftw_plan_r2r_1d(N, in.get(), out.get(), FFTW_RODFT10, FFTW_MEASURE);

		for(int_type i=0; i<N; ++i) in.get()[i] = vals[i]; // Need to initialize after planning in this case

		plan.execute(); // Execute before in is deallocated
	}

	flt_array_type result = flt_array_map_type(out.get(),N)/flt_array_type::LinSpaced(N,1,N)/square(N);

	return result;
}

#endif // spherical Fourier transform of discrete values

// Inverse spherical Fourier transform of discrete values
#if(1)
template< typename array_type,
typename std::enable_if<IceBRG::is_container<array_type>::value,char>::type = 0 >
flt_array_type inverse_spherical_Fourier_transform(array_type vals,
		const boost::optional<fftw_wisdom_accumulator &> & wisdom = boost::none )
{
	int_type N = ssize(vals);

	fftw_flt_ptr in((flt_type*) fftw_malloc(sizeof(flt_type) * N));
	fftw_flt_ptr out((flt_type*) fftw_malloc(sizeof(flt_type) * N));

	vals *= flt_array_type::LinSpaced(N,1,N);

	managed_fftw_plan plan;

	// Check if we have any wisdom. If not, estimate. If so, measure.
	if(!wisdom)
	{
		// For the estimating case, it's safe to use the vals array as input - it won't be overwritten.
		// We do have to use a const_cast so it can go into the function though
		#pragma omp critical(make_fftw_plan)
		plan = fftw_plan_r2r_1d(N, const_cast<flt_type *>(vals.data()), out.get(),
				FFTW_RODFT01, FFTW_ESTIMATE);

		plan.execute();
	}
	else
	{
		fftw_flt_ptr in((flt_type*) fftw_malloc(sizeof(flt_type) * N));

		#pragma omp critical(make_fftw_plan)
		plan = fftw_plan_r2r_1d(N, in.get(), out.get(), FFTW_RODFT01, FFTW_MEASURE);

		for(int_type i=0; i<N; ++i) in.get()[i] = vals[i]; // Need to initialize after planning in this case

		plan.execute(); // Execute before in is deallocated
	}

	flt_array_type result = N*flt_array_map_type(out.get(),N)/(2*flt_array_type::LinSpaced(N,0.5,N-0.5));

	return result;
}

#endif // Inverse spherical Fourier transform of discrete values

// Fourier transform of function
#if(1)

template< typename func_type,
typename std::enable_if<!IceBRG::is_container<func_type>::value,char>::type = 0 >
complex_array_type Fourier_transform( const func_type & func,
		const flt_type & min = 0.,
		const flt_type & max = 1.,
		const int_type & samples = 1024,
		const boost::optional<fftw_wisdom_accumulator &> & wisdom = boost::none)
{
	flt_array_type vals = flt_array_type::LinSpaced(samples,min,max).unaryExpr(func);
	return Fourier_transform(std::move(vals),wisdom);
}

template< typename func_type,
typename std::enable_if<!IceBRG::is_container<func_type>::value,char>::type = 0 >
flt_array_type Fourier_sin_transform( const func_type & func,
		const flt_type & max = 1.,
		const int_type & samples = 1024,
		const boost::optional<fftw_wisdom_accumulator &> & wisdom = boost::none)
{
	flt_array_type xvals = flt_array_type::LinSpaced(samples,0.,max) + max/(2*(samples-1));
	flt_array_type vals = xvals.unaryExpr(func);

	return Fourier_sin_transform(std::move(vals),wisdom);
}

template< typename func_type,
typename std::enable_if<!IceBRG::is_container<func_type>::value,char>::type = 0 >
flt_array_type spherical_Fourier_transform( const func_type & func,
		const flt_type & max = 1.,
		const int_type & samples = 1024,
		const boost::optional<fftw_wisdom_accumulator &> & wisdom = boost::none)
{
	flt_array_type xvals = flt_array_type::LinSpaced(samples,0.,max) + max/(2*(samples-1));
	flt_array_type vals = xvals * xvals.unaryExpr(func);

	flt_array_type result = (samples-1)/max * Fourier_sin_transform(std::move(vals),wisdom) /
			flt_array_type::LinSpaced(samples,1,samples);

	return result;
}

#endif

} // namespace Fourier

// Pull out all defined methods from the Fourier namespace into the brgastro namespace
using Fourier::Fourier_transform;
using Fourier::inverse_Fourier_transform;
using Fourier::Fourier_sin_transform;
using Fourier::inverse_Fourier_sin_transform;
using Fourier::spherical_Fourier_transform;
using Fourier::inverse_spherical_Fourier_transform;

} // namespace IceBRG

#endif /* BRG_MATH_FOURIER_TRANSFORM_HPP_ */
