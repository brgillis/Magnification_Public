/**********************************************************************\
 @file functor.hpp
 ------------------

 Abstract base class for a functor. Inherit from this to ensure
 compatibility with all functions in the library.

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


#ifndef _BRG_FUNCTOR_HPP_INCLUDED_
#define _BRG_FUNCTOR_HPP_INCLUDED_

#include <utility>

#include "IceBRG_main/common.h"

namespace IceBRG {

/**
 * An abstract base class representing a functor which can store parameters in a "param_struct" object.
 */
template<typename Tin, typename Tout, typename param_struct=char> // Defaults to using the minimum size for the param structure
class functor
{
private:

	param_struct _params_;

public:

	functor()
	{
	}

	functor(param_struct init_params)
	: _params_(init_params)
	{
	}

	virtual ~functor()
	{
	}

	template<typename other_param_struct>
	void set_params(other_param_struct&& new_params)
	{
		_params_ = std::forward<other_param_struct>(new_params);
	}

	const param_struct & params() const
	{
		return _params_;
	}

	virtual Tout operator()(const Tin & in_param) const =0;

};

} // namespace IceBRG

#endif // _BRG_FUNCTOR_HPP_INCLUDED_
