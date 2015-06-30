/**********************************************************************\
  @file interpolator.h

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

// body file: brg/math/interpolator/interpolator.cpp

#ifndef _BRG_INTERPOLATOR_H_INCLUDED_
#define _BRG_INTERPOLATOR_H_INCLUDED_

#include <cstdlib>
#include <vector>
#include <stdexcept>

#include "IceBRG_main/common.h"

#include "IceBRG_main/utility.hpp"

#include "IceBRG_main/external/tk_spline.h"

namespace IceBRG {

bool p1first_lt_p2first(std::pair<flt_type,flt_type> pair1, std::pair<flt_type,flt_type> pair2);
bool p1first_lt_v2(std::pair<flt_type,flt_type> pair1, flt_type v2);

class interpolator {
public:

	enum allowed_interpolation_type {
		LOWER,
		UPPER,
		LINEAR,
		SPLINE
	}; // end enum allowed_interpolation_type

private:
#if (1)

	std::vector< std::pair<flt_type,flt_type> > _data_;

	mutable std::vector< std::pair<flt_type,flt_type> > _sorted_data_;
	mutable bool _sorted_data_cached_;

	mutable tk::spline _spline_;
	mutable bool _spline_cached_;

	static allowed_interpolation_type _default_interpolation_type_;
	allowed_interpolation_type _interpolation_type_;

	void _set_spline_points() const;

#endif // end private members and methods
public:
#if (1)

	// Swap functions
	void swap(interpolator &other);
	friend void swap(interpolator &same, interpolator &other) {same.swap(other);}

	// Constructors
	interpolator();
	interpolator(const interpolator &other);

	// Destructor
	virtual ~interpolator()
	{
	}

	// Operator=
	interpolator & operator=(interpolator other);

	// Clearing functions
	void clear();
	void clear_points();

	// Accessors to current and default interpolation types
	static const allowed_interpolation_type default_interpolation_type()
		{return _default_interpolation_type_;}
	const allowed_interpolation_type interpolation_type() const
		{return _interpolation_type_;}

	// Set functions for the current and default interpolation types
	static void set_default_interpolation_type(const allowed_interpolation_type new_default_type);
	void set_default_interpolation_type(const allowed_interpolation_type new_default_type,
			const bool override_current);
	void set_interpolation_type(const allowed_interpolation_type new_type);

	// This version doesn't check for duplicate x values, but if one does exist, an exception will
	// eventually be thrown
	void add_point(const flt_type & x, const flt_type & y);

	// This version checks if there's a point with a duplicate x value. If so, it throws an
	// exception
	void try_add_point(const flt_type & x, const flt_type & y);

	ssize_t size() const
	{
		return ssize(sorted_data());
	}

	bool empty() const
	{
		return (size()==0);
	}

	std::vector< std::pair<flt_type,flt_type> > & sorted_data() const;

	flt_type operator()(const flt_type & x) const;

#endif // public

}; // end class interpolator

} // end namespace IceBRG



#endif //__BRG_INTERPOLATOR_H_INCLUDED__
