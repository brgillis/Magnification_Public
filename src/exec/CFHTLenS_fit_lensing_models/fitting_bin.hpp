/**********************************************************************\
 @file fitting_bin.hpp
 ------------------

 TODO <Insert file description here>

 **********************************************************************

 Copyright (C) 2015  Bryan R. Gillis

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.

 \**********************************************************************/

#ifndef _BRG_FITTING_BIN_HPP_INCLUDED_
#define _BRG_FITTING_BIN_HPP_INCLUDED_

#include <utility>

#include "common.h"

/**
 *
 */
struct fitting_bin
{
	flt_type x;
	flt_type y_mean;
	flt_type y_stderr;

	template< typename Tx=flt_type, typename Ty_mean=flt_type, typename Ty_stderr=flt_type >
	fitting_bin( Tx && x = 0., Ty_mean && y_mean = 0., Ty_stderr && y_stderr = 1.)
	: x(std::forward<Tx>(x)),
	  y_mean(std::forward<Ty_mean>(y_mean)),
	  y_stderr(std::forward<Ty_stderr>(y_stderr))
	{
	}
};

#endif // _BRG_FITTING_BIN_HPP_INCLUDED_
