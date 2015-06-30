/**********************************************************************\
 @file solver_classes.hpp
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

#ifndef _BRG_BRG_MATH_SOLVERS_SOLVER_CLASSES_HPP_INCLUDED_
#define _BRG_BRG_MATH_SOLVERS_SOLVER_CLASSES_HPP_INCLUDED_

#include <cassert>
#include <cmath>
#include <type_traits>
#include <utility>

#include <boost/optional.hpp>

#include "IceBRG_main/Eigen.hpp"

#include "IceBRG_main/math/random/random_functions.hpp"
#include "IceBRG_main/math/misc_math.hpp"
#include "IceBRG_main/math/safe_math.hpp"
#include "IceBRG_main/vector/elementwise_functions.hpp"
#include "IceBRG_main/utility.hpp"

namespace IceBRG {

template< typename T_value_type, int_type T_num_params=Eigen::Dynamic >
class MCMC_solver
{
public:
	// Public typedefs
	typedef T_value_type value_type;
	typedef Eigen::Array<value_type,T_num_params,1> array_type;
	typedef Eigen::Array<value_type,T_num_params,Eigen::Dynamic> array2d_type;
	typedef decltype(array_type().size()) size_type;

private:
	// Private result storage
	mutable boost::optional<array2d_type> _test_run_points_;
	mutable boost::optional<array_type> _means_;
	mutable boost::optional<array_type> _best_fits_;
	mutable boost::optional<array2d_type> _covars_;
	mutable boost::optional<array_type> _stddevs_;

public:

	void clear()
	{
		IceBRG::set_zero(_test_run_points_);
		IceBRG::set_zero(_means_);
		IceBRG::set_zero(_best_fits_);
		IceBRG::set_zero(_covars_);
		IceBRG::set_zero(_stddevs_);
	}

	template< typename f >
	const array_type & solve( const f & func, const array_type & init_in_params,
			const array_type & init_min_in_params,
			const array_type & init_max_in_params,
			const array_type & init_in_param_step_sigmas,
			const size_type & max_steps=1000000,
			const size_type & annealing_period=100000,
			const flt_type & annealing_factor=4,
			const size_type & skip_factor=10)
	{
		static_assert(std::is_convertible<decltype(func(array_type())),value_type>::value,
					  "Function passed to MCMC solver has incorrect return type.");

		assert(skip_factor>0);
		assert(max_steps>=2*annealing_period);

		assert(init_in_params.size()==init_min_in_params.size());
		assert(init_in_params.size()==init_max_in_params.size());
		assert(init_in_params.size()==init_in_param_step_sigmas.size());
		assert((init_in_params.size()==T_num_params) or (T_num_params==Eigen::Dynamic));

		size_type array_size = init_in_params.size();

		clear();

		bool bounds_check = true;

		// Check how bounds were passed in
		if((init_min_in_params.size()==0) && (init_max_in_params.size()==0))
			bounds_check = false;

		array_type min_in_params = init_min_in_params;
		array_type max_in_params = init_max_in_params;

		if(min_in_params.size() < max_in_params.size())
		{
			min_in_params.resize(max_in_params.size(),std::numeric_limits<value_type>::lowest());
		}
		if(max_in_params.size() < min_in_params.size())
		{
			max_in_params.resize(min_in_params.size(),std::numeric_limits<value_type>::max());
		}

		// Check if any of the params likely have max and min mixed up
		if(bounds_check)
		{
			for(int_type i = 0; i < max_in_params.size(); i++)
			{
				if(min_in_params(i)>max_in_params(i))
				{
					std::swap(min_in_params(i),max_in_params(i));
				} // if(min_in_params.at(i)>max_in_params.at(i))
			} // for(int_type i = 0; i < max_in_params.size(); i++)
		} // if(bounds_check)

		// Check step sizes

		array_type in_param_step_sigmas = init_in_param_step_sigmas;

		if(in_param_step_sigmas.size() < min_in_params.size())
			in_param_step_sigmas.resize(min_in_params.size(),0);

		for(int_type i = 0; i < in_param_step_sigmas.size(); i++)
		{
			if(in_param_step_sigmas(i) <= 0)
			{
				if(bounds_check)
				{
					in_param_step_sigmas(i) =
							(max_in_params(i)-min_in_params(i))/10.;
				}
				else
				{
					in_param_step_sigmas(i) = 1; // Default behaviour
				}
			}
		}

		// Initialise generally
		flt_type annealing = 1.;

		bool last_cycle = false;
		size_type last_cycle_count = 0;

		array_type current_in_params = init_in_params, test_in_params = init_in_params,
				best_in_params = init_in_params;
		value_type out_param, best_out_param;
		array_type mean_in_params = array_type::Zero(min_in_params.size());

		// Set up second cycle data
		bool first_cycle = true;
		bool second_cycle = false;
		size_type second_cycle_points_to_record = annealing_period/skip_factor;
		_test_run_points_ = array2d_type(array_size,second_cycle_points_to_record);

		// Get value at initial point
		try
		{
			out_param = func(test_in_params);
		}
		catch(const std::exception &e)
		{
			std::cerr << "Cannot execute solve_MCMC at initial point.\n";
			throw;
		}
		best_out_param = out_param;
		flt_type last_log_likelihood = -annealing*out_param/2;

		auto new_Gaus_rand = [] (const value_type & dummy) {return IceBRG::Gaus_rand();};

		for(int_type step = 0; step < max_steps; step++)
		{
			// Get a new value
			test_in_params = current_in_params + array_type::Zero(array_size).unaryExpr(new_Gaus_rand)*in_param_step_sigmas/annealing;

			// Check if it's in bounds
			if(bounds_check)
			{
				test_in_params = bound(min_in_params,test_in_params,max_in_params);
			}

			// Find the result for this value
			bool good_result = true;
			try
			{
				out_param = func(test_in_params);
			}
			catch(const std::exception &e)
			{
				good_result = false;
			}
			flt_type new_log_likelihood = -annealing*out_param/2;

			// If it's usable, check if we should step to it
			bool step_to_it = false;
			if(good_result)
			{
				if(new_log_likelihood>=last_log_likelihood)
				{
					step_to_it = true;
				}
				else
				{
					if(drand() < std::exp(new_log_likelihood - last_log_likelihood))
						step_to_it = true;
				}
				if(step_to_it)
				{
					last_log_likelihood = new_log_likelihood;
					current_in_params = test_in_params;

					// Check if we have a new best point
					if(out_param < best_out_param)
					{
						best_out_param = out_param;
						best_in_params = current_in_params;
					}
				}
			}

			// If we're on the second cycle, add to the recorded list of test points
			if(second_cycle)
			{
				if(IceBRG::divisible(step,skip_factor))
				{
					size_type c_i = (step-annealing_period)/skip_factor-1;
					auto col = _test_run_points_->col(c_i);
					for(size_type p_i=0; p_i<array_size; ++p_i)
					{
						col(p_i) = current_in_params(p_i);
					}
				}
			}

			// If we're on the last cycle, add to the mean
			if(last_cycle)
			{
				mean_in_params += current_in_params;
				last_cycle_count += 1;
			}

			if((step!=0)&&(IceBRG::divisible(step,annealing_period)))
			{
				if(first_cycle) // Special handling for end of first cycle
				{
					first_cycle = false;
					second_cycle = true;
				}
				else
				{
					second_cycle = false;
					annealing *= annealing_factor;

					// Recalculate likelihood
					last_log_likelihood *= annealing_factor;

					// Check if we're going into the last cycle
					if(max_steps-step<=annealing_period)
						last_cycle = true;
				}
			}
		} // for(int_type step = 0; step < max_steps; step++)

		// Calculate mean now
		mean_in_params = mean_in_params/IceBRG::safe_d(last_cycle_count);

		// Check if mean actually gives a better best
		try
		{
			out_param = func(mean_in_params);
			if(out_param < best_out_param)
			{
				best_in_params = mean_in_params;
				best_out_param = out_param;
			}
		}
		catch(const std::exception &e)
		{
			// Just leave it, no need to do anything
		}

		_best_fits_ = std::move(best_in_params);

		return *_best_fits_;
	}

	const array2d_type & get_test_run_points() const
	{
		if(!_test_run_points_) throw std::logic_error("Must solve before requesting result data.");
		return *_test_run_points_;
	}

	const array_type & get_means() const
	{
		if(!_means_)
		{
			const auto & test_points = get_test_run_points();
			size_type num_params = test_points.rows();
			size_type num_test_points = test_points.cols();

			_means_ = array_type(num_params);

			for(size_type p_i=0; p_i<num_params; ++p_i)
			{
				_means_->operator()(p_i) = test_points.row(p_i).mean();
			}
		}
		return *_means_;
	}
	const array_type & get_best_fits() const
	{
		if(!_best_fits_) throw std::logic_error("Must solve before requesting result data.");
		return *_best_fits_;
	}
	const array2d_type & get_covars() const
	{
		if(!_covars_)
		{
			const auto & means = get_means();
			size_type num_params = means.rows();
			const auto & test_points = get_test_run_points();
			assert(num_params==test_points.rows());

			_covars_ = array2d_type(num_params,num_params);

			for(size_type p1_i=0; p1_i<num_params; ++p1_i)
			{
				for(size_type p2_i=0; p2_i<num_params; ++p2_i)
				{
					_covars_->operator()(p1_i,p2_i) = ((test_points.row(p1_i)-means(p1_i))*(test_points.row(p2_i)-means(p2_i))).mean();
				}
			}

		}

		return *_covars_;
	}
	const array_type & get_stddevs() const
	{
		if(!_stddevs_)
		{
			const auto & covars = get_covars();
			size_type num_params = covars.rows();
			assert(covars.cols()==num_params);

			_stddevs_ = array_type(num_params);

			for(size_type p_i=0; p_i<num_params; ++p_i)
			{
				_stddevs_->operator()(p_i) = std::sqrt(covars(p_i,p_i));
			}

		}

		return *_stddevs_;
	}
};

} // namespace IceBRG

#endif // _BRG_BRG_MATH_SOLVERS_SOLVER_CLASSES_HPP_INCLUDED_
