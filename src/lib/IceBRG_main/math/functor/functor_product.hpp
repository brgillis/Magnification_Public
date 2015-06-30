/**********************************************************************\
  @file functor_product.hpp

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

#ifndef _BRG_FUNCTOR_PRODUCT_HPP_INCLUDED_
#define _BRG_FUNCTOR_PRODUCT_HPP_INCLUDED_

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "IceBRG_main/common.h"

#include "IceBRG_main/vector/elementwise_functions.hpp"

namespace IceBRG
{

/** Class definitions **/
#if (1)

template< typename f1, typename f2, typename T=flt_type >
class functor_product
{
	/*****************************************************
	 functor_product
	 -----------------------------------------------------

	 An example of a functor which returns the
	 product of two other functors (see the
	 integrate_weighted* functions for
	 how this is used).

	 ****************************************************/
private:

	const f1 *_f1_ptr_;
	const f2 *_f2_ptr_;

public:
	// Constructors

	functor_product()
	{
		_f1_ptr_ = _f2_ptr_ = NULL;
	}

	functor_product( const f1 *new_f1, const f2 *new_f2 )
	{
		_f1_ptr_ = new_f1;
		_f2_ptr_ = new_f2;
	}
	functor_product( const f1 *new_f1, const f2 &new_f2 )
	{
		_f1_ptr_ = &new_f1;
		_f2_ptr_ = &new_f2;
	}
	functor_product( const f1 &new_f1, const f2 *new_f2 )
	{
		_f1_ptr_ = new_f1;
		_f2_ptr_ = new_f2;
	}
	functor_product( const f1 &new_f1, const f2 &new_f2 )
	{
		_f1_ptr_ = &new_f1;
		_f2_ptr_ = &new_f2;
	}

	// Virtual destructor
	virtual ~functor_product()
	{
	}

	// Set methods

	void set_f1_ptr( const f1* new_f1_ptr )
	{
		_f1_ptr_ = new_f1_ptr;
	}
	void set_f1_ptr( const f1 &new_f1_ptr )
	{
		_f1_ptr_ = &new_f1_ptr;
	}

	void set_f2_ptr( const f2 *new_f2_ptr )
	{
		_f2_ptr_ = new_f2_ptr;
	}
	void set_f2_ptr( const f2 &new_f2_ptr )
	{
		_f2_ptr_ = &new_f2_ptr;
	}

	void set_f1_f2_ptrs( const f1 *new_f1_ptr, const f2 *new_f2_ptr )
	{
		set_f1_ptr( new_f1_ptr );
		set_f2_ptr( new_f2_ptr );
	}
	void set_f1_f2_ptrs( const f1 *new_f1_ptr, const f2 &new_f2_ptr )
	{
		set_f1_ptr( new_f1_ptr );
		set_f2_ptr( new_f2_ptr );
	}
	void set_f1_f2_ptrs( const f1 &new_f1_ptr, const f2 *new_f2_ptr )
	{
		set_f1_ptr( new_f1_ptr );
		set_f2_ptr( new_f2_ptr );
	}
	void set_f1_f2_ptrs( const f1 &new_f1_ptr, const f2 &new_f2_ptr )
	{
		set_f1_ptr( new_f1_ptr );
		set_f2_ptr( new_f2_ptr );
	}

	// Function method

	decltype(multiply((*_f1_ptr_)( T() ),(*_f2_ptr_)( T() )))
		operator()( const T & in_param ) const
	{
		assert((_f1_ptr_!=NULL) && (_f2_ptr_!=NULL));

		return multiply((*_f1_ptr_)( in_param ),(*_f2_ptr_)( in_param ));
	}
}; // class functor_product< f1, f2, T>

#endif // end class declarations

} // end namespace IceBRG

#endif // __BRG_FUNCTOR_HPP_INCLUDED__
