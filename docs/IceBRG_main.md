The IceBRG_main library contains various C++ functions and classes which will likely be of use to programmers in a variety of
fields. Most of the library is header-only, with all classes and functions inlined, which is intended to allow better optimizations
at compilation time but may slow down compilation.

This section will go over various functions and classes available in the library, sorted by the files and folders they're contained
within.

## Miscellaneous

The header files discussed here reside in the root folder, "IceBRG_main". They can be include by, for instance "#include
<IceBRG_main/common.h>".

- call_program.hpp - This header defines a function "call_program(...)" which handles calling another program at the command-line and passing arguments
to it. See the header for the required syntax.
- command_line.hpp - This header defines various functions for processing command-line arguments, such as getting an argument and
throwing an exception if it isn't present, or testing if an argument satisfies a test function or otherwise using a default value.
- <a href=lib_2IceBRG__main_2common_8h.html><b>common.h</b></a> - This file defines various common typedefs (eg. "flt_type" for double), to ease maintenance when its needed to change one of these. It
also includes a few global defines for numeric values. Notably, it defines "pi" as a constexpr flt_type with the value of M_PI.
Basically, include this if you want to get the typedefs and/or pi.
- Eigen.hpp - This is an include-wrapper for Eigen/Core, which includes it, defines begin and end functions for Eigen matrices and arrays (allowing
their elements to be iterated over instead of requiring a for loop with each element indexed), and defines a few typedefs for arrays.
Include this as an alternative to including Eigen/Core if you want to iterate over Eigen arrays, particularly with the C++11 "for each"
syntax.
- error_handling.h - This header defines functions for handling (non-critical) errors, warnings, and notifications, where the
desired behavior on an error is determined by the value of a static variable. This allows the value of the this variable to be changed
to alter whether errors will result in exceptions, printed warnings, etc.
- join_path.hpp - This header defines a function to join two strings as elements of a file path, ensuring there's just one slash
between them. Note that it does this by replacing all instances of multiple slashes with a single slash in the resulting string, so it
won't work properly on strings that deliberately contain multiple slashes (eg. the "http://" at the beginning of a URL).
- lexical_cast.hpp - This header defines functions to apply a few extensions to Boost's lexical cast, such as functions which
return the minimum possible value for a variable if the value can't be cast, or casting to a boolean (useful for reading user-defined
values).
- utility.hpp - This header defines miscellaneous utility functions used within the library.

## Folders

### container

This folder contains various classes and functions for dealing with containers other than vectors (which have their own folder). Of
particular note are the headers:

- coerce.hpp - Functions to coerce one type into a different type of container when the assignment operator won't
work, by using the assignment operator on the individual elements (or recursively coercing, in the case of structures of structures).
For instance, this can be used to set a vector of doubles equal to a vector of ints.
- insertion_ordered_map.hpp - Works like a std::map, except items are ordered by the order in which
they're added to the map.
- labeled_array.hpp - A container meant to represent a table of values. Each column has its own label, and
it's possible to access a column by its label. It's also possible to easily iterate over rows, columns, or elements of an individual
row or column. Also includes methods for reading in data from an ASCII table format and outputting in this format. The only
significant limitation is that it can only include one type of data - ie. it can't have one column of doubles and another of strings.
- tuple.hpp - Many functions to extend the use of boost::tuples, allowing elementwise-operations to be performed on
them for instance.

### external

This folder contains code developed by others (licensed under the GPL), which I've included in the library:

- tk_spline.h - A fast implementation of a spline. See also my [interpolator](@ref interpolator.h) class,
which allows this and other types of interpolation.
- sgsmooth.h - An implementation of Savitsky-Golay smoothing, useful for handling noisy data which needs to be
differentiated.

### file_access

This folder contains various functions for handling file access and ASCII tables. Most of the functions and classes related to
ASCII tables are subsumed by the functionality of the labeled_array class, which provides a superior interface. These are
still used behind the scenes in order to create a labeled_array, however. Some notable headers here are:

- open_file.hpp - Functions to handle the boilerplate of opening up a file stream.
- table_utility.hpp - Various utility functions for loading in a table.
- trim_comments.hpp - Functions for trimming the comments from files

### math

This folder contains functions for various mathematical operations, sorted into further subfolders.

#### math

The following useful headers are in the root "math" folder:

