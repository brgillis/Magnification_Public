The IceBRG_main library contains various C++ functions and classes which will likely be of use to programmers in a variety of
fields. Most of the library is header-only, with all classes and functions inlined, which is intended to allow better optimizations
at compilation time but may slow down compilation.

This section will go over various functions and classes available in the library, sorted by the files and folders they're contained
within.

==Miscellaneous

The header files discussed here reside in the root folder, "IceBRG_main". They can be include by, for instance "#include
<IceBRG_main/common.h>".

===call_program.hpp

This header defines a function "call_program(...)" which handles calling another program at the command-line and passing arguments
to it. See [the header](@ref call_program.hpp) for the required syntax.

===command_line.hpp

[This header](@ref command_line.hpp) defines various functions for processing command-line arguments, such as getting an argument and
throwing an exception if it isn't present, or testing if an argument satisfies a test function or otherwise using a default value.

===common.h

This file defines various common typedefs (eg. "flt_type" for double), to ease maintenance when its needed to change one of these. It
also includes a few global defines for numeric values. Notably, it defines "pi" as a constexpr flt_type with the value of M_PI.

Basically, include this if you want to get the typedefs and/or pi.

===Eigen.hpp

This is an include-wrapper for Eigen/Core, which includes it, defines begin and end functions for Eigen matrices and arrays (allowing
their elements to be iterated over instead of requiring a for loop with each element indexed), and defines a few typedefs for arrays.

Include this as an alternative to including Eigen/Core if you want to iterate over Eigen arrays, particularly with the C++11 "for each"
syntax.

===error_handling.h

[This header](@ref error_handling.h) defines functions for handling (non-critical) errors, warnings, and notifications, where the
desired behavior on an error is determined by the value of a static variable. This allows the value of the this variable to be changed
to alter whether errors will result in exceptions, printed warnings, etc.

===join_path.hpp

[This header](@ref join_path.hpp) defines a function to join two strings as elements of a file path, ensuring there's just one slash
between them. Note that it does this by replacing all instances of multiple slashes with a single slash in the resulting string, so it
won't work properly on strings that deliberately contain multiple slashes (eg. the "http://" at the beginning of a URL).

===lexical_cast.hpp

[This header](@ref lexical_cast.hpp) defines functions to apply a few extensions to Boost's lexical cast, such as functions which
return the minimum possible value for a variable if the value can't be cast, or casting to a boolean (useful for reading user-defined
values).

===utility.hpp

[This header](@ref utility.hpp) defines miscellaneous utility functions used within the library.

==Folders

===container

