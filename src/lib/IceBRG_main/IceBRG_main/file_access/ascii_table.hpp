/**********************************************************************\
 @file ascii_table.h
 ------------------

 This header file contains definitions of various functions used to
 read from and write to ASCII data tables.

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

#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "IceBRG_main/common.h"

#include "IceBRG_main/Eigen.hpp" // Just for RowMajor and ColMajor tags

#include "IceBRG_main/container/table_typedefs.hpp"

#include "IceBRG_main/file_access/open_file.hpp"
#include "IceBRG_main/file_access/table_utility.hpp"
#include "IceBRG_main/file_access/trim_comments.hpp"

#include "IceBRG_main/math/misc_math.hpp"

#include "IceBRG_main/utility.hpp"

#include "IceBRG_main/vector/manipulations.hpp"


#ifndef _BRG_ASCII_TABLE_H_INCLUDED_
#define _BRG_ASCII_TABLE_H_INCLUDED_

namespace IceBRG {

// Prints a formatted table in the passed stream. header is a vector of strings representing the labels for each column,
// and data is a 2-d vector of the data to be printed, in the format data[c][r], where c is the column index and r is the row index.
// Some templates to coerce any table of data to be printed out
template<typename T>
void print_table( std::ostream & out_stream,
		const table_t<T> & data,
		const header_t & header = header_t(),
		const char major_tag = Eigen::ColMajor,
		const char precision = 10)
{
	// Set up the value we'll print if an entry is bad
	std::stringstream ss("");
	ss << std::numeric_limits<flt_type>::max();
	std::string bad_value = ss.str();
	ss.clear();
	ss.str("");

	// Set the precision of this stringstream
	ss.precision(precision);

	size_t num_col;
	size_t num_row;

	if(major_tag==Eigen::ColMajor)
	{
		num_col = ssize(data);
		num_row = ssize(data.at(0));
	}
	else
	{
		num_row = ssize(data);
		num_col = ssize(data.at(0));
	}

	std::vector< size_t > width(num_col,0);

	const bool skip_header = (ssize(header)==0);

	try
	{
		bool add_comment_marker = false;

		// First, we loop through to get the maximum width of each column
		// Check the header first
		if(!skip_header)
		{
			for ( size_t c = 0; c < num_col; c++ )
			{
				if(c==0)
				{
					// For the first header element, we'll check if it starts with a
					// '#'. If it doesn't, we'll add one to it.
					ss.str(header[c]);
					if ( ss.peek() != (int_type)( *"#" ) )
					{
						add_comment_marker = true;
					}
				}
				if ( header[c].length() > width[c] )
				{
					width[c] = header[c].length();
				}
			} // for( int_type c = 0; c < num_columns; c++ )
		}

		// Now loop through the data
		for ( size_t i = 0; i < num_row; i++ )
		{
			for ( size_t c = 0; c < num_col; c++ )
			{
				T val;
				if(major_tag==Eigen::ColMajor)
				{
					val = data[c].at(i);
				}
				else
				{
					val = data[i].at(c);
				}

				std::string str_val;
				if(isbad(val))
				{
					str_val = bad_value;
				}
				else
				{
					ss.str("");
					ss << val;
					str_val = ss.str();
				}
				if ( str_val.length() > width[c] )
				{
					width[c] = str_val.length();
				}
			} // for( int_type c = 0; c < num_columns; c++ )
		} // for( int_type i = 0; i < num_rows; i++ ) (testing width)

		if(add_comment_marker)
		{
			// Increase all widths by 1 to ensure spacing
			for ( size_t c = 0; c < num_col; c++ )
				width[c] += 1;
		}
		else
		{
			// Increase all widths except the first by 1 to ensure spacing
			for ( size_t c = 1; c < num_col; c++ )
				width[c] += 1;
		}

		// Output the header
		if ( !skip_header )
		{
			if(add_comment_marker)
			{
				out_stream << "# ";
			}
			for ( size_t c = 0; c < num_col; c++ )
				out_stream << std::setfill( ' ' ) << std::setw( width[c] ) << header[c];
		}

		out_stream << std::endl;

		// Output the data
		for ( size_t i = 0; i < num_row; i++ )
		{
			if(add_comment_marker)
			{
				out_stream << "  ";
			}
			for ( size_t c = 0; c < num_col; c++ )
			{
				T val;
				if(major_tag==Eigen::ColMajor)
				{
					val = data[c].at(i);
				}
				else
				{
					val = data[i].at(c);
				}

				std::string str_val;
				if(isbad(val))
				{
					str_val = bad_value;
				}
				else
				{
					ss.str("");
					ss << val;
					str_val = ss.str();
				}
				out_stream << std::setfill( ' ' ) << std::setw( width[c] ) << str_val;
			}
			out_stream << std::endl;
		}

	}
	catch ( const std::out_of_range &e )
	{
		throw std::runtime_error((std::string)"ERROR: Could not print table. Check that the data is properly formatted\n"
					+ "at least num_columns length for header and first index of data, and at\n"
					+ "least num_rows length for all vectors contained within data.\n");
	}
}

template<typename T>
void print_table( const std::string & file_name,
		const table_t<T> & data,
		const header_t & header = header_t(),
		const char major_tag = Eigen::ColMajor)
{
	std::ofstream fo;
	open_file_output(fo,file_name);

	print_table<T>(fo,data,header,major_tag);
}

// Load in entire table
template<typename T>
table_t<T> load_table( std::istream & fi,
						const char major_tag = Eigen::ColMajor,
						const T & default_value=T(),
						size_t min_length = 0)
{
	table_t<T> table_data;

	// Trim the header
	trim_comments_all_at_top(fi);

	// Clear the output vector
	table_data.resize(0);

	std::string line_data;
	std::istringstream line_data_stream;
	while ( getline(fi, line_data) )
	{
		line_data_stream.clear();
		line_data_stream.str(std::move(line_data));

		auto temp_vector = split_line<T>(line_data_stream,default_value,min_length);

		if(min_length==0) min_length = ssize(temp_vector);

		table_data.push_back(std::move(temp_vector));
	}

	if(major_tag == Eigen::ColMajor)
		return transpose(table_data,default_value);
	else
		return table_data;
}

// And to allow us to load from a file name instead of a stream
template<typename T>
inline table_t<T> load_table( const std::string & file_name,
								const char major_tag = Eigen::ColMajor,
								const T & default_value=T())
{
	std::ifstream fi;
	open_file_input(fi,file_name);

	return load_table<T>(fi,major_tag,default_value);
}

// Load a table's header as a vector of strings
inline header_t load_header( std::istream & table_stream )
{
	std::string temp_line;
	std::vector<header_t> possible_headers;

	// Get all comment lines at the top of the file
	while ( table_stream )
	{
		if ( table_stream.peek() == (int_type)( *"#" ) )
		{
			getline( table_stream, temp_line );

			header_t temp_header = convert_to_header(temp_line);

			if(ssize(temp_header) > 0)
				possible_headers.push_back(temp_header);
		}
		else
		{
			break;
		}
	}

	if(ssize(possible_headers)==1) return possible_headers[0];
	if(ssize(possible_headers)==0) return header_t();

	// If we get here, more than one line is a possible header candidate. Our next step is to
	// go to the data and count the columns in the first line. If only one possible header has
	// the right length, we know that's the one.

	int_type n_cols = 0;
	do
	{
		getline( table_stream, temp_line );
		std::string junk;
		std::istringstream line_data_stream(temp_line);
		while (line_data_stream >> junk)
		{
			++n_cols;
		}
	} while(n_cols==0 && table_stream);

	if(n_cols==0) // If we can't find any data
	{
		return header_t();
	}

	// Search through the possible headers, and see if we find exactly one with the right size
	int_type num_right_size = 0;
	ssize_t i_best = 0;
	for(ssize_t i=0; i<ssize(possible_headers); ++i)
	{
		if(ssize(possible_headers[i])==n_cols)
		{
			++num_right_size;
			i_best = i;
		}
	}

	if(num_right_size != 1) // If multiple or zero lines are the right size
	{
		return header_t();
	}

	return possible_headers[i_best];
}

// And to allow us to load from a file name instead of a stream
inline header_t load_header( const std::string & file_name )
{
	std::ifstream fi;
	open_file_input(fi,file_name);

	return load_header(fi);
}

} // namespace IceBRG

#endif // _BRG_ASCII_TABLE_H_INCLUDED_
