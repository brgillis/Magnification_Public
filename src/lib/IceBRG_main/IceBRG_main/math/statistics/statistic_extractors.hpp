/**********************************************************************\
 @file statistic_extractors.hpp
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

#ifndef _BRG_MATH_STATISTICS_STATISTIC_EXTRACTORS_HPP_INCLUDED_
#define _BRG_MATH_STATISTICS_STATISTIC_EXTRACTORS_HPP_INCLUDED_

#include <utility>

#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/error_of_mean.hpp>
#include <boost/accumulators/statistics/moment.hpp>
#include <boost/accumulators/statistics/variance.hpp>
#include <boost/accumulators/statistics/weighted_mean.hpp>
#include <boost/accumulators/statistics/weighted_moment.hpp>
#include <boost/accumulators/statistics/weighted_variance.hpp>

#include "IceBRG_main/math/statistics/effective_count.hpp"
#include "IceBRG_main/math/statistics/error_of_weighted_mean.hpp"
#include "IceBRG_main/math/statistics/mean_weight.hpp"
#include "IceBRG_main/math/statistics/sum_of_square_weights.hpp"

namespace IceBRG {

// count
#if(1)
template<typename Ti>
auto extract_count(Ti && acc) -> decltype(boost::accumulators::count(std::forward<Ti>(acc)))
{
	return boost::accumulators::count(std::forward<Ti>(acc));
}

template<typename Ti>
auto safe_extract_count(Ti && acc) -> decltype(boost::accumulators::count(std::forward<Ti>(acc)))
{
	auto result = boost::accumulators::count(std::forward<Ti>(acc));
	if(isbad(result)) result = 0;
	return result;
}
#endif // count

// effective count
#if(1)
template<typename Ti>
auto extract_effective_count(Ti && acc) -> decltype(boost::accumulators::effective_count(std::forward<Ti>(acc)))
{
	return boost::accumulators::effective_count(std::forward<Ti>(acc));
}

template<typename Ti>
auto safe_extract_effective_count(Ti && acc) -> decltype(boost::accumulators::effective_count(std::forward<Ti>(acc)))
{
	auto result = boost::accumulators::effective_count(std::forward<Ti>(acc));
	if(isbad(result)) result = 0;
	return result;
}
#endif // effective count

// sum of weights
#if(1)
template<typename Ti>
auto extract_sum_of_weights(Ti && acc) -> decltype(boost::accumulators::sum_of_weights(std::forward<Ti>(acc)))
{
	return boost::accumulators::sum_of_weights(std::forward<Ti>(acc));
}

template<typename Ti>
auto safe_extract_sum_of_weights(Ti && acc) -> decltype(boost::accumulators::sum_of_weights(std::forward<Ti>(acc)))
{
	auto result = boost::accumulators::sum_of_weights(std::forward<Ti>(acc));
	if(isbad(result)) result = 0;
	return result;
}
#endif // sum of weights

// mean weight
#if(1)
template<typename Ti>
auto extract_mean_weight(Ti && acc) -> decltype(boost::accumulators::mean_weight(std::forward<Ti>(acc)))
{
	return boost::accumulators::mean_weight(std::forward<Ti>(acc));
}

template<typename Ti>
auto safe_extract_mean_weight(Ti && acc) -> decltype(boost::accumulators::mean_weight(std::forward<Ti>(acc)))
{
	auto result = boost::accumulators::mean_weight(std::forward<Ti>(acc));
	if(isbad(result)) result = 0;
	return result;
}
#endif // mean weight

// sum of square weights
#if(1)
template<typename Ti>
auto extract_sum_of_square_weights(Ti && acc) -> decltype(boost::accumulators::sum_of_square_weights(std::forward<Ti>(acc)))
{
	return boost::accumulators::sum_of_square_weights(std::forward<Ti>(acc));
}

template<typename Ti>
auto safe_extract_sum_of_square_weights(Ti && acc) -> decltype(boost::accumulators::sum_of_square_weights(std::forward<Ti>(acc)))
{
	auto result = boost::accumulators::sum_of_square_weights(std::forward<Ti>(acc));
	if(isbad(result)) result = 0;
	return result;
}
#endif // sum of square weights

// sum
#if(1)
template<typename Ti>
auto extract_sum(Ti && acc) -> decltype(boost::accumulators::sum(std::forward<Ti>(acc)))
{
	return boost::accumulators::sum(std::forward<Ti>(acc));
}

template<typename Ti>
auto safe_extract_sum(Ti && acc) -> decltype(boost::accumulators::sum(std::forward<Ti>(acc)))
{
	auto result = boost::accumulators::sum(std::forward<Ti>(acc));
	if(isbad(result)) result = 0;
	return result;
}
#endif // sum

// weighted sum
#if(1)
template<typename Ti>
auto extract_weighted_sum(Ti && acc) -> decltype(boost::accumulators::weighted_sum(std::forward<Ti>(acc)))
{
	return boost::accumulators::weighted_sum(std::forward<Ti>(acc));
}

template<typename Ti>
auto safe_extract_weighted_sum(Ti && acc) -> decltype(boost::accumulators::weighted_sum(std::forward<Ti>(acc)))
{
	auto result = boost::accumulators::weighted_sum(std::forward<Ti>(acc));
	if(isbad(result)) result = 0;
	return result;
}
#endif // weighted sum

// mean
#if(1)
template<typename Ti>
auto extract_mean(Ti && acc) -> decltype(boost::accumulators::mean(std::forward<Ti>(acc)))
{
	return boost::accumulators::mean(std::forward<Ti>(acc));
}

template<typename Ti>
auto safe_extract_mean(Ti && acc) -> decltype(boost::accumulators::mean(std::forward<Ti>(acc)))
{
	auto result = boost::accumulators::mean(std::forward<Ti>(acc));
	if(isbad(result)) result = 0;
	return result;
}
#endif // mean

// weighted mean
#if(1)
template<typename Ti>
auto extract_weighted_mean(Ti && acc) -> decltype(boost::accumulators::weighted_mean(std::forward<Ti>(acc)))
{
	return boost::accumulators::weighted_mean(std::forward<Ti>(acc));
}

template<typename Ti>
auto safe_extract_weighted_mean(Ti && acc) -> decltype(boost::accumulators::weighted_mean(std::forward<Ti>(acc)))
{
	auto result = boost::accumulators::weighted_mean(std::forward<Ti>(acc));
	if(isbad(result)) result = 0;
	return result;
}
#endif // weighted mean

// error of mean
#if(1)
template<typename Ti>
auto extract_error_of_mean(Ti && acc) ->
	decltype(boost::accumulators::error_of<boost::accumulators::tag::mean>(std::forward<Ti>(acc)))
{
	return boost::accumulators::mean(std::forward<Ti>(acc));
}

template<typename Ti>
auto safe_extract_error_of_mean(Ti && acc) ->
decltype(boost::accumulators::error_of<boost::accumulators::tag::mean>(std::forward<Ti>(acc)))
{
	auto result = boost::accumulators::mean(std::forward<Ti>(acc));
	if(isbad(result)) result = 0;
	return result;
}
#endif // mean

// error of weighted mean
#if(1)
template<typename Ti>
auto extract_error_of_weighted_mean(Ti && acc) ->
	decltype(boost::accumulators::error_of_weighted_mean(std::forward<Ti>(acc)))
{
	return boost::accumulators::error_of_weighted_mean(std::forward<Ti>(acc));
}

template<typename Ti>
auto safe_extract_error_of_weighted_mean(Ti && acc) ->
	decltype(boost::accumulators::error_of_weighted_mean(std::forward<Ti>(acc)))
{
	auto result = boost::accumulators::error_of_weighted_mean(std::forward<Ti>(acc));
	if(isbad(result)) result = 0;
	return result;
}
#endif // error of weighted mean

// variance
#if(1)
template<typename Ti>
auto extract_variance(Ti && acc) -> decltype(boost::accumulators::variance(std::forward<Ti>(acc)))
{
	return boost::accumulators::variance(std::forward<Ti>(acc));
}

template<typename Ti>
auto safe_extract_variance(Ti && acc) -> decltype(boost::accumulators::variance(std::forward<Ti>(acc)))
{
	auto result = boost::accumulators::variance(std::forward<Ti>(acc));
	if(isbad(result)) result = 0;
	return result;
}
#endif // variance

// weighted variance
#if(1)
template<typename Ti>
auto extract_weighted_variance(Ti && acc) -> decltype(boost::accumulators::weighted_variance(std::forward<Ti>(acc)))
{
	return boost::accumulators::weighted_variance(std::forward<Ti>(acc));
}

template<typename Ti>
auto safe_extract_weighted_variance(Ti && acc) -> decltype(boost::accumulators::weighted_variance(std::forward<Ti>(acc)))
{
	auto result = boost::accumulators::weighted_variance(std::forward<Ti>(acc));
	if(isbad(result)) result = 0;
	return result;
}
#endif // weighted variance

// moment
#if(1)
template<int_type n, typename Ti>
auto extract_moment(Ti && acc) -> decltype(boost::accumulators::moment<n>(std::forward<Ti>(acc)))
{
	return boost::accumulators::moment<n>(std::forward<Ti>(acc));
}

template<int_type n, typename Ti>
auto safe_extract_moment(Ti && acc) -> decltype(boost::accumulators::moment<n>(std::forward<Ti>(acc)))
{
	auto result = boost::accumulators::moment<n>(std::forward<Ti>(acc));
	if(isbad(result)) result = 0;
	return result;
}
#endif // moment

// weighted moment
#if(1)
template<int_type n, typename Ti>
auto extract_weighted_moment(Ti && acc) -> decltype(boost::accumulators::weighted_moment<n>(std::forward<Ti>(acc)))
{
	return boost::accumulators::weighted_moment<n>(std::forward<Ti>(acc));
}

template<int_type n, typename Ti>
auto safe_extract_weighted_moment(Ti && acc) -> decltype(boost::accumulators::weighted_moment<n>(std::forward<Ti>(acc)))
{
	auto result = boost::accumulators::weighted_moment<n>(std::forward<Ti>(acc));
	if(isbad(result)) result = 0;
	return result;
}
#endif // weighted moment

}


#endif // _BRG_MATH_STATISTICS_STATISTIC_EXTRACTORS_HPP_INCLUDED_
