/**********************************************************************\
 @file effective_count.hpp
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

#ifndef _BRG_EFFECTIVE_COUNT_HPP_INCLUDED_
#define _BRG_EFFECTIVE_COUNT_HPP_INCLUDED_

#include <boost/accumulators/framework/accumulator_base.hpp>
#include <boost/accumulators/framework/extractor.hpp>
#include <boost/accumulators/framework/features.hpp>
#include <boost/accumulators/framework/parameters/sample.hpp>
#include <boost/accumulators/framework/depends_on.hpp>
#include <boost/accumulators/statistics/count.hpp>
#include <boost/accumulators/statistics/sum.hpp>

#include "IceBRG_main/common.h"

#include "IceBRG_main/math/misc_math.hpp"
#include "IceBRG_main/math/statistics/sum_of_square_weights.hpp"
#include "IceBRG_main/units/units.hpp"

namespace boost { namespace accumulators {

namespace impl {

template<typename Sample>
struct effective_count_accumulator
: accumulator_base
{
  typedef flt_type result_type;

  effective_count_accumulator(dont_care)
  {
  }

  template<typename Args>
  result_type result(Args const &args) const
  {
	  if(count(args[accumulator])<=1) return count(args[accumulator]);
	  if(sum_of_square_weights(args[accumulator])==0) return IceBRG::units_cast<result_type>(0.);
	  return IceBRG::square(sum_of_weights(args[accumulator]))/sum_of_square_weights(args[accumulator]);
  }
};

} // namespace impl

namespace tag {

struct effective_count
  : depends_on< count, sum_of_weights, sum_of_square_weights >
{
	typedef mpl::true_ is_weight_accumulator;
	typedef accumulators::impl::effective_count_accumulator< mpl::_1 > impl;
};

} // namespace tag

namespace extract {
extractor< tag::effective_count > const effective_count = {};
} // namespace extract
using extract::effective_count;

}} // End all namespaces

#endif // _BRG_EFFECTIVE_COUNT_HPP_INCLUDED_
