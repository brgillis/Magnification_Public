/**********************************************************************\
 @file unitconv_map.hpp
 ------------------

 A simple typedef of IceBRG::unitconv_map.

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


// body file: unitconv_map.cpp


#ifndef _BRG_UNITCONV_MAP_HPP_INCLUDED_
#define _BRG_UNITCONV_MAP_HPP_INCLUDED_

#include <map>
#include <string>

#include "IceBRG_main/common.h"

namespace IceBRG
{

typedef std::map<std::string,flt_type> unitconv_map;

} // namespace IceBRG



#endif // _BRG_UNITCONV_MAP_HPP_INCLUDED_
