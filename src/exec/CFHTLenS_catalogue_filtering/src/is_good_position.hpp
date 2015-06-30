/**********************************************************************\
 @file is_good_position.hpp
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


// body file: is_good_position.cpp


#ifndef _BRG_SRC_IS_GOOD_POSITION_HPP_INCLUDED_
#define _BRG_SRC_IS_GOOD_POSITION_HPP_INCLUDED_

#include <vector>

inline bool is_good_position(int i, int j, const std::vector<std::vector<bool>> & vec)
{
	if((i<0)||(j<0)) return false;
	if(static_cast<unsigned>(i)>=vec.size()) return false;
	if(static_cast<unsigned>(j)>=vec[0].size()) return false;

	return vec[i][j];
}



#endif // _BRG_SRC_IS_GOOD_POSITION_HPP_INCLUDED_
