/**********************************************************************\
 @file ascii_table_map.hpp
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


#ifndef _BRG_ASCII_TABLE_MAP_HPP_INCLUDED_
#define _BRG_ASCII_TABLE_MAP_HPP_INCLUDED_

#include <iostream>
#include <map>
#include <vector>


#include "IceBRG_main/common.h"

#include "IceBRG_main/Eigen.hpp" // Just for RowMajor and ColMajor tags

#include "IceBRG_main/container/table_typedefs.hpp"
#include "IceBRG_main/file_access/ascii_table.hpp"
#include "IceBRG_main/file_access/table_utility.hpp"

namespace IceBRG {

// Print a table from a map
template<typename T>
void print_table_map( std::ostream & out,
		const table_map_t<T> & table_map)
{
	header_t header;
	table_t<T> data;

	for(auto it=table_map.begin(); it!=table_map.end(); ++it)
	{
		header.push_back(it->first);
		data.push_back(it->second);
	}
	print_table<T>(out,data,header,Eigen::ColMajor);
}

// And to allow us to print to a file name instead of a stream
template<typename T>
inline void print_table_map( const std::string & file_name,
		const table_map_t<T> & table_map)
{
	std::ofstream fo;
	open_file_output(fo,file_name);

	print_table_map<T>(fo,table_map);
}

// Directly load a map of the data
template<typename T>
table_map_t<T> load_table_map( std::istream & fi,
		const T default_value=T())
{
	header_t header = load_header(fi);
	table_t<T> data = load_table<T>(fi,Eigen::ColMajor,default_value);
	return make_table_map<T>(data,header);
}

// And to allow us to load from a file name instead of a stream
template<typename T>
table_map_t<T> load_table_map( const std::string & file_name,
		const T default_value=T() )
{
	std::ifstream fi;
	open_file_input(fi,file_name);

	return load_table_map<T>(fi,default_value);
}
template<typename T>
void load_table_columns( std::istream & fi,
		std::map< std::string, std::vector<T>* > & column_map,
		const bool case_sensitive=false, const T default_value=T())
{
	table_map_t<T> table_map = load_table_map<T>(fi,default_value);

	for(typename std::map< std::string, std::vector<T>* >::iterator it=column_map.begin();
			it!=column_map.end(); ++it)
	{
		*(it->second) = table_map[it->first];

		// Check that we found it
		if(it->second->size()==0)
		{
			// TODO output warning to logger here
		}

	}
}

// And to allow us to load from a file name instead of a stream
template<typename T>
void load_table_columns( const std::string & file_name,
		std::map< std::string, std::vector<T>* > & column_map,
		const bool case_sensitive=false, const T default_value=T())
{
	std::ifstream fi;
	open_file_input(fi,file_name);

	load_table_columns<T>(fi,column_map,case_sensitive,default_value);
}

} // namespace IceBRG

#endif // _BRG_ASCII_TABLE_MAP_HPP_INCLUDED_