- ipow.hpp - Defines the function "ipow" in a few different manners, which represents taking an integral
power. In the C++11 standard, compilers can't optimize integral powers with std::pow, so this function can be used instead
when the power is integral. Variations exist for when the power is and isn't known at runtime.
- misc_math.hpp - Miscellaneous useful functions, such as testing if a value is good (or NaN or inf),
squaring/cubing/etc. values, adding/subtracting in quadruture, getting distance between two points, and a few useful
functions for calculating errors under certain operations.
- safe_math.hpp - "Safe" functions, to allow operations such as sqrt or division without worrying
about edge cases (sqrt of a negative number or division by zero) causing too much of an issue. The results won't always
be sensible, but they'll be numerical, so you don't have to worry about a NaN contaminating all future math if it might
be ignored (eg. 0/safe_d(0) will result in 0 rather than NaN, which is often a more useful result for eg. taking a
weighted average).

#### cache

The headers here define abstract base classes which help with caching the result of a function which is expensive to
evaluate. The user will define a derived class from the cache of the desired dimension, define the desired bounds of
input variables to use for the limits of the cache, and define a function to calculate the function. The cache can
then be used to get the value at any point, interpolating between cached values.

The different header files define caches for functions of different dimensionality. When possible, the 1-4d caches
should be used, as the cache for arbitrary dimensionality is significantly slower. Headers are:

- cache.hpp
- cache_2d.hpp
- cache_3d.hpp
- cache_4d.hpp
- cache_nd.hpp

Examples of caches being used can be seen in the astro_caches.h and astro_caches.cpp files.

#### calculus

This folder contains the following calculus-related headers:

- DE.hpp - Differential equation solving methods
- differentiate.hpp - Functions to numerically differentiate a function of one or arbitrary dimensions
- integrate.hpp - Functions to numerically integrate a function of one or arbitrary dimensions, using various different methods

#### functor

The header functor_product.hpp here is useful for easily defining a functor whose result is the product of two other
functions or functors applied to the input value.

#### Fourier

This folder contains headers to easily perform Fourier transforms with the FFTW library, handling the necessary boilerplate:

- radial_vector.hpp - A class for handling transforms of spherically-symmetric functions easily.
- transform.hpp - Functions which handle the necessary boilerplate for Fourier transforms

#### interpolator

This folder contains the header interpolator.h for a class which helps determine the interpolation between known points,
using one of multiple interpolation methods, and the header interpolator_derivative.h for a class which calculates the
(smoothed) derivative of an interpolator.

#### random

This folder contains functions related to random-number generation.

- distributions.hpp - Functions to get aspects of common distributions (eg. pdf/cdf values)
- random_functions.hpp - Functions to generate a random number with given distributions, optionally using a generator object.

#### solvers

Functions to solve or minimize functions, and the ability to get the errors on best-fit values.

- solvers.hpp - Functions to solve or minimize
- solver_classes.hpp - Classes which perform minimize functions and retain data needed to also get errors on solutions

#### statistics

This folder contains class definitions which are used with Boost::accumulators to get additional statistics from them, and
other statistical operations.

Headers for accumulators:

- effective_count.hpp - Using the definition: Square of sum of weights / sum of squares of weights
- error_of_weighted_mean.hpp - Using an old definition; shouldn't be used until I update to a better one
- mean_weight.hpp - Mean of weights
- sum_of_square_weights.hpp - Sum of squares of weights

Other:

- resampling.hpp - Functions to generate a bootstrap or jackknife resampled version of a dataset.

### units

This folder deals with the set-up for dimensional analysis. Notable headers are:

- units.hpp - Defines typedefs for common unit types and base units, plus function overloads which can be used on
quantities and return quantities with the correct units (eg. sqrt, ipow)
- unit_conversions.hpp - Defines conversions to and from the base SI unit set.

### vector

This folder contains headers for operations on vectors and the definitions of some derived classes. Notable headers
include:

- elementwise_functions.hpp - Elementwise operations on vectors and other containers, which work recursively to
allow function on vectors of vectors as well
- limit_vector.hpp - A class representing a set of bin limits. It includes various operations to quickly determine
the bin which a given value falls within.
- manipulations.hpp - Functions to concatenate vectors, transpose and vertically flip vectors of vectors.
- summary_functions.hpp - Functions which provide summary statistics of vectors (eg. the sum of all elements). As
with the elementwise functions, these also work recursively.
