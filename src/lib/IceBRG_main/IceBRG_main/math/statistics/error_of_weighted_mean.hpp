/**********************************************************************\
 @file error_of_weighted_mean.hpp
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


#ifndef _BRG_ERROR_OF_WEIGHTED_MEAN_HPP_INCLUDED_
#define _BRG_ERROR_OF_WEIGHTED_MEAN_HPP_INCLUDED_

#include <limits>

#include <boost/accumulators/framework/accumulator_base.hpp>
#include <boost/accumulators/framework/extractor.hpp>
#include <boost/accumulators/framework/features.hpp>
#include <boost/accumulators/framework/parameters/sample.hpp>
#include <boost/accumulators/statistics/count.hpp>
#include <boost/accumulators/statistics/weighted_variance.hpp>

#include "IceBRG_main/common.h"

#include "IceBRG_main/math/misc_math.hpp"
#include "IceBRG_main/math/statistics/effective_count.hpp"

namespace boost { namespace accumulators {

namespace impl {

template<typename Sample>
struct error_of_weighted_mean_accumulator
: accumulator_base
{
  typedef Sample result_type;

  error_of_weighted_mean_accumulator(dont_care)
  {
  }

  template<typename Args>
  result_type result(Args const &args) const
  {
	  if(count(args[accumulator])<2) return std::numeric_limits<result_type>::max();
	  return std::sqrt((weighted_variance(args[accumulator])/effective_count(args[accumulator])) *
			  count(args[accumulator])/(count(args[accumulator])-1));
  }
};

} // namespace impl

namespace tag {

struct error_of_weighted_mean
  : depends_on< weighted_variance, effective_count, count >
{
	typedef mpl::true_ is_weight_accumulator;
	typedef accumulators::impl::error_of_weighted_mean_accumulator< mpl::_1 > impl;
};

} // namespace tag

namespace extract {
extractor< tag::error_of_weighted_mean > const error_of_weighted_mean = {};
} // namespace extract
using extract::error_of_weighted_mean;

}} // End all namespaces



#endif // _BRG_STANDARD_ERROR_OF_WEIGHTED_MEAN_HPP_INCLUDED_
