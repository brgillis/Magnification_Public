/**********************************************************************\
 @file magnification_alpha.cpp
 ------------------

 File containing the implementation of magnification_alpha.

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

#include "IceBRG_main/math/calculus/differentiate.hpp"
#include "IceBRG_main/units/units.hpp"

#include "IceBRG_lensing/magnification/expected_count_cache.h"
#include "IceBRG_lensing/magnification/expected_count_derivative_cache.h"
#include "IceBRG_lensing/magnification/magnification_alpha.h"

namespace IceBRG {

flt_type magnification_alpha(const flt_type & m, const flt_type & z)
{
	//return 2; // For sanity test purposes - this leads to alpha-1 = 1, so a the measured mag signal is the count overdensity

	flt_type res = any_cast<flt_type>(expected_count_derivative_cache().get(m,z));

	if(res<=0) return 1.; // 1 corresponds to a flat profile, so this is appropriate for zero counts


	return res;
}

} // namespace IceBRG
