The IceBRG_physics library contains functions and classes related to generic astrophysics problems and
concepts. The IceBRG_lensing library contains those aspects which are more focused on gravitational
lensing.

This section will go over various functions and classes available in the library, sorted by the files and
folders they're contained within.

## Miscellaneous

The header files discussed here reside in the root folder, "IceBRG_physics". They can be included by,
for instance "#include <IceBRG_physics/astro.h>".

- astro.h - Generic astrophysics functions, including conversions between redshift, distance measures,
and cosmic time, and simply calculations of distance on the sky.
- astro_caches.h and astro_caches.cpp - These provide a good example of how to use the cache base classes
to cache the result of a function which is expensive to evaluate.
- correlation_function_estimator.h - A class used to calculate correlation functions
- lensing_correlation_function_estimator.h - Used to calculate correlation functions between lenses and
sources in the background from them. (This may get moved to the IceBRG_lensing library at some point
in the future.)

## Folders

### density_profile

This folder contains a definition of a base class for a spherically-symmetric density profile, and the
definitions of some derived classes. The class allows easy calculation of various aspect of the profile
from a minimal definition. Notable headers are:

- density_profile.h - The base class
- point_mass_profile.h - An example profile for a point mass
- tNFW_profile.h - A truncated Navarro-Frenk-White (NFW) profile

### sky_obj

This folder contains the definition of the "sky_obj" base class, representing anything with a position on
the sky, and a few derived classes. Notable headers are:

- sky_obj.h - The base class
- galaxy.h - A class representing a galaxy, with data stored on various aspects of it

The other classes defined in this folder have not been thoroughly tested, and should be used with caution.

### sky_obj_with_density_profile

This folder contains objects with both a position on the sky and a density profile.

- tNFW_galaxy.h - A galaxy with a tNFW mass profile