/**********************************************************************\
 @file mean_weight.hpp
 ------------------

 TODO <Insert file description here>

 **********************************************************************

 Copyright (C) 2015  Bryan R. Gillis

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


#ifndef _BRG_MEAN_WEIGHT_HPP_INCLUDED_
#define _BRG_MEAN_WEIGHT_HPP_INCLUDED_

#include <limits>

#include <boost/accumulators/framework/accumulator_base.hpp>
#include <boost/accumulators/framework/extractor.hpp>
#include <boost/accumulators/framework/features.hpp>
#include <boost/accumulators/framework/parameters/sample.hpp>
#include <boost/accumulators/statistics/count.hpp>
#include <boost/accumulators/statistics/weighted_mean.hpp>

#include "IceBRG_main/common.h"

#include "IceBRG_main/math/misc_math.hpp"

namespace boost { namespace accumulators {

namespace impl {

template<typename Sample>
struct mean_weight_accumulator
: accumulator_base
{
  typedef flt_type result_type;

  mean_weight_accumulator(dont_care)
  {
  }

  template<typename Args>
  result_type result(Args const &args) const
  {
	  if(count(args[accumulator])<1) return 1.;
	  return sum_of_weights(args[accumulator])/count(args[accumulator]);
  }
};

} // namespace impl

namespace tag {

struct mean_weight
  : depends_on< sum_of_weights, count >
{
	typedef mpl::true_ is_weight_accumulator;
	typedef accumulators::impl::mean_weight_accumulator< mpl::_1 > impl;
};

} // namespace tag

namespace extract {
extractor< tag::mean_weight > const mean_weight = {};
} // namespace extract
using extract::mean_weight;

}} // End all namespaces



#endif // _BRG_MEAN_WEIGHT_HPP_INCLUDED_
