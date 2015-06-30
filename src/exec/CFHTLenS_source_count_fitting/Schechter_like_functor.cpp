/**********************************************************************\
 @file Schechter_like_functor.cpp
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

#include <cmath>
#include <cstdlib>

#include "IceBRG_main/units/units.hpp"

#include "Schechter_like_functor.h"

namespace IceBRG {

Schechter_like_functor::fout_type Schechter_like_functor::operator()(
		const Schechter_like_functor::fin_type & in_param ) const
{
	const flt_type & mag_jump_limit = 23.;

	const flt_type & x = std::pow(10,0.4*(m_star()-in_param));
	fout_type result = 0.4*std::log(10.)*N_scale()*std::pow(x,alpha()+1)*
			std::exp(-std::pow(x,mag_lower_lim_sharpness()));

	if(in_param>=mag_jump_limit) result += mag23_jump();

	const flt_type & xh = std::pow(10,0.4*(in_param-mag_upper_lim()));

	result *= std::exp(-std::pow(xh,mag_upper_lim_sharpness()));

	return result;
}

}
