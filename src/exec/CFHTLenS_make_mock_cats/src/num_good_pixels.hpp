/**********************************************************************\
 @file num_good_pixels.hpp
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

// body file: num_good_pixels.cpp

#ifndef _BRG_NUM_GOOD_PIXELS_HPP_INCLUDED_
#define _BRG_NUM_GOOD_PIXELS_HPP_INCLUDED_

#include <cassert>
#include <vector>

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



#endif // _BRG_NUM_GOOD_PIXELS_HPP_INCLUDED_
