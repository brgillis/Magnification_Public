## Introduction

This package contains the code used for Gillis and Taylor (2015, "GT15"), available at
http://arxiv.org/abs/1507.01858. In broad terms, the code in this package can
be considered in two parts:

1. The IceBRG libraries - C++ libraries I've developed through the course of my work,
which I'm distributing in full (even though some parts aren't used here, and some
aren't complete).
2. Executables and scripts which recreate most of the analysis performed in GT15, all
prepended with "CFHTLenS_Mag_".


### Installation

If you're reading this, chances are you've already built the package. If not, see
the directions in README.md in the root directory.


### General Code Explanations

Here are some of the naming conventions I use:

- C++ source files are all .cpp. Headers are either .h or .hpp. .hpp headers are those
which are self-contained - they include inlined definitions of all functions and methods
defined within.
- Private member variables are surrounded by underscores ("_x_"), and private methods are
prepended with an underscore ("_f()").
- Getter methods are in most cases simply the name of the variable. For instance,
galaxy.z() would return the redshift of the galaxy.

Variable typedefs are defined in "IceBRG_main/common.h" and "IceBRG_main/units/units.hpp".
I use Boost's units library for dimensional analysis in programming, using typedefs to save
space in giving the types of units. This is mostly checked at compile-time, but it can't be
entirely optimized out in my code, so I use a preprocessor macro to disable dimensional
analysis in the release version of the code. This means that in the release version, any value
like "distance_type" is actually just a double.

Since Boost's "quantities" (values with units) aren't directly comparable to unitless values
(even zero), you'll see functions like "value_of(...)" in various places. This is defined
differently depending on whether dimensional analysis is used or not, but in either case it has
the result of returning the unitless value of a variable. The reverse operation, turning a
unitless value into a quantity, is performed with the method "units_cast<T>()".

To simplify units, I use all values in SI kms units throughout my code, except at input and
output. This way I don't have to worry in the translation from debug code (with dimensional
analysis enabled) to release code (with it disabled) that the units might differ. This is done
through the use of conversions to and from this base in the header
"IceBRG_main/units/unit_conversions.hpp", which are defined in the namespace "IceBRG::unitconv".


### IceBRG Libraries

In this section, I'll give an overview of the IceBRG libraries, explain the purpose of
each, and point to some functions and classes which others may find interesting. All
functions, classes, and typedefs in these libraries are declared in the namespace
"IceBRG", except for certain overloads of std:: functions.


#### IceBRG_main Library

[IceBRG_main](md_IceBRG_main.html) contains all useful functions and classes I've written which don't have
anything specifically to do with my astrophysics work. That is, anything that might be
useful to someone completely outside of astrophysics would go in this library.


#### IceBRG_physics Library

[IceBRG_physics](md_IceBRG_physics.html) contains functions and classes related to (astro)physics, but not
specifically to a subfield which has its own library.


#### IceBRG_lensing Library

[IceBRG_lensing](md_IceBRG_lensing.html) contains functions and classes related to gravitational lensing in one
way or another.


### CFHTLenS_Mag executables

The various programs here generate executables for performing most of the necessary steps for
a gravitational lensing analysis as done in GT15. Much of the heavy lifting for these
executables is performed by functions and classes within the IceBRG libraries. The code in
these executables is typically designed to work specifically with the CFHTLenS catalogues and
isn't as portable as the code in the libraries.

When run without command-line arguments, the downloaded and generated data will be stored in a
Data/ subdirectory of the directory from which the executables are run. The first command-line
argument to all executables can be used to specify an alternate directory for data.

The code here will be listed by folder, in the order in which the generated executables should
generally be run in a first pass.


#### CFHTLenS_catalogue_fetching

This folder contains Python scripts to generate a list of the fields we want to process,
download them from the CFHTLenS web server, and download and compress the corresponding
mask files. Two executable python scripts will be installed:

-CFHTLenS_Mag_get_fields.py - This generates the list of fields and downloads them. By default,
this creates a list of only a single field and downloads just that. This is done to lower
the burden on the CFHTLenS server for people who only want to test the code. The second
command-line argument can be set to "True" to instead generate a list of all fields and
download them all.

-CFHTLenS_Mag_get_masks.py - This downloads the mask file for each field in the generated fields
list, then compresses it with fpack (the files are quite large, but smoothly varying, so they
have a very good compression ratio).


#### CFHTLenS_catalogue_filtering

This folder contains the source to build an executable which will filter the catalogues,
separating into lens and source catalogues and removing unnecessary columns. It manually
applies the masks (since the values in the catalogue don't always match with the mask files)
and removes galaxies which don't meed some cuts.

It generates the executable "CFHTLenS_Mag_filter_catalogues".


#### CFHTLenS_get_unmasked_fractions

This folder contains source to build an executable which will determine the unmasked fraction
in annuli surrounding every lens galaxy. At present, it uses a hard-coded set of annuli. I'll
try to update this soon so it can take a configuration file.

It generates the executable "CFHTLenS_Mag_get_unmasked_fractions".


#### CFHTLenS_source_counting

This folder contains source to build an executable which will count the number of source galaxies
in magnitude bins which lie at or beyond a given redshift.

It generates the executable "CFHTLenS_Mag_count_sources".


#### CFHTLenS_gg_lensing

This folder contains source to build an executable which will perform the weak lensing measurement
(including both shear and magnification) for a sample of lens galaxies. It allows use of a config
file to determine how lenses are binned.

It generates the executable "CFHTLenS_measure_weak_lensing".


#### CFHTLenS_fit_lensing_models

This folder contains source to build an executable which will fit a model halo profile to the
measured lensing signals.

It generates the executable "CFHTLenS_fit_lensing_model".


#### CFHTLenS_make_mock_cats

This folder contains source to build an executable which will generate mock lens and source
catalogues, which are needed for a correlation-function analysis. This isn't part of the main
pipeline, so its code hasn't been made as user-friendly yet.

It generates the executable "CFHTLenS_Mag_make_mock_cats".


#### CFHTLenS_corr_func

This folder contains source to build an executable which will measure the correlation functions
between lens and source galaxies, as an alternate means of analysis. This isn't part of the main
pipeline, so its code hasn't been made as user-friendly yet.

It generates the executable "CFHTLenS_Mag_measure_corr_funcs".


#### CFHTLenS_field_stats

This folder contains source to build an executable which will count the number density of lenses
in each field in the CFHTLenS at various redshift slices. This isn't part of the main
pipeline, so its code hasn't been made as user-friendly yet.

It generates the executable "CFHTLenS_Mag_get_field_stats".


#### CFHTLenS_source_count_fitting

This folder contains source to build an executable which will attempt to fit an analytic function
to the source number count distribute, and will report on the quality of the fit. This isn't part
of the main pipeline, so its code hasn't been made as user-friendly yet.

It generates the executable "CFHTLenS_Mag_fit_source_counts".
