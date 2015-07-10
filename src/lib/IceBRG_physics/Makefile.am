AM_CPPFLAGS = @MY_CPPFLAGS@
AM_CXXFLAGS = @MY_LIBCXXFLAGS@

AM_LDFLAGS = @MY_LDFLAGS@
AM_LIBFLAGS = @MY_LDFLAGS@

if mypj_install_icebrg
lib_LTLIBRARIES = libIceBRG_physics.la
myincludedir = $(pkgincludedir)/IceBRG_physics
nobase_myinclude_HEADERS = astro_caches.h correlation_function_estimator.h phase.hpp astro.h lensing_correlation_function_estimator.h density_profile/density_profile_functors.h density_profile/density_profile.h density_profile/point_mass_profile.h density_profile/tNFW_profile_functors.hpp density_profile/tNFW_profile.h sky_obj/galaxy_group.h sky_obj/galaxy.h sky_obj/position_grid_cache.h sky_obj/position_grid_extension.hpp sky_obj/sky_obj.h sky_obj_with_density_profile/tNFW_galaxy.h
else
noinst_LTLIBRARIES = libIceBRG_physics.la
noinst_HEADERS = astro_caches.h correlation_function_estimator.h phase.hpp astro.h lensing_correlation_function_estimator.h density_profile/density_profile_functors.h density_profile/density_profile.h density_profile/point_mass_profile.h density_profile/tNFW_profile_functors.hpp density_profile/tNFW_profile.h sky_obj/galaxy_group.h sky_obj/galaxy.h sky_obj/position_grid_cache.h sky_obj/position_grid_extension.hpp sky_obj/sky_obj.h sky_obj_with_density_profile/tNFW_galaxy.h
endif
libIceBRG_physics_la_SOURCES = astro_caches.cpp lensing_correlation_function_estimator.cpp astro.cpp correlation_function_estimator.cpp density_profile/density_profile.cpp density_profile/density_profile_functors.cpp density_profile/point_mass_profile.cpp density_profile/tNFW_profile.cpp sky_obj/galaxy.cpp sky_obj/galaxy_group.cpp sky_obj/position_grid_cache.cpp sky_obj/sky_obj.cpp sky_obj_with_density_profile/tNFW_galaxy.cpp
libIceBRG_physics_la_LIBADD = @MY_LDADD@ -lIceBRG_main
