The IceBRG_lensing library contains functions and classes related to gravitational lensing.

This section will go over various functions and classes available in the library, sorted by the files and
folders they're contained within.

## Miscellaneous

The header files discussed here reside in the root folder, "IceBRG_lensing". They can be included by,
for instance "#include <IceBRG_lensing/pair_binner.h>".

- source_obj.hpp - A base class for an object on the sky which has a measured shape
- source_galaxy.h - A galaxy which also stores data on its shape
- lens_source_pair.h - A class representing a pair of a lens object and a source object
- pair_binner.h - A class which sorts lens-source pairs into bins and measures the lensing signal

- lensing_profile_extension.hpp - An extension for density profiles which adds in methods to determine
the expected shear and magnification of background sources
- lensing_tNFW_profile.h - A tNFW profile extended with lensing methods

- two_halo_term_functions.hpp and two_halo_term_methods.hpp - Work in progress for calculations of the
"two halo term." Will not give sensible results in current state.

## Folders

### magnification

This folder contains functions and classes used for calculations of weak lensing magnification. Notable
headers include:

- mag_global_values.h - The definitions of global values used (eg. the magnitude bin limits)
- mag_correlation_function_estimator.h - My implementation of the optimally-weighted correlation function
estimator for measuring magnification

- count_fitting_results.hpp, Shcechter_like_functor.h, and expected_count_fit_loader.h - Headers which
were used in attempts to model the source counts with an analytic function. Not used in current
implementation.
- mag_calibration_cache.h and mag_calibration_loader.h - Headers which were used to test if calibrating
for numerical errors resulted in any bias reduction in the magnification signal. Not used in current
implementation.

### shifting

This folder contains functions and classes that I used to test whether the effect of translation due to
lensing has a notable effect of the shear signal. This was planned for an earlier stage of this paper,
and may be published in a future paper if it fits in well enough. Headers here aren't expected to be of
general interest.