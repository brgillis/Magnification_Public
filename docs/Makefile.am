if HAVE_DOXYGEN
directory = $(top_srcdir)/docs/html

noinst_MANS = $(directory)/index.html
$(directory)/index.html: doxyfile.stamp

doxyfile.stamp:
	doxygen Doxyfile
	echo Timestamp > doxyfile.stamp

CLEANFILES = doxyfile.stamp

all-local: doxyfile.stamp
clean-local:
	rm -rf $(top_srcdir)/docs/html
	rm -rf $(top_srcdir)/docs/latex
endif
