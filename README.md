# README #

This is a pre-release public posting of the code used for Gillis and Taylor (2015). There's still a lot of documentation to do, though.

### What is this repository for? ###

* Code used for Gillis and Taylor (2015), including a release of my C++ library, "IceBRG"
* v0.9.0
* [Learn Markdown](https://bitbucket.org/tutorials/markdowndemo)

### How do I get set up? ###

* Summary of set up

This project is set up with autotools, so it can be built from source on a Linux-like system with the standard:

```
#!bash

./configure (options)
make
make install
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

Boost headers and Boost Serialization library
Eigen headers
CCfits headers and library
wcstools (needed at runtime for certain executables, but not needed at installation)

* How to run tests

(TBW)

### Contribution guidelines ###

(TBW)

### Who do I talk to? ###

* Bryan Gillis - repo owner