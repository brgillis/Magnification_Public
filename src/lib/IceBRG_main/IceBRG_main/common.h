/*
 * common.h
 *
 *  Created on: 17 Apr 2015
 *      Author: brg
 */

#ifndef BRG_LENSING_COMMON_H_
#define BRG_LENSING_COMMON_H_

#include <complex>
#include <vector>

typedef double flt_type;
typedef long double long_flt_type;

typedef int int_type;
typedef long int long_int_type;
typedef short int short_int_type;

typedef unsigned int unsigned_int_type;
typedef long unsigned int long_unsigned_int_type;
typedef short unsigned int short_unsigned_int_type;

typedef std::complex<flt_type> complex_type;
typedef std::complex<long_flt_type> long_complex_type;

typedef std::vector<int_type> int_vector_type;
typedef std::vector<long_int_type> long_int_vector_type;
typedef std::vector<short_int_type> short_int_vector_type;

typedef std::vector<unsigned_int_type> unsigned_int_vector_type;
typedef std::vector<long_unsigned_int_type> long_unsigned_int_vector_type;
typedef std::vector<short_unsigned_int_type> short_unsigned_int_vector_type;

typedef std::vector<flt_type> flt_vector_type;
typedef std::vector<long_flt_type> long_flt_vector_type;

typedef std::vector<complex_type> complex_vector_type;
typedef std::vector<long_complex_type> long_complex_vector_type;

// Typedef for the signed version of size_t
typedef std::make_signed<size_t>::type ssize_t;
typedef ssize_t size_type;

// Global compiler directives
// Alter these by switching between #define and #undef
#if(1)

//#define _BRG_WARN_FOR_SAFE_FUNCTIONS_TRIGGERED_ // Warns if a function like "safe_d" prevents an error
// This may be expected or not an issue in some cases though,
// so just undef this for release builds if you're satisfied
// there's no actual problem.

#if(__cplusplus==201103L)
#define _BRG_USE_CPP_11_STD_
#else
#undef _BRG_USE_CPP_11_STD_
#endif

// Magic values
#if(1)

#ifndef MAX_STACK_DEPTH
#define MAX_STACK_DEPTH 100
#endif

#ifndef _BRG_PI_DEFINED_
#define _BRG_PI_DEFINED_
// Defining pi to keep it short, but as a variable so it won't act unusually
constexpr flt_type pi = M_PI;
#endif

#ifndef MIN_DIVISOR
#define MIN_DIVISOR 1e-99
#endif

#ifndef SMALL_FACTOR
#define SMALL_FACTOR 1e-9
#endif

#ifndef FLT_ROUNDING_EPSILON
#define FLT_ROUNDING_EPSILON 10*FLT_EPSILON
#endif

#ifndef ROUNDING_EPSILON
#define ROUNDING_EPSILON 10*DBL_EPSILON
#endif

#ifndef DBL_MAX_PRECISION
#define DBL_MAX_PRECISION 14
#endif

#endif // Magic values

// Conditional defines
#if(1)

#ifdef _BRG_USE_CPP_11_STD_
#define BRG_UNIQUE_PTR std::unique_ptr
#define BRG_SHARED_PTR std::shared_ptr
#else
#define BRG_UNIQUE_PTR std::auto_ptr
#define BRG_SHARED_PTR boost::shared_ptr
#endif // #ifdef _BRG_USE_CPP_11_STD_

#ifndef NULL
#ifdef _BRG_USE_CPP_11_STD_
#define NULL nullptr
#else
#define NULL (void *)0
#endif // _BRG_USE_CPP_11_STD_
#endif // #ifndef NULL

#endif // Conditional defines

#endif

#endif /* BRG_LENSING_COMMON_H_ */
