/**********************************************************************\
 @file call_program.hpp
 ------------------

 This header defines functions to call another program as if through
 the command-line.

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

#ifndef _BRG_CALL_PROGRAM_HPP_INCLUDED_
#define _BRG_CALL_PROGRAM_HPP_INCLUDED_

#include <stdexcept>
#include <string>
#include <unistd.h>
#include <wait.h>

#include <boost/lexical_cast.hpp>

#include "IceBRG_main/common.h"

namespace IceBRG{

template<typename... Args>
int_type call_program_noexcept(const char *program_name, int_type num_retries, Args... args)
{
	bool good_result=false;
	int_type try_count = 0;
	pid_t pid;
	while(!good_result)
	{
		good_result = true;
		// Spawn a subprocess to delete the unpacked file
		#ifdef _OPENMP
		#pragma omp critical(call_program_noexcept)
		#endif
		{
			pid = fork();
		}
		switch (pid)
		{
			case -1:
				if(try_count>num_retries)
					return -1;
				sleep(5);
				good_result = false;
				break;
			case 0:
				/* This is processed by the child */
				execl(program_name, program_name, args..., nullptr);
				_exit(1);
				good_result = false;
				break;
			default:
				/* This is processed by the parent */
				++try_count;
				int_type status=0;
				waitpid(pid,&status,0);
				if(status)
				{
					if(try_count>num_retries)
						return status;
					good_result = false;
				break;
			}
		}
	}
	return 0;
}

inline int_type call_program_noexcept(const char *program_name)
{
	return call_program_noexcept(program_name,0);
}

template<typename... Args>
void call_program(const char *program_name, int_type num_retries, Args... args)
{
	int_type result=call_program_noexcept(program_name,num_retries,args...);
	if(result>0)
		throw std::runtime_error("Child in call_program returned error status "
			+ boost::lexical_cast<std::string>(result) + "\n.");
	if(result<0)
		throw std::runtime_error("Could not fork new program in call_program.\n");
	return;
}

inline std::string exec(const char* cmd) {
    FILE* pipe = popen(cmd, "r");
    if (!pipe) return "ERROR";
    char buffer[128];
    std::string result = "";
    while(!feof(pipe)) {
    	if(fgets(buffer, 128, pipe) != NULL)
    		result += buffer;
    }
    pclose(pipe);
    return result;
}


} // end namespace IceBRG

#endif // _BRG_CALL_PROGRAM_HPP_INCLUDED_
