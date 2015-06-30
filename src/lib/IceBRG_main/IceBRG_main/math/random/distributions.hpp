/**********************************************************************\
  @file distributions.hpp

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

#ifndef _BRG_DISTRIBUTIONS_HPP_INCLUDED_
#define _BRG_DISTRIBUTIONS_HPP_INCLUDED_

#include <cmath>
#include <cstdlib>

#include "IceBRG_main/common.h"

#include "IceBRG_main/math/misc_math.hpp"

namespace IceBRG {

// Error function - forward to std::erf
template< typename Tx >
inline Tx erf(Tx && x)
{
	return std::erf(std::forward<Tx>(x));
}

template< typename Tx >
inline Tx erfc(Tx && x)
{
	return std::erfc(std::forward<Tx>(x));
}

// Gaussian PDF
template< typename Tx, typename Tmean, typename Tstddev >
inline flt_type Gaus_pdf( const Tx x, const Tmean mean,
		const Tstddev std_dev )
{
	return std::exp( -square( x - mean ) / ( 2 * square(std_dev) ) )
			/ ( std_dev * std::sqrt( 2 * pi ) );
}
template< typename Tx, typename Tmean >
inline flt_type Gaus_pdf( const Tx x, const Tmean mean )
{
	return Gaus_pdf(x,mean,1.);
}
template< typename Tx >
inline flt_type Gaus_pdf( const Tx x )
{
	return Gaus_pdf(x,0.,1.);
}

// Spherical Gaussian PDF
template< typename Tr, typename Tstddev >
inline flt_type spherical_Gaus_pdf( const Tr & r, const Tstddev & stddev )
{
	return std::exp(-square(r/stddev)/2.)/std::pow(2*pi*stddev*stddev,1.5);
}
template< typename Tr >
inline flt_type spherical_Gaus_pdf( const Tr & r )
{
	return spherical_Gaus_pdf(r,1.);
}

// Function to integrate a Gaussian from min to max
template< typename Tlo, typename Thi, typename Tmean, typename Tstddev >
inline flt_type Gaus_int( const Tlo min, const Thi max)
{
	return Gaus_int(min,max,0.,1.);
}
template< typename Tlo, typename Thi, typename Tmean>
inline flt_type Gaus_int( const Tlo min, const Thi max,
		const Tmean mean)
{
	return Gaus_int(min,max,mean,1.);
}
template< typename Tlo, typename Thi, typename Tmean, typename Tstddev >
inline flt_type Gaus_int( const Tlo min, const Thi max,
		const Tmean mean, const Tstddev std_dev )
{
	flt_type klo = ( min - mean ) / std_dev;
	flt_type khi = ( max - mean ) / std_dev;

	return std::fabs( erf( khi ) - ( klo ) ) / 2;
}

} // namespace IceBRG

#endif /* _BRG_DISTRIBUTIONS_HPP_INCLUDED_ */
