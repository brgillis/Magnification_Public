/**********************************************************************\
 @file join_path.hpp
 ------------------

 A function to join multiple strings in a path, with exactly one '/'
 between each block.

 **********************************************************************

 Copyright (C) 2015 brg

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

#ifndef JOIN_PATH_HPP_
#define JOIN_PATH_HPP_

#include <boost/algorithm/string/replace.hpp>

namespace IceBRG {

/**
 * Replace all double and triple slashes in a string with single slashes.
 * @param s
 */
inline void reduce_slashes(std::string & s)
{
	boost::replace_all(s,"///","/"); // Cut down triple slashes to single
	boost::replace_all(s,"//","/"); // Cut down double slashes to single
}

/**
 * Concatenates two strings.
 * @param s1
 * @param s2
 * @return
 */
inline std::string join_strings(const std::string & s1, const std::string & s2)
{
	return std::string(s1 + s2);
}

/**
 * Concatenates three or more strings
 * @param s1
 * @param s2
 * @param s3
 * @param so
 * @return
 */
template<typename... To>
inline std::string join_strings(const std::string & s1, const std::string & s2,
		const std::string & s3, To... so)
{
	return join_strings(join_strings(s1,s2),s3,so...);
}

/**
 * Joins two strings together with exactly one slash between them, regardless of whether
 * s1 begins with a slash and/or s2 ends with one.
 * @param s1
 * @param s2
 * @return
 */
inline std::string join_path(const std::string & s1, const std::string & s2)
{
	std::string res(s1 + "/" + s2); // Combine with an extra slash

	reduce_slashes(res); // Get rid of duplicate slashes

	return res;
}

/**
 * Joins multiple strings together with exactly one slash between each pair, regardless of whether
 * they begin or end with slashes.
 * @param s1
 * @param s2
 * @param s3
 * @param so
 * @return
 */
template<typename... To>
inline std::string join_path(const std::string & s1, const std::string & s2,
		const std::string & s3, To... so)
{
	const std::string s12(s1 + "/" + s2); // Combine with an extra slash

	return join_path(s12,s3,so...); // Recurse down
}

}

#endif // JOIN_PATH_HPP_
