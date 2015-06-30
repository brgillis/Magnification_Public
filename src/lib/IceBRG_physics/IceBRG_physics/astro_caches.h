/**********************************************************************\
  @file astro_caches.h

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

// body file: astro_caches.cpp

#ifndef _BRG_ASTRO_CACHES_H_INCLUDED_
#define _BRG_ASTRO_CACHES_H_INCLUDED_

#include <string>

#include "IceBRG_main/common.h"

#include "IceBRG_main/math/cache/cache.hpp"
#include "IceBRG_main/math/cache/cache_2d.hpp"

namespace IceBRG
{

class dfa_cache : public brg_cache<dfa_cache>
{
	// "Distance from angle_type" cache
private:

	DECLARE_BRG_CACHE_STATIC_VARS();

	friend class brg_cache<dfa_cache>;

protected:

	const std::string _name_base() const throw()
	{
		return "dfa";
	}

	// Long-form calculation function.
	flt_type _calculate( const flt_type & in_param ) const;

#ifdef _BRG_USE_UNITS_

	// Gets the result in the proper units
	const any_units_type _units( const flt_type & v ) const;
	const any_units_type _inverse_units(const flt_type & v) const;

#endif

public:

};
// class dfa_cache

class add_cache : public brg_cache_2d<add_cache>
{
	// Angular diameter distance_type
private:

	DECLARE_BRG_CACHE_2D_STATIC_VARS();

	friend class brg_cache_2d<add_cache>;

protected:

	const std::string _name_base() const throw()
	{
		char name_base[BRG_CACHE_ND_NAME_SIZE] = "ang_di_d";
		return name_base;
	}

	// Long-form calculation function.
	flt_type _calculate( const flt_type & in_param_1, const flt_type & in_param_2 ) const;

#ifdef _BRG_USE_UNITS_

	// Gets the result in the proper units
	const any_units_type _units( const flt_type & v ) const;
	const any_units_type _inverse_units(const flt_type & v) const;

#endif

public:

};
// class add_cache

class tfa_cache : public brg_cache<tfa_cache>
{
	// "Time from a (scale factor)" cache
private:

	DECLARE_BRG_CACHE_STATIC_VARS();

	friend class brg_cache<tfa_cache>;

protected:

	const std::string _name_base() const throw()
	{
		return "tfa";
	}

	// Long-form calculation function.
	flt_type _calculate( const flt_type & in_param ) const;

#ifdef _BRG_USE_UNITS_

	// Gets the result in the proper units
	const any_units_type _units( const flt_type & v ) const;
	const any_units_type _inverse_units(const flt_type & v) const;

#endif

public:

};
// class tfa_cache

} // end namespace IceBRG

#endif // __BRG_ASTRO_CACHES_H_INCLUDED__

