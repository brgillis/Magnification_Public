/**********************************************************************\
 @file sum_of_square_weights.hpp
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


#ifndef _BRG_SUM_OF_SQUARE_WEIGHTS_HPP_INCLUDED_
#define _BRG_SUM_OF_SQUARE_WEIGHTS_HPP_INCLUDED_

#include <boost/accumulators/framework/accumulator_base.hpp>
#include <boost/accumulators/framework/extractor.hpp>
#include <boost/accumulators/framework/features.hpp>
#include <boost/accumulators/framework/parameters/sample.hpp>

#include "IceBRG_main/common.h"

#include "IceBRG_main/math/misc_math.hpp"

namespace boost { namespace accumulators {

namespace impl {

template<typename Sample>
struct sum_of_square_weights_accumulator
  : accumulator_base
{
    typedef decltype(Sample()*Sample()) result_type;

    template<typename Args>
    sum_of_square_weights_accumulator(Args const & args)
      : sum_of_square_weights(args[sample | Sample()])
    {
    }

    template<typename Args>
    void operator ()(Args const & args)
    {
        this->sum_of_square_weights += IceBRG::square(args[weight]);
    }

    result_type result(dont_care) const
    {
        return this->sum_of_square_weights;
    }
private:
    Sample sum_of_square_weights;
};

} // namespace impl

namespace tag {

struct sum_of_square_weights
  : depends_on<>
{
	typedef mpl::true_ is_weight_accumulator;
	typedef accumulators::impl::sum_of_square_weights_accumulator< mpl::_1 > impl;
};

} // namespace tag

namespace extract {
extractor< tag::sum_of_square_weights > const sum_of_square_weights = {};
} // namespace extract
using extract::sum_of_square_weights;

}} // End all namespaces

#endif // _BRG_SUM_OF_SQUARE_WEIGHTS_HPP_INCLUDED_
