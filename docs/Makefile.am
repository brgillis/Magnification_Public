if HAVE_DOXYGEN
directory = $(top_srcdir)/docs/man/man3

noinst_MANS = $(directory)/config.h.3
$(directory)/config.h.3: doxyfile.stamp

doxyfile.stamp:
	doxygen Doxyfile
	echo Timestamp > doxyfile.stamp

CLEANFILES = doxyfile.stamp

all-local: doxyfile.stamp
clean-local:
	rm -rf $(top_srcdir)/docs/man
	rm -rf $(top_srcdir)/docs/html
	rm -rf $(top_srcdir)/docs/latex
endif
