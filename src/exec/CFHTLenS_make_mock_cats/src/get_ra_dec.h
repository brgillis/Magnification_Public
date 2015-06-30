/**********************************************************************\
 @file get_ra_dec.h
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

// body file: get_ra_dec.cpp

#ifndef _BRG_GET_RA_DEC_H_INCLUDED_
#define _BRG_GET_RA_DEC_H_INCLUDED_

#include <string>
#include <utility>
#include <vector>

std::vector<std::pair<std::pair<double,double>,std::pair<double,double>>>
	get_ra_dec(const std::string & field_name,
		const std::vector<std::pair<double,double>> & xp_yp);

#endif // _BRG_GET_RA_DEC_H_INCLUDED_
