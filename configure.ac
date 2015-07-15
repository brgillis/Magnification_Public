#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([CFHTLenS_Magnification], [0.9.4], [brg@roe.ac.uk])
AC_CONFIG_SRCDIR([src/exec/CFHTLenS_catalogue_filtering/correct_redshift_bias.h])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIRS([m4])

LT_INIT

AM_INIT_AUTOMAKE([subdir-objects foreign])

AC_LANG(C++)
AM_PATH_PYTHON([2.6])

# Ask the user if they want to disable parallelization
AC_ARG_ENABLE([parallelization],
    AS_HELP_STRING([--disable-parallelization], [Disable OpenMP parallelization.]))

AS_IF([test "x$enable_parallelization" != "xno"], [parallelization_flags="-fopenmp -DEIGEN_DONT_PARALLELIZE"],[parallelization_flags=""])

# Ask the user if they want to install the IceBRG libraries
AC_ARG_ENABLE([install-IceBRG],
    AS_HELP_STRING([--disable-install-IceBRG], [Don't install the IceBRG static libraries. They will be only used for compilation of the executables here.]))

AS_IF([test "x$enable_install_IceBRG" != "xno"], [install_icebrg="true"],[install_icebrg="false"])
AM_CONDITIONAL([mypj_install_icebrg], [test x$install_icebrg = xtrue])

# Ask the user if they want to fully optimize the install
AC_ARG_ENABLE([full-optimization],
    AS_HELP_STRING([--enable-full-optimization], [Increase the optimization level and remove debugging flags by changing default CXXFLAGS to '-O3'. Any user-defined CXXFLAGS will override this.]))

AS_IF([test "x$enable_full_optimization" != "xyes"],
	[], [: ${CXXFLAGS="-O3"}])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP 
AC_PROG_AWK

AC_CHECK_PROG(sky2xy_found,sky2xy,"yes","no")
AC_CHECK_PROG(xy2sky_found,xy2sky,"yes","no")
AC_CHECK_PROG(fpack_found,fpack,"yes","no")
AC_CHECK_PROG(funpack_found,funpack,"yes","no")
AC_CHECK_PROG(doxygen_found,doxygen,"yes","no")

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
                 src/lib/Makefile
                 docs/Makefile])



# Setup flags

MY_CPPFLAGS="-DNDEBUG -I\$(top_srcdir)/src/lib -I\$(top_srcdir)/src/lib/CFHTLenS_include $BOOST_CPPFLAGS $CCFITS_CPPFLAGS"
AC_SUBST(MY_CPPFLAGS)

MY_CXXFLAGS="-std=c++11 $parallelization_flags -funroll-loops -march=native -mfpmath=sse $BOOST_CXXFLAGS $CCFITS_CXXFLAGS"
AC_SUBST(MY_CXXFLAGS)
MY_LIBCXXFLAGS="-fPIC $MY_CXXFLAGS"
AC_SUBST(MY_LIBCXXFLAGS)

MY_LDADD="-L\$(top_builddir)/src/lib/IceBRG_lensing -L\$(top_builddir)/src/lib/IceBRG_physics -L\$(top_builddir)/src/lib/IceBRG_main -lboost_serialization -lCCfits $parallelization_flags $BOOST_LDADD $CCFITS_LDADD"
AC_SUBST(MY_LDADD)

MY_LDADD_ICEBRG="-lIceBRG_lensing -lIceBRG_physics -lIceBRG_main"
AC_SUBST(MY_LDADD_ICEBRG)

MY_LDFLAGS="$parallelization_flags $BOOST_LDFLAGS $CCFITS_LDFLAGS"
AC_SUBST(MY_LDFLAGS)

# Print all warnings and errors at the end

AS_IF([test "x$sky2xy_found" == "xyes" -a "x$xy2sky_found" == "xyes"], [],
	[AC_MSG_WARN([Unable to find sky2xy and/or xy2sky. This package can compile, but certain executables which call these programs will fail until these are installed. They can be installed as part of the wcstools package.])])
	
AS_IF([test "x$fpack_found" == "xyes" -a "x$funpack_found" == "xyes"], [],
	[AC_MSG_WARN([Unable to find fpack and/or funpack. This package can compile, but certain executables which call these programs will fail until these are installed.])])

AS_IF([test "x$doxygen_found" == "xyes"], [],
	[AC_MSG_WARN([Unable to find Doxygen. This package can compile, but documentation will not be generated.])])

AS_IF([test "x$mypj_found_std_headers" != "xyes"],
	[AC_MSG_WARN([Unable to find the standard definitions headers.])])

AS_IF([test "x$mypj_found_eigen_headers" != "xyes"],
	[AC_MSG_ERROR([Unable to find Eigen headers. If you know Eigen is installed, set the include path to include their location.])])
AS_IF([test "x$mypj_found_ccfits_headers" != "xyes"],
	[AC_MSG_WARN([Unable to find CCfits headers. If you know CCfits is installed, set the include path to include their location. Compilation for all executables except get_unmasked_fractions can still proceed.])
		mypj_compile_get_unmasked_fractions=""],
	[mypj_compile_get_unmasked_fractions="CFHTLenS_get_unmasked_fractions"])
AC_SUBST(mypj_compile_get_unmasked_fractions)

AC_OUTPUT
