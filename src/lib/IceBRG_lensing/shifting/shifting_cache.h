/**********************************************************************\
 @file shifting_cache.h
 ------------------

 Cache for getting the expected shift sigma for a given separation and
 redshift.

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

// body file: shifting_cache.cpp
#ifndef _BRG_SHIFTING_CACHE_H_INCLUDED_
#define _BRG_SHIFTING_CACHE_H_INCLUDED_

#include "IceBRG_main/units/units.hpp"
#include <string>

#include "IceBRG_main/common.h"
#include "IceBRG_main/math/cache/cache_2d.hpp"


namespace IceBRG {

/**
 *
 */
class shifting_cache: public brg_cache_2d<shifting_cache> {
private:

	DECLARE_BRG_CACHE_2D_STATIC_VARS();

	friend class brg_cache_2d<shifting_cache>;

protected:

	std::string _name_base() const
	{
		char name_base[BRG_CACHE_ND_NAME_SIZE] = "lensshft";
		return name_base;
	}

#ifdef _BRG_USE_UNITS_

	// Tells what units the result should have
	any_units_type _units( const flt_type & v ) const
	{
		return units_cast<surface_density_type>(v);
	}

#endif // _BRG_USE_UNITS_

	// Long-form calculation function.
	flt_type _calculate( const flt_type & in_param_1, const flt_type & in_param_2 ) const;

public:
	shifting_cache()
	{
	}
	~shifting_cache()
	{
	}
};

} // end namespace IceBRG

#endif // _BRG_SHIFTING_CACHE_H_INCLUDED_
