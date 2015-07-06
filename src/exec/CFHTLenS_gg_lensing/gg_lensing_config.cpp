/**********************************************************************\
 @file gg_lensing_config.cpp
 ------------------

 Corresponding source file for gg_lensing_config.h - see that file for
 documentation.

 **********************************************************************

 Copyright (C) 2014, 2015  Bryan R. Gillis

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

#include <fstream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

#include "IceBRG_main/error_handling.h"
#include "IceBRG_main/file_access/open_file.hpp"
#include "IceBRG_main/file_access/trim_comments.hpp"
#include "IceBRG_main/lexical_cast.hpp"
#include "IceBRG_main/units/unit_conversions.hpp"
#include "IceBRG_main/units/units.hpp"

#include "gg_lensing_config.h"

gg_lensing_config::gg_lensing_config( const int argc, const char *argv[] )
{
	using namespace IceBRG;
	using namespace IceBRG::unitconv;

	if(argc<3)
	{
		handle_notification(
				std::string("CFHTLenS_gg_lensing normally takes a configuration file as the second\n") +
				"command-line argument. None was detected, so it will use default values.\n");

		use_precalculated_data = false;
		precalculated_data_filename = "";

		R_min = 0*kpctom*m;
		R_max = 2000*kpctom*m;
		R_bins = 20;
		R_log = false;

		m_min = 1e9*Msuntokg*kg;
		m_max = 1e12*Msuntokg*kg;
		m_bins = 3;
		m_log = true;

		z_min = 0.2;
		z_max = 1.3;
		z_bins = 11;
		z_log = false;

		mag_min = -std::numeric_limits<double>::infinity();
		mag_max = 25;
		mag_bins = 1;
		mag_log = false;
	}
	else
	{
		// Open the config file
		std::string filename(argv[2]);
		std::ifstream fi;
		IceBRG::open_file_input(fi,filename);

		// Set up a vector to store config values in
		std::vector<std::string> config_value_strings(num_config_params);

		auto get_config_value = [] (std::istream & fi)
		{
			std::string line_buffer, word_buffer;
			std::istringstream line_data_stream;

			IceBRG::trim_comments_all_at_top(fi);

			do
			{
				if(!fi)
					throw std::runtime_error("End of config file reached before all values read in.\n");
				std::getline( fi, line_buffer );
			} while(line_buffer.size()==0);
			line_data_stream.str(line_buffer);
			do
			{
				line_data_stream >> word_buffer;
			} while(line_data_stream);
			return word_buffer;
		};

		for(std::string & val : config_value_strings)
		{
			val = get_config_value(fi);
		}

		// Load in the values
		size_t i=0;
		use_precalculated_data = IceBRG::bool_cast(config_value_strings.at(i++));
		precalculated_data_filename = config_value_strings.at(i++);

		R_min = IceBRG::min_cast<double>(config_value_strings.at(i++))*kpctom*m;
		R_max = IceBRG::max_cast<double>(config_value_strings.at(i++))*kpctom*m;
		R_bins = boost::lexical_cast<size_t>(config_value_strings.at(i++));
		R_log = IceBRG::bool_cast(config_value_strings.at(i++));

		m_min = IceBRG::min_cast<double>(config_value_strings.at(i++))*Msuntokg*kg;
		m_max = IceBRG::max_cast<double>(config_value_strings.at(i++))*Msuntokg*kg;
		m_bins = boost::lexical_cast<size_t>(config_value_strings.at(i++));
		m_log = IceBRG::bool_cast(config_value_strings.at(i++));

		z_min = IceBRG::min_cast<double>(config_value_strings.at(i++));
		z_max = IceBRG::max_cast<double>(config_value_strings.at(i++));
		z_bins = boost::lexical_cast<size_t>(config_value_strings.at(i++));
		z_log = IceBRG::bool_cast(config_value_strings.at(i++));

		mag_min = IceBRG::min_cast<double>(config_value_strings.at(i++));
		mag_max = IceBRG::max_cast<double>(config_value_strings.at(i++));
		mag_bins = boost::lexical_cast<size_t>(config_value_strings.at(i++));
		mag_log = IceBRG::bool_cast(config_value_strings.at(i++));

	}
}
