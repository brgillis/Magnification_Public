/**********************************************************************\
 @file Schechter_like_functor.h
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

// body file: Schechter_like_functor.cpp

#ifndef _BRG_SCHECHTER_LIKE_FUNCTOR_H_INCLUDED_
#define _BRG_SCHECHTER_LIKE_FUNCTOR_H_INCLUDED_

#include <cassert>
#include <utility>

#include "IceBRG_main/container/tuple.hpp"
#include "IceBRG_main/math/functor/functor.hpp"
#include "IceBRG_main/units/units.hpp"

namespace IceBRG {

typedef custom_unit_type<0,0,0,-2,0> inverse_square_angle;

/**
 *
 */
class Schechter_like_functor: public functor<flt_type,
	inverse_square_angle,
	tuple<inverse_square_angle,flt_type,flt_type,flt_type,
			inverse_square_angle,flt_type,flt_type>> {
private:
	const size_t _num_params_ = 7;

public:
	typedef flt_type fin_type;
	typedef inverse_square_angle fout_type;
	typedef tuple<inverse_square_angle,flt_type,flt_type,flt_type,
			inverse_square_angle,flt_type,flt_type> params_type;

	typedef functor<flt_type, inverse_square_angle, params_type> base_type;

	Schechter_like_functor()
	{
	}
	template< typename T >
	Schechter_like_functor(T && init_params )
	: base_type(std::forward<T>(init_params))
	{
	}
	virtual ~Schechter_like_functor()
	{
	}

	// Params accessors
#if (1)

	inverse_square_angle N_scale() const
	{
		return params().get<0>();
	}
	flt_type m_star() const
	{
		return params().get<1>();
	}
	flt_type alpha() const
	{
		return params().get<2>();
	}
	flt_type mag_lower_lim_sharpness() const
	{
		return params().get<3>();
	}
	inverse_square_angle mag23_jump() const
	{
		return params().get<4>();
	}
	flt_type mag_upper_lim() const
	{
		return params().get<5>();
	}
	flt_type mag_upper_lim_sharpness() const
	{
		return params().get<6>();
	}

#endif

	// Function method

	fout_type operator()( const fin_type & in_params ) const;

};

} // namespace IceBRG

#endif // _BRG_SCHECHTER_LIKE_FUNCTOR_H_INCLUDED_
