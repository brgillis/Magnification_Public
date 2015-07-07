/**********************************************************************\
 @file get_data_directory.hpp
 ------------------

 TODO <Insert file description here>

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

#ifndef GET_DATA_DIRECTORY_HPP_
#define GET_DATA_DIRECTORY_HPP_

#include <memory>
#include <string>

#include "IceBRG_main/command_line.hpp"
#include "IceBRG_main/error_handling.h"
#include "IceBRG_main/join_path.hpp"
#include "IceBRG_main/file_access/open_file.hpp"


const std::string primary_default_data_directory = "./Data/";
const std::string secondary_default_data_directory = "../Data/";

inline std::string get_data_directory(const int & argc, const char *argv[],
		std::ifstream *fi)
{

	using namespace IceBRG;

	// To do this, we'll use a default if something wasn't passed in the command-line.
	// First, set up a test to see if a path works. We'll load the fields list while we
	// do this.

	const auto test_data_path = [&] (const std::string & path)
	{
		const std::string fields_list = join_path(path,"fields_list.txt");
		try
		{
			open_file_input(*fi,fields_list);
			return true;
		}
		catch(const std::runtime_error &)
		{
			return false;
		}
	};

	std::string data_directory;

	try
	{
		data_directory = get_cline_arg<std::string>(argc,argv,1,test_data_path,
				primary_default_data_directory,
				secondary_default_data_directory);
	}
	catch(const std::runtime_error &)
	{
		handle_error(std::string("Error: Data directory not passed at command line (as first argument),")
				+ "and it cannot be deduced.");
		throw;
	}

	handle_notification("Using " + data_directory + " as Data directory.\n");

	return data_directory;

}

inline std::string get_data_directory(const int & argc, const char *argv[])
{
	std::unique_ptr<std::ifstream> fi(new std::ifstream);
	return get_data_directory(argc,argv,fi.get());
}

inline std::string get_data_directory(const int & argc, const char *argv[],
		std::ifstream & fi)
{
	return get_data_directory(argc,argv,&fi);
}



#endif // GET_DATA_DIRECTORY_HPP_
