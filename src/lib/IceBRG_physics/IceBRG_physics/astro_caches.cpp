/**********************************************************************\
  @file astro_caches.cpp

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

#include <cstdlib>
#include <iostream>
#include <exception>
#include <string>

#include "IceBRG_main/common.h"

#include "IceBRG_main/math/cache/cache.hpp"
#include "IceBRG_main/math/cache/cache_2d.hpp"
#include "IceBRG_main/units/units.hpp"

#include "IceBRG_physics/astro.h"
#include "astro_caches.h"

/** Static Class Initialisation **/
#if (1)

// Initialisation for IceBRG::dfa_cache
DEFINE_BRG_CACHE_STATIC_VARS( dfa_cache, 0, 5, 0.001 );

// Initialisation for IceBRG::add_cache
DEFINE_BRG_CACHE_2D_STATIC_VARS( add_cache, 0, 5, 0.01,
		                                    0, 5, 0.01);

// Initialisation for IceBRG::tfa_cache
DEFINE_BRG_CACHE_STATIC_VARS( tfa_cache, 0.001, 1.02, 0.001 );

#endif // end Static Class Initialisation

/** Class Method Definitions **/
#if (1)

// IceBRG::dfa_cache class methods
#if (1)

flt_type IceBRG::dfa_cache::_calculate( const flt_type & in_param ) const
{
	return value_of(IceBRG::integrate_add( 0, in_param ));
}

#ifdef _BRG_USE_UNITS_

// Gets the result in the proper units
const IceBRG::any_units_type IceBRG::dfa_cache::_units( const flt_type & v ) const
{
	return units_cast<custom_unit_type<1,0,0,-1,0>>(v);
}
const IceBRG::any_units_type IceBRG::dfa_cache::_inverse_units(const flt_type & v) const
{
	return v;
}

#endif

#endif // end IceBRG::dfa_cache functions

// IceBRG::add_cache class methods
#if(1)

flt_type IceBRG::add_cache::_calculate( const flt_type & in_param_1,
		const flt_type & in_param_2) const
{
	return value_of(IceBRG::integrate_add(in_param_1,in_param_2));
}

#ifdef _BRG_USE_UNITS_

// Gets the result in the proper units
const IceBRG::any_units_type IceBRG::add_cache::_units( const flt_type & v ) const
{
	return units_cast<distance_type>(v);
}
const IceBRG::any_units_type IceBRG::add_cache::_inverse_units(const flt_type & v) const
{
	return v;
}

#endif

#endif // IceBRG::add_cache class methods

// IceBRG::tfa_cache class methods
#if(1)

flt_type IceBRG::tfa_cache::_calculate( const flt_type & in_param ) const
{
	return value_of(-IceBRG::integrate_ltd( 0, IceBRG::zfa( in_param ) ) / c);
}

#ifdef _BRG_USE_UNITS_

// Gets the result in the proper units
const IceBRG::any_units_type IceBRG::tfa_cache::_units( const flt_type & v ) const
{
	return units_cast<time_type>(v);
}
const IceBRG::any_units_type IceBRG::tfa_cache::_inverse_units(const flt_type & v) const
{
	return dimensionless_type(v);
}

#endif

#endif // IceBRG::tfa_cache class methods

#endif // end class methods
