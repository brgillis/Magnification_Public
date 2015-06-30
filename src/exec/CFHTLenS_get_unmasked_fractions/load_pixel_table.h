/**********************************************************************\
 @file load_pixel_table.h
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


// body file: load_pixel_table.cpp


#ifndef _BRG_SRC_LOAD_PIXEL_TABLE_H_INCLUDED_
#define _BRG_SRC_LOAD_PIXEL_TABLE_H_INCLUDED_

#include <string>
#include <vector>

std::string unpack_fits(const std::string & filename);
void delete_file(const std::string & filename);
std::vector<std::vector<bool>> load_pixel_table(const std::string & filename);


#endif // _BRG_SRC_LOAD_PIXEL_TABLE_H_INCLUDED_
