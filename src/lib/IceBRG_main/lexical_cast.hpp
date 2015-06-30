/**********************************************************************\
 @file lexical_cast.hpp
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


#ifndef _BRG_LEXICAL_CAST_HPP_INCLUDED_
#define _BRG_LEXICAL_CAST_HPP_INCLUDED_

#include <string>
#include <type_traits>

#include <boost/lexical_cast.hpp>

namespace IceBRG {

inline bool bool_cast(const std::string & s)
{
	try
	{
		return boost::lexical_cast<bool>(s);
	}
	catch(const boost::bad_lexical_cast &e)
	{
		// Check if the string is literally "true" or "false"
		if((s=="true")||(s=="True")||(s=="TRUE")) return true;
		if((s=="false")||(s=="False")||(s=="FALSE")) return false;

		// Can't figure it out, so rethrow.
		throw;
	}
}

// Try to cast, and output the lowest possible value if we can't
template<typename T_o, typename T_i>
T_o min_cast(const T_i & s)
{
	try
	{
		return boost::lexical_cast<T_o>(s);
	}
	catch(const boost::bad_lexical_cast &e)
	{
		// Check if it's an integral type
		if(std::is_integral<T_o>::value)
		{
			return std::numeric_limits<T_o>::min();
		}
		else
		{
			return -std::numeric_limits<T_o>::max();
		}
	}
}
// As above, but highest possible value if we can't
template<typename T_o, typename T_i>
T_o max_cast(const T_i & s)
{
	try
	{
		return boost::lexical_cast<T_o>(s);
	}
	catch(const boost::bad_lexical_cast &e)
	{
		return std::numeric_limits<T_o>::max();
	}
}

} // namespace IceBRG

#endif // _BRG_LEXICAL_CAST_HPP_INCLUDED_
