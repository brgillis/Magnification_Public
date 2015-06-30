/**********************************************************************\
 @file error_handling.h
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

#ifndef BRG_ERROR_HANDLING_H_
#define BRG_ERROR_HANDLING_H_

#include <iostream>
#include <stdexcept>
#include <string>

namespace IceBRG {

/// Error behavior enum
enum class error_behavior_type
{
	THROW,
	WARN,
	LOG,
	NOTHING
};

extern error_behavior_type error_behavior;

/// Handle an error message
inline void handle_error(const std::string & str)
{
	switch (error_behavior) {
		case error_behavior_type::THROW:
			throw std::runtime_error(str);
			break;
		case error_behavior_type::WARN:
			std::cerr << "WARNING: " << str << std::endl;
			break;
		case error_behavior_type::LOG:
			std::cerr << "WARNING: " << str << std::endl; // TODO Implement logger
			break;
		case error_behavior_type::NOTHING:
			break;
		default:
			break;
	}
}

/// Handle a notification
inline void handle_notification(const std::string & str)
{
	switch (error_behavior) {
		case error_behavior_type::THROW:
			std::cout << str << std::endl;
			break;
		case error_behavior_type::WARN:
			std::cout << str << std::endl;
			break;
		case error_behavior_type::LOG:
			std::cout << str << std::endl; // TODO Implement logger
			break;
		case error_behavior_type::NOTHING:
			break;
		default:
			break;
	}
}

/// Handle an error message when throwing isn't an option
inline void handle_error_message(const std::string & str)
{
	switch (error_behavior) {
		case error_behavior_type::THROW:
			std::cerr << "WARNING: " << str << std::endl;
			break;
		case error_behavior_type::WARN:
			std::cerr << "WARNING: " << str << std::endl;
			break;
		case error_behavior_type::LOG:
			std::cerr << "WARNING: " << str << std::endl; // TODO Implement logger
			break;
		case error_behavior_type::NOTHING:
			break;
		default:
			break;
	}
}

}

#endif // BRG_ERROR_HANDLING_H_
