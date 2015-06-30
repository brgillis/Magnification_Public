/**********************************************************************\
 @file get_good_positions.hpp
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

// body file: get_good_positions.cpp

#ifndef _BRG_SRC_GET_GOOD_POSITIONS_HPP_INCLUDED_
#define _BRG_SRC_GET_GOOD_POSITIONS_HPP_INCLUDED_

#include <cassert>

std::vector<std::vector<bool>> get_good_positions(const std::vector<std::vector<bool>> & input)
{
	std::vector<std::vector<bool>> result(input);

	unsigned ncol = input.size();
	assert(ncol>0);
	unsigned nrow = input[0].size();

	for(unsigned i=0; i<ncol; ++i)
	{
		for(unsigned j=0; j<nrow; ++j)
		{
			if(!result[i][j]) continue;
			result[i][j] = input[i-1][j-1] && input[i-1][j+1] && input[i+1][j-1] && input[i+1][j+1] &&
					input[i-1][j] && input[i][j-1] && input[i+1][j] && input[i][j+1];
		}
	}

	return result;
}

size_t num_good_pixels(const std::vector<std::vector<bool>> & input)
{
	size_t result(0);

	unsigned ncol = input.size();
	assert(ncol>0);
	unsigned nrow = input[0].size();

	for(unsigned i=0; i<ncol; ++i)
	{
		for(unsigned j=0; j<nrow; ++j)
		{
			if(input[i][j]) ++result;
		}
	}

	return result;
}



#endif // _BRG_SRC_GET_GOOD_POSITIONS_HPP_INCLUDED_
