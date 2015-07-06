# README #

This is a pre-release public posting of the code used for Gillis and Taylor (2015). There's still a lot of documentation to do, though.

### What is this repository for? ###

* Code used for Gillis and Taylor (2015), including a release of my C++ library, "IceBRG"
* v0.9.1
* [Learn Markdown](https://bitbucket.org/tutorials/markdowndemo)

### How do I get set up? ###

* Summary of set up

This project is set up with autotools, so it can be built from source on a Linux-like system with the standard:

```
#!bash

./configure <options>
make <options>
make install
```

My quick recommendation for options is:

```
#!bash

./configure --enable-full-optimization (add --prefix=$HOME for a user-space install)
make -j8
make install
```

If the configuration step fails, see below for relevant options that may help point to dependencies.

You can uninstall through:

```
#!bash

make uninstall
```

and clean up the installation directory through:

```
#!bash

make clean
```


* Configuration

Relevant configure options:

--help (view help and configure options)
--prefix=??? (install in a particular directory. --prefix=$HOME will do install in userspace)
--disable-parallelization (unless this flag is used, the executables will be installed using parallelization. With this, only one thread will be run at a time.)
--enable-full-optimization (overrides default C++ compilation flags (-g -O2) and uses a recommended set for better optimization. I recommend using this unless you want to tweak the flags yourself.)
--with-boost=??? (point to a specific location to find boost headers)
--with-boost-libdir=??? (point to a specific location to find boost libraries)
--with-ccfits=??? (point to a specific location to find CCfits headers)
--with-ccfits-libdir=??? (point to a specific location to find the CCfits library)
CPPFLAGS=??? (set specific preprocessor flags. For instance, CPPFLAGS='-I/path/to/Eigen/' to help find the Eigen headers.

* Dependencies

C++11-compliant C++ compiler
Python 2.6 or greater
Boost headers and Boost Serialization library
Eigen headers
CCfits headers and library
wcstools (needed at runtime for certain executables, but not needed at installation)

* How to run tests

Once it's installed, you can run a test analysis through going to a directory where you want to store data, then running the series of commands:


```
#!bash

CFHTLenS_Mag_get_fields.py
CFHTLenS_Mag_filter_catalogues
CFHTLenS_Mag_get_unmasked_fractions
CFHTLenS_Mag_count_sources
CFHTLenS_Mag_measure_weak_lensing
CFHTLenS_Mag_fit_lensing_model

```

This will run all necessary steps on the process up through fitting a lensing model to the data. By default, this only downloads one pointing from the CFHTLenS server (to lighten the load on it and speed things up for those only performing quick tests). A configure file (not distributed at the moment) may be used with the CFHTLenS_Mag_measure_weak_lensing step to modify the lens bins for which the lensing signal is measured.

### Who do I talk to? ###

* Bryan Gillis - repo owner