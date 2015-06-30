/**********************************************************************\
 @file magnification_functors.h
 ------------------

 This file contains functions used in the calculations of magnification
 values.

 **********************************************************************

 Copyright (C) 2014  Bryan R. Gillis

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


// body file: magnification_functors.cpp

#include "IceBRG_main/common.h"

#ifndef _BRG_MAGNIFICATION_FUNCTORS_H_INCLUDED_
#define _BRG_MAGNIFICATION_FUNCTORS_H_INCLUDED_

namespace IceBRG {

class mag_expected_count_functor
{
private:

	flt_type _z_mean_;

public:

	// Constructors and destructor
#if(1)
	mag_expected_count_functor(flt_type init_z=0)
	: 	_z_mean_(init_z)
	{
	}
	virtual ~mag_expected_count_functor()
	{
	}
#endif

	// Operator()
	flt_type operator()(const long_flt_type & m=true) const;

}; // class expected_count_functor

class mu_signal_integration_functor
{
private:

	flt_type _z_mean_;

public:

	// Constructors and destructor
#if(1)
	mu_signal_integration_functor(flt_type init_z=0)
	:	_z_mean_(init_z)
	{
	}
	virtual ~mu_signal_integration_functor()
	{
	}
#endif

	// Operator()
	flt_type operator()(const long_flt_type & m=true) const;

}; // class mu_signal_integration_functor

class mu_weight_integration_functor
{
private:

	flt_type _z_mean_;

public:

	// Constructors and destructor
#if(1)
	mu_weight_integration_functor(flt_type init_z=0)
	:	_z_mean_(init_z)
	{
	}
	virtual ~mu_weight_integration_functor()
	{
	}
#endif

	// Operator()
	flt_type operator()(const long_flt_type & m=true) const;

}; // class mu_signal_integration_functor

} // namespace IceBRG

#endif // _BRG_MAGNIFICATION_FUNCTORS_H_INCLUDED_
