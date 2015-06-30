/**********************************************************************\
 @file magnification_functors.cpp
 ------------------

 Source file for the classes defined in magnification_functors.h.

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

#include "IceBRG_main/common.h"

#include "IceBRG_main/math/misc_math.hpp"
#include "IceBRG_main/units/units.hpp"

#include "IceBRG_physics/astro.h"
#include "IceBRG_lensing/magnification/expected_count_cache.h"
#include "IceBRG_lensing/magnification/magnification_alpha.h"
#include "IceBRG_lensing/magnification/magnification_functors.h"

namespace IceBRG {

constexpr flt_type test_fudge_factor = 1.;

flt_type mag_expected_count_functor::operator() (const long_flt_type & m) const
{
	flt_type res = any_cast<flt_type>(expected_count_cache().get(m,_z_mean_));

	return res;
}

flt_type mu_signal_integration_functor::operator() (const long_flt_type & m) const
{
	long_flt_type alpha = magnification_alpha(m,_z_mean_);
	long_flt_type count = test_fudge_factor*any_cast<flt_type>(expected_count_cache().get(m,_z_mean_));
	return count*(alpha-1);
}

flt_type mu_weight_integration_functor::operator() (const long_flt_type & m) const
{
	long_flt_type alpha = magnification_alpha(m,_z_mean_);
	long_flt_type count = test_fudge_factor*any_cast<flt_type>(expected_count_cache().get(m,_z_mean_));
	return count*square(alpha-1);
}

} // namespace IceBRG

