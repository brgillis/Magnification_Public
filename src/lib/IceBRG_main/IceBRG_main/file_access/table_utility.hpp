/**********************************************************************\
 @file table_utility.hpp
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


#ifndef _BRG_TABLE_UTILITY_H_INCLUDED_
#define _BRG_TABLE_UTILITY_H_INCLUDED_

#include <cstdlib>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

#include "IceBRG_main/common.h"

#include "IceBRG_main/container/table_typedefs.hpp"
#include "IceBRG_main/utility.hpp"
#include "IceBRG_main/vector/make_vector.hpp"

namespace IceBRG {

// Merge a header and data table into a map
template<typename T>
inline table_map_t<T> make_table_map(
		const table_t<T> & data,
		const header_t & header)
{
	table_map_t<T> result;

	auto h_size = ssize(header);
	auto d_size = ssize(data);

	const header_t *header_to_use = &header;
	header_t new_header;
	const table_t<T> *data_to_use = &data;
	table_t<T> new_data;

	// First, check if the header and data aren't the same size
	if(h_size<d_size)
	{
		// We'll pad the header with default values
		new_header = header;
		new_header.resize(d_size);
		for(auto i=h_size; i<d_size; ++i)
		{
			std::stringstream ss("col_");
			ss << i+1;
			new_header[i] = ss.str();
		}
		header_to_use = &new_header;
		h_size = d_size;
	}
	else if(d_size<h_size)
	{
		// We'll pad the data with default values
		new_data = data;
		new_data.resize(h_size);
		if(d_size>0)
		{
			// If we have some data, match the size of it in new columns
			for(auto i=d_size; i<h_size; ++i)
			{
				make_vector_default(new_data[i],new_data[0].size());
			}
		}
		data_to_use = &new_data;
		d_size = h_size;
	}

	for(decltype(h_size) i=0; i<h_size; ++i)
	{
		result[(*header_to_use)[i]] = (*data_to_use)[i];
	}
	return result;
}

// Splits a string into a vector of "word" strings on whitespace
inline std::vector< std::string > split_on_whitespace( const std::string & sentence )
{
	std::vector< std::string > result;
	std::istringstream sentence_data_stream(sentence);

	std::string word;
	while (sentence_data_stream >> word)
	{
		result.push_back(word);
	}

	return result;
}
inline header_t convert_to_header( const std::string & line )
{
	header_t result;
	std::istringstream line_data_stream(line);

	std::string word;

	// Get rid of first word if it's the comment indicator
	if ( line_data_stream.peek() == (int_type)( *"#" ) )
	{
		line_data_stream >> word;
	}

	while (line_data_stream >> word)
	{

		result.push_back(word);
	}

	return result;
}

template< typename T_out, typename T_in,
	typename std::enable_if<std::is_same<T_out,std::string>::value,char>::type = 0 >
inline T_in & pop_from_istream(T_in & in, T_out & out)
{
	in >> out;
	return in;
}

template< typename T_out, typename T_in,
	typename std::enable_if<!std::is_same<T_out,std::string>::value,char>::type = 0 >
inline T_in & pop_from_istream(T_in & in, T_out & out)
{
	in >> out;
	if(in.fail())
	{
		// If we fail (but NOT for eof), clear the error state, and try to pop to a junk value
		in.clear();
		std::string junk;
		in >> junk;
	}
	return in;
}

template< typename T_out, typename T_in >
inline std::vector<T_out> split_line(T_in && line_data, const T_out & default_value=T_out(), const size_t & min_length = 0)
{
	std::vector<T_out> res;
	res.resize(min_length,default_value);

	// Split the line on whitespace

	// Load up the minimum portion first
	for( auto & value : res)
	{
		pop_from_istream(line_data,value);
	}

	// Load up values beyond minimum
	do
	{
		T_out value(default_value);
		if(!pop_from_istream(line_data,value)) break;
		res.push_back(value);
	} while(true);

	return res;
}

} // namespace IceBRG

#endif // _BRG_TABLE_UTILITY_H_INCLUDED_
