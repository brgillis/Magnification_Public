/**********************************************************************\
 @file Schechter_like_functor.h
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

// body file: Schechter_like_functor.cpp

#ifndef _BRG_SCHECHTER_LIKE_FUNCTOR_H_INCLUDED_
#define _BRG_SCHECHTER_LIKE_FUNCTOR_H_INCLUDED_

#include <cassert>
#include <vector>

#include "IceBRG_main/math/functor/functor.hpp"
#include "IceBRG_main/utility.hpp"

namespace IceBRG {

/**
 *
 */
class Schechter_like_functor: public IceBRG::functor<flt_type,flt_type,
	std::vector<flt_type>> {
private:
	constexpr static ssize_t _num_params_ = 7;

public:
	Schechter_like_functor()
	{
	}
	Schechter_like_functor(const std::vector<flt_type> & init_params )
	: functor(init_params)
	{
	}
	virtual ~Schechter_like_functor()
	{
	}

	// Params accessors
#if (1)

	long_flt_type N_scale() const
	{
		assert(ssize(params())==_num_params_);
		return params()[0];
	}
	long_flt_type m_star() const
	{
		assert(ssize(params())==_num_params_);
		return params()[1];
	}
	long_flt_type alpha() const
	{
		assert(ssize(params())==_num_params_);
		return params()[2];
	}
	long_flt_type mag_lower_lim_sharpness() const
	{
		assert(ssize(params())==_num_params_);
		return params()[3];
	}
	long_flt_type mag23_jump() const
	{
		assert(ssize(params())==_num_params_);
		return params()[4];
	}
	long_flt_type mag_upper_lim() const
	{
		assert(ssize(params())==_num_params_);
		return params()[5];
	}
	long_flt_type mag_upper_lim_sharpness() const
	{
		assert(ssize(params())==_num_params_);
		return params()[6];
	}

#endif

	// Function method

	long_flt_type operator()( const long_flt_type & in_params) const;

};

} // namespace IceBRG

#endif // _BRG_SCHECHTER_LIKE_FUNCTOR_H_INCLUDED_
