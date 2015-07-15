/**********************************************************************\
  @file trim_comments.hpp

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

// body file: brg/file_access/file_functions.cpp

#ifndef _BRG_FILE_FUNCTIONS_H_INCLUDED_
#define _BRG_FILE_FUNCTIONS_H_INCLUDED_

#include <fstream>
#include <string>

#include "IceBRG_main/common.h"

namespace IceBRG
{

/** Global function declarations **/
#if (1)

// Functions to get rid of comments lines (those starting with #) from open fstreams and ifstreams. The "one_line" versions will
// trim the next line if it's commented out, and the "all_at_top" versions will trim all commented lines from the current
// position until they find a line which isn't a comment.
// The "one_line" functions will return 1 if the file is already at the end, and 0 otherwise.
// The "all_at_top" functions will return 1 if they reach the end of the file before the run out of comments, and 0 otherwise.
template <typename streamtype>
void trim_comments_one_line( streamtype & stream )
{
	std::string file_data;
	if ( stream )
	{
		if ( stream.peek() == (int_type)( *"#" ) )
			getline( stream, file_data );
	}
}
template <typename streamtype>
void trim_comments_all_at_top( streamtype & stream )
{
	std::string file_data;
	while ( stream )
	{
		if ( stream.peek() == (int_type)( *"#" ) )
		{
			getline( stream, file_data );
		}
		else
		{
			return;
		}
	}
}

#endif // End global function declarations

}

#endif // __BRG_FILE_FUNCTIONS_H_INCLUDED__
