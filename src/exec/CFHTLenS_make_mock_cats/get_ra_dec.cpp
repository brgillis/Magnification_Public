/**********************************************************************\
 @file get_ra_dec.cpp
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

#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include <boost/lexical_cast.hpp>

#include "IceBRG_main/call_program.hpp"

#include "get_ra_dec.h"
#include "IceBRG_main/file_access/ascii_table.hpp"

const std::string data_directory = "/disk2/brg/git/CFHTLenS_cat/Data/";
const std::string mask_directory = data_directory + "masks/";
const std::string mask_file_root = ".fits.fz";
const std::string list_file_root = "_poslist.tmp";
const std::string temp_file_root = ".tmp";

const std::string get_ra_dec_command = "/disk2/brg/bin/xy2sky";
const std::string get_ra_dec_args = "-d -n 10 ";

std::vector<std::pair<std::pair<double,double>,std::pair<double,double>>>
	get_ra_dec(const std::string & field_name,
		const std::vector<std::pair<double,double>> & xp_yp)
{
	size_t num_pairs = 0;

	const std::string list_file_name = field_name + list_file_root;

	// Set up the list of pairs
	std::ofstream out;
	IceBRG::open_file(out,list_file_name);
	out.precision(10);
	for(const auto & pair : xp_yp)
	{
		out << pair.first << " " << pair.second << "\n";
		++num_pairs;
	}
	out.close();

	const std::string args = get_ra_dec_args + mask_directory + field_name + mask_file_root +
			" @" + list_file_name;

	// Call the program
	std::stringstream output(IceBRG::exec((get_ra_dec_command + " " + args).c_str()));

	const auto result_table = IceBRG::load_table<std::string>(output);

	// Delete the temp files
	IceBRG::call_program("/bin/rm", 1, "-f",list_file_name.c_str());

	std::vector<std::pair<std::pair<double,double>,std::pair<double,double>>> result;

	for(size_t i=0; i<num_pairs; ++i)
	{
		result.push_back(std::make_pair(std::pair<double,double>(
				std::stod(result_table[0][i]),
				std::stod(result_table[1][i])),
			std::pair<double,double>(
				std::stod(result_table[3][i]),
				std::stod(result_table[4][i]))));
	}

	return result;
}


