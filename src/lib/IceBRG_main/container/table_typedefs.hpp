/**********************************************************************\
 @file table_typedefs.hpp
 ------------------

 This header file contains typedefs used for handling data tables and
 their headers.

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


#ifndef _BRG_TABLE_TYPEDEFS_HPP_INCLUDED_
#define _BRG_TABLE_TYPEDEFS_HPP_INCLUDED_

#include <map>
#include <string>
#include <vector>

#include "IceBRG_main/container/insertion_ordered_map.hpp"

namespace IceBRG
{

// Typedefs
#if(1)

typedef std::vector< std::string > header_t;

template <typename T>
using table_t = std::vector< std::vector< T > >;

template <typename T, typename key_type=std::string>
using table_map_t = IceBRG::insertion_ordered_map<key_type,std::vector<T>>;

#endif

}

#endif // _BRG_TABLE_TYPEDEFS_HPP_INCLUDED_
