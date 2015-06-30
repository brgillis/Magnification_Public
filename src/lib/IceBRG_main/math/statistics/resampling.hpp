/**********************************************************************\
 @file resampling.hpp
 ------------------

 Methods for statistical resampling of a dataset.

 **********************************************************************

 Copyright (C) 2015 brg

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

// TODO: Test all functions and classes here.

#ifndef BRG_MATH_STATISTICS_RESAMPLING_HPP_
#define BRG_MATH_STATISTICS_RESAMPLING_HPP_

#include <algorithm>
#include <functional>
#include <random>
#include <utility>

#include <boost/optional.hpp>

#include "IceBRG_main/Eigen.hpp"

#include "IceBRG_main/common.h"
#include "IceBRG_main/math/random/random_functions.hpp"

namespace IceBRG
{

// Generate a case-resampled version of a vector or array
template< typename T_vec, typename T_gen=decltype(rng) >
T_vec get_case_resampled(const T_vec & vec, T_gen & gen=rng)
{
	T_vec res(vec);

	std::uniform_int_distribution<int_type> dist(0,vec.size()-1);

	for( auto & v : res )
	{
		v = vec[dist(gen)];
	}

	return res;
}

// Get a jackknife resample - note that this doesn't preserve ordering
template< typename T_vec,
	std::enable_if<IceBRG::is_eigen_container<T_vec>::value,char> = 0>
T_vec get_jackknife_resampled(const T_vec & vec,
		const int_type & sample_num = 0,
		const int_type & num_samples = 2)
{
	assert(num_samples>=1);
	assert(sample_num>=0);
	assert(sample_num<num_samples);

	T_vec res(vec);

	res.conservativeResize(std::partition(
	  res.data(),
	  res.data()+res.size(),
	  [&vec](const int_type & i){return vec(i) % num_samples == sample_num;})-res.data());

	return res;
}
template< typename T_vec,
	std::enable_if<!IceBRG::is_eigen_container<T_vec>::value,char> = 0>
T_vec get_jackknife_resampled(const T_vec & vec,
		const int_type & sample_num = 0,
		const int_type & num_samples = 2)
{
	assert(num_samples>=1);
	assert(sample_num>=0);
	assert(sample_num<num_samples);

	T_vec res(vec);

	res.resize(std::partition(
	  res.data(),
	  res.data()+res.size(),
	  [&vec](const int_type & i){return vec(i) % num_samples == sample_num;})-res.data());

	return res;
}

template< typename T_vec,
		typename T_func=std::function<decltype(T_vec().mean())(const T_vec &)>,
		typename T_gen=decltype(rng) >
class bootstrap
{
public:
	// Public typedefs
	typedef T_vec array_type;
	typedef decltype(array_type()[0]) value_type;

	typedef T_func func_type;
	typedef decltype(func_type(array_type())) func_output_type;
	typedef Eigen::Array<func_output_type,1,Eigen::Dynamic> func_output_array_type;

	typedef T_gen gen_type;

private:

	constexpr int_type _default_num_resamples_ = 50;

	const array_type * _data_;
	const func_type * _callback_;
	int_type _num_resamples_;
	gen_type * _gen_;

	mutable boost::optional<func_output_array_type> _results_;
	mutable boost::optional<func_output_type> _mean_;
	mutable boost::optional<func_output_type> _std_of_mean_;

	bool _valid_state() const
	{
		if((_data_==nullptr) || (_callback_ == nullptr) || (_gen_ == nullptr)) return false;
		if(_num_resamples_ < 2) return false;
		if(_data_->size() <= 0) return false;
		return true;
	}

	// Generate results from resampling
	void _generate_results() const
	{
		assert(_valid_state());

		func_output_array_type new_results(get_num_resamples());

		for(int_type i=0; i<new_results.size(); ++i)
		{
			new_results[i] = _callback_(get_case_resampled(*_data_, *_gen_));
		}

		_results_ = std::move(new_results);
	}

	// Generate mean of results
	void _generate_mean() const
	{
		assert(_valid_state());

		if(!_results_) _generate_results();

		_mean_ = _results_->mean();
	}

	// Generate std of mean of results
	void _generate_std_of_mean() const
	{
		assert(_valid_state());

		if(!_results_) _generate_results();

		_std_of_mean_ = _results_->std();
	}

public:

	// Constructors
#if(1)
	// Normal constructor
	bootstrap( const T_vec & data,
				const T_func & callback = [] (const array_type & vec) {return vec.mean();},
				const int_type & num_resamples = _default_num_resamples_,
				gen_type & gen = rng)
	: _data_(&data),
	  _callback_(&callback),
	  _num_resamples_(num_resamples),
	  _gen_(&gen)
	{
	}

	// Default constructor
	bootstrap()
	: _data_(nullptr),
	  _callback_([] (const array_type & vec) {return vec.mean();}),
	  _num_resamples_(_default_num_resamples_),
	  _gen_(&rng)
	{
	}
#endif // Constructors

	// Getters and setters
#if(1)
	const func_type & get_callback() const
	{
		return *_callback_;
	}

	void set_callback(const func_type & callback)
	{
		_callback_ = &callback;
	}

	const array_type & get_data() const
	{
		return *_data_;
	}

	void set_data(const array_type & data)
	{
		_data_ = &data;
	}

	gen_type & get_gen() const
	{
		return *_gen_;
	}

	void set_gen(gen_type & gen)
	{
		_gen_ = &gen;
	}

	const int_type & get_num_resamples() const noexcept
	{
		return _num_resamples_;
	}

	void set_num_resamples(const int_type & num_resamples)
	{
		_num_resamples_ = num_resamples;
	}

#endif // Getters and setters

	// Get results
#if(1)
	func_output_type get_mean()
	{
		if(!_mean_) _generate_mean();
		return *_mean_;
	}
	func_output_type get_std()
	{
		if(!_std_of_mean_) _generate_std_of_mean();
		return *_std_of_mean_;
	}
#endif
};

template< typename T_vec,
		typename T_func=std::function<decltype(T_vec().mean())(const T_vec &)> >
class jackknife
{
public:
	// Public typedefs
	typedef T_vec array_type;
	typedef decltype(array_type()[0]) value_type;

	typedef T_func func_type;
	typedef decltype(func_type(array_type())) func_output_type;
	typedef Eigen::Array<func_output_type,1,Eigen::Dynamic> func_output_array_type;

private:

	constexpr int_type _default_num_resamples_ = 50;

	const array_type * _data_;
	const func_type * _callback_;
	int_type _num_resamples_;

	mutable boost::optional<func_output_array_type> _results_;
	mutable boost::optional<func_output_type> _mean_;
	mutable boost::optional<func_output_type> _std_of_mean_;

	bool _valid_state() const
	{
		if((_data_==nullptr) || (_callback_ == nullptr)) return false;
		if(_num_resamples_ < 2) return false;
		if(_data_->size() <= 0) return false;
		return true;
	}

	// Generate results from resampling
	void _generate_results() const
	{
		assert(_valid_state());

		func_output_array_type new_results(get_num_resamples());

		for(int_type i=0; i<new_results.size(); ++i)
		{
			new_results[i] = _callback_(get_jackknife_resampled(*_data_, i, _num_resamples_));
		}

		_results_ = std::move(new_results);
	}

	// Generate mean of results
	void _generate_mean() const
	{
		assert(_valid_state());

		if(!_results_) _generate_results();

		_mean_ = _results_->mean();
	}

	// Generate std of mean of results
	void _generate_std_of_mean() const
	{
		assert(_valid_state());

		if(!_results_) _generate_results();

		_std_of_mean_ = _results_->std() * _num_resamples_/(_num_resamples_-1);
	}

public:

	// Constructors
#if(1)
	// Normal constructor
	jackknife( const T_vec & data,
				const T_func & callback = [] (const array_type & vec) {return vec.mean();},
				const int_type & num_resamples = _default_num_resamples_)
	: _data_(&data),
	  _callback_(&callback),
	  _num_resamples_(num_resamples)
	{
	}

	// Default constructor
	jackknife()
	: _data_(nullptr),
	  _callback_([] (const array_type & vec) {return vec.mean();}),
	  _num_resamples_(_default_num_resamples_)
	{
	}
#endif // Constructors

	// Getters and setters
#if(1)
	const func_type & get_callback() const
	{
		return *_callback_;
	}

	void set_callback(const func_type & callback)
	{
		_callback_ = &callback;
	}

	const array_type & get_data() const
	{
		return *_data_;
	}

	void set_data(const array_type & data)
	{
		_data_ = &data;
	}

	const int_type & get_num_resamples() const noexcept
	{
		return _num_resamples_;
	}

	void set_num_resamples(const int_type & num_resamples)
	{
		_num_resamples_ = num_resamples;
	}

#endif // Getters and setters

	// Get results
#if(1)
	func_output_type get_mean()
	{
		if(!_mean_) _generate_mean();
		return *_mean_;
	}
	func_output_type get_std()
	{
		if(!_std_of_mean_) _generate_std_of_mean();
		return *_std_of_mean_;
	}
#endif
};

} // namespace IceBRG

#endif // BRG_MATH_STATISTICS_RESAMPLING_HPP_
