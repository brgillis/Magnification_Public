#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([CFHTLenS_Magnification], [0.9.0], [brg@roe.ac.uk])
AC_CONFIG_SRCDIR([src/exec/CFHTLenS_catalogue_filtering/correct_redshift_bias.h])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([subdir-objects])

AC_LANG(C++)
AM_PATH_PYTHON([2.6])

AC_PROG_RANLIB

# Ask the user if they want to disable parallelization
AC_ARG_ENABLE([parallelization],
    AS_HELP_STRING([--disable-parallelization], [Disable OpenMP parallelization.]))

AS_IF([test "x$enable_parallelization" != "xno"], [parallelization_flags="-fopenmp -DEIGEN_DONT_PARALLELIZE"],[parallelization_flags=""])

# Ask the user if they want to install the IceBRG libraries
AC_ARG_ENABLE([install_IceBRG],
    AS_HELP_STRING([--disable-install_IceBRG], [Don't install the IceBRG static libraries. They will be only used for compilation of the executables here.]))

AS_IF([test "x$enable_install_IceBRG" != "xno"], [install_icebrg="true"],[install_icebrg="false"])
AM_CONDITIONAL([mypj_install_icebrg], [test x$install_icebrg = xtrue])

# Ask the user if they want to fully optimize the install
AC_ARG_ENABLE([full-optimization],
    AS_HELP_STRING([--enable-full-optimization], [Uses a recommended set of optimization flags for compiling the programs and libraries. This overrides any CXXFLAGS option passed by the user.]))

AS_IF([test "x$enable_full_optimization" != "xyes"], [],[AC_SUBST([CXXFLAGS], ["-O3 -funroll-loops -march=native -mfpmath=sse"])])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP 
AC_PROG_AWK

AC_CHECK_PROG(sky2xy_found,sky2xy,"yes","no")
AC_CHECK_PROG(xy2sky_found,xy2sky,"yes","no")
AC_CHECK_PROGS(doxygen_found,doxygen,"yes","no")

AM_CONDITIONAL([HAVE_DOXYGEN], [test "x$doxygen_found" == "xyes"])
AM_COND_IF([HAVE_DOXYGEN], [AC_CONFIG_FILES([docs/Doxyfile])])

# Checks for libraries.

AX_BOOST_BASE
AX_BOOST_SERIALIZATION
AX_CCFITS

# Checks for header files.
AC_CHECK_HEADERS([stddef.h unistd.h],[mypj_found_std_headers=yes])
AC_CHECK_HEADER_STDBOOL
AC_CHECK_HEADERS([Eigen/Core],[mypj_found_eigen_headers=yes])
AC_CHECK_HEADERS([CCfits/CCfits.h],[mypj_found_ccfits_headers=yes])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_CHECK_TYPES([ptrdiff_t])

# Checks for library functions.
AC_FUNC_FORK
AC_CHECK_FUNCS([floor modf pow sqrt])

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 src/exec/CFHTLenS_catalogue_fetching/Makefile
                 src/exec/CFHTLenS_catalogue_filtering/Makefile
                 src/exec/CFHTLenS_corr_func/Makefile
                 src/exec/CFHTLenS_field_stats/Makefile
                 src/exec/CFHTLenS_fit_lensing_models/Makefile
                 src/exec/CFHTLenS_get_unmasked_fractions/Makefile
                 src/exec/CFHTLenS_gg_lensing/Makefile
                 src/exec/CFHTLenS_make_mock_cats/Makefile
                 src/exec/CFHTLenS_source_count_fitting/Makefile
                 src/exec/CFHTLenS_source_counting/Makefile
                 src/exec/Makefile
                 src/lib/IceBRG_lensing/Makefile
                 src/lib/IceBRG_main/Makefile
                 src/lib/IceBRG_physics/Makefile
                 src/lib/Makefile])



# Setup flags

MY_CPPFLAGS="-std=c++0x $parallelization_flags -DNDEBUG -I\$(top_srcdir)/src/lib -I\$(top_srcdir)/src/lib/CFHTLenS_include $BOOST_CPPFLAGS $CCFITS_CPPFLAGS"
AC_SUBST(MY_CPPFLAGS)

MY_LDFLAGS="-L\$(top_builddir)/src/lib/IceBRG_lensing -L\$(top_builddir)/src/lib/IceBRG_physics -L\$(top_builddir)/src/lib/IceBRG_main -lIceBRG_lensing -lIceBRG_physics -lIceBRG_main -lboost_serialization -lCCfits $parallelization_flags $BOOST_LDFLAGS $CCFITS_LDFLAGS"
AC_SUBST(MY_LDFLAGS)

# Print all warnings and errors at the end

AS_IF([test "x$sky2xy_found" == "xyes" -a "x$xy2sky_found" == "xyes"], [],
	[AC_MSG_WARN([Unable to find sky2xy and/or xy2sky. This program can compile, but certain executables
		which call these programs will fail until these are installed. They can be installed as part of the wcstools package.])])

AS_IF([test "x$doxygen_found" == "xyes"], [],
	[AC_MSG_WARN([Unable to find Doxygen. This program can compile, but documentation will not be generated.])])

AS_IF([test "x$mypj_found_std_headers" != "xyes"],
	[AC_MSG_WARN([Unable to find the standard definitions headers.])])

AS_IF([test "x$mypj_found_eigen_headers" != "xyes"],
	[AC_MSG_ERROR([Unable to find Eigen headers. If you know Eigen is installed, make sure its headers
		are in the proper include path.])])
AS_IF([test "x$mypj_found_ccfits_headers" != "xyes"],
	[	AC_MSG_WARN([Unable to find CCfits headers. If you know CCfits is installed, make sure its
		headers are in the proper include path. Compilation for all executables except
		get_unmasked_fractions can still proceed.])
		mypj_compile_get_unmasked_fractions=""],
	[mypj_compile_get_unmasked_fractions="CFHTLenS_get_unmasked_fractions"])
AC_SUBST(mypj_compile_get_unmasked_fractions)

AC_OUTPUT
