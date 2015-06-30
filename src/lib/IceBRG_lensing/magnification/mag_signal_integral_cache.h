/**********************************************************************\
 @file mag_signal_integral_cache.h
 ------------------

 TODO <Insert file description here>

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

// body file: mag_signal_integral_cache.cpp

#ifndef _BRG_MAG_SIGNAL_INTEGRAL_CACHE_H_INCLUDED_
#define _BRG_MAG_SIGNAL_INTEGRAL_CACHE_H_INCLUDED_

#include <string>

#include "IceBRG_main/common.h"

#include "IceBRG_main/math/cache/cache.hpp"
#include "IceBRG_main/units/units.hpp"

namespace IceBRG {

/**
 *
 */
class mag_signal_integral_cache: public brg_cache<mag_signal_integral_cache> {
	DECLARE_BRG_CACHE_STATIC_VARS();

	friend class brg_cache<mag_signal_integral_cache>;

protected:

	const std::string _name_base() const throw()
	{
		return "mag_signal_integral";
	}

#ifdef _BRG_USE_UNITS_

	// Tells what units the result should have. Only the units matter in the return, not the value
	const any_units_type _units( const flt_type & v ) const
	{
		return any_units_cast<custom_unit_type<0,0,0,-2,0>>(v);
	}

#endif // _BRG_USE_UNITS_

	// Long-form calculation function.
	flt_type _calculate( const flt_type & in_param_1 ) const;
	void _load_cache_dependencies() const;

public:
	mag_signal_integral_cache()
	{
	}
	virtual ~mag_signal_integral_cache()
	{
	}
};

} // end namespace IceBRG

#endif // _BRG_MAG_SIGNAL_INTEGRAL_CACHE_H_INCLUDED_
