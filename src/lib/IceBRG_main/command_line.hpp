/**********************************************************************\
 @file command_line.hpp
 ------------------

 Various functions for processing command-line arguments.

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

#ifndef ICEBRG_MAIN_COMMAND_LINE_HPP_
#define ICEBRG_MAIN_COMMAND_LINE_HPP_

#include <functional>
#include <stdexcept>
#include <string>

#include <boost/lexical_cast.hpp>
#include <boost/type_index.hpp>

#include "common.h"

namespace IceBRG {

/**
 * Try to get a command-line argument, and throw an exception if it's not present.
 *
 * @param argc
 * @param argv
 * @param i Position of the argument to take (remember that cline args start at 1).
 * @return
 */
template< typename To >
inline To get_cline_arg(const int & argc, const char *argv[], const int & i)
{
	if(i>=argc)
	{
		throw std::runtime_error("Required command-line argument at position " +
				boost::lexical_cast<std::string>(i) + " is not present.");
	}

	try
	{
		return boost::lexical_cast<To>(argv[i]);
	}
	catch( const boost::bad_lexical_cast & )
	{
		throw std::runtime_error("Command-line argument in position " +
				boost::lexical_cast<std::string>(i) + " (" + argv[i] +
				") cannot be converted to type " + boost::typeindex::type_id<To>().pretty_name()
				+ ".");
	}
}

/**
 * Try to get a command-line argument, throw an exception if it's not present or if it fails
 * the provided test.
 *
 * @param argc
 * @param argv
 * @param i Position of the argument to take (remember that cline args start at 1).
 * @param test Test on the argument. Must take a value of the output type and return a bool, which
 *             will be true on a pass.
 * @return
 */
template< typename To >
inline To get_cline_arg(const int & argc, const char *argv[], const int & i,
		const std::function<bool(const To &)> & test)
{
	if(i>=argc)
	{
		throw std::runtime_error("Required command-line argument at position " +
				boost::lexical_cast<std::string>(i) + " is not present.");
	}

	To res;

	try
	{
		res = boost::lexical_cast<To>(argv[i]);
	}
	catch( const boost::bad_lexical_cast & )
	{
		throw std::runtime_error("Command-line argument in position " +
				boost::lexical_cast<std::string>(i) + " (" + argv[i] +
				") cannot be converted to type " + boost::typeindex::type_id<To>().pretty_name()
				+ ".");
	}

	if(test(res))
	{
		return res;
	}
	else
	{
		throw std::runtime_error("Command-line argument in position " +
				boost::lexical_cast<std::string>(i) + " (" + argv[i] +
				") fails test and cannot be used.");
	}
}

/**
 * Try to get a command-line argument, returns a default value if it isn't present.
 *
 * @param argc
 * @param argv
 * @param i Position of the argument to take (remember that cline args start at 1).
 * @param default_out Default return value if the argument isn't present.
 * @return
 */
template< typename To >
inline To get_cline_arg(const int & argc, const char *argv[], const int & i,
		const To & default_out)
{
	if(i>=argc)
	{
		return default_out;
	}

	try
	{
		return boost::lexical_cast<To>(argv[i]);
	}
	catch( const boost::bad_lexical_cast & )
	{
		throw std::runtime_error("Command-line argument in position " +
				boost::lexical_cast<std::string>(i) + " (" + argv[i] +
				") cannot be converted to type " + boost::typeindex::type_id<To>().pretty_name()
				+ ".");
	}
}

/**
 * Try to get a command-line argument, throw if it fails the provided test, if it's not present,
 * test possible default values.
 *
 * @param argc
 * @param argv
 * @param i Position of the argument to take (remember that cline args start at 1).
 * @param test Test on the argument. Must take a value of the output type and return a bool, which
 *             will be true on a pass.
 * @param default_out Default return value if the argument isn't present.
 * @return
 */
template< typename To >
inline To get_cline_arg(const int & argc, const char *argv[], const int & i,
		const std::function<bool(const To &)> & test, const To & default_out)
{
	if(i>=argc)
	{
		if(test(default_out))
		{
			return default_out;
		}
		else
		{
			throw std::runtime_error("No default command-line argument for position " +
					boost::lexical_cast<std::string>(i) + " passes the required test.");
		}
	}

	return get_cline_arg(argc,argv,i,test);
}

/**
 * Try to get a command-line argument, throw if it fails the provided test, if it's not present,
 * test possible default values.
 *
 * @param argc
 * @param argv
 * @param i Position of the argument to take (remember that cline args start at 1).
 * @param test Test on the argument. Must take a value of the output type and return a bool, which
 *             will be true on a pass.
 * @param default_out Default return value if the argument isn't present.
 * @return
 */
template< typename To, typename... Tos >
inline To get_cline_arg(const int & argc, const char *argv[], const int & i,
		const std::function<bool(const To &)> & test,
		const To & primary_default_out, const To & secondary_default_out,
		Tos... other_default_outs)
{
	if(i>=argc)
	{
		if(test(primary_default_out))
		{
			return primary_default_out;
		}
		else
		{
			return get_cline_arg<To>(argc,argv,i,test,secondary_default_out,other_default_outs...);
		}
	}

	return get_cline_arg(argc,argv,i,test);
}


} // namespace IceBRG



#endif // ICEBRG_MAIN_COMMAND_LINE_HPP_
