/**********************************************************************\
  @file galaxy_group.cpp

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

#include <cstdio>
#include <stdexcept>
#include <vector>

#include "IceBRG_main/common.h"

#include "IceBRG_main/error_handling.h"
#include "IceBRG_physics/sky_obj/galaxy.h"

#include "galaxy_group.h"

// IceBRG::galaxy_group class methods
#if (1)

// Constructor
IceBRG::galaxy_group::galaxy_group( size_t init_num_members )
{
	if ( init_num_members < 0 )
		init_num_members = 0;

	z_phot = z_phot_err = 0;
	odds = 1;

	BCG_index = 0;

	num_members = init_num_members;
	if ( num_members > 0 )
	{
		member_indices.reserve( num_members );
		members.reserve( num_members );
	}
	for ( size_t i = 0; i < num_members; i++ )
	{
		member_indices[i] = -1;
		members[i] = 0;
	}
}

// Destructor
IceBRG::galaxy_group::~galaxy_group()
{
}

// Other functions
void IceBRG::galaxy_group::clear()
{
	z_phot = z_phot_err = 0;
	odds = 1;

	BCG_index = 0;

	num_members = 0;
	if ( num_members > 0 )
	{
		member_indices.reserve( num_members );
		members.reserve( num_members );
	}
	for ( size_t i = 0; i < num_members; i++ )
	{
		member_indices[i] = -1;
		members[i] = 0;
	}

	sky_obj::clear();
}

void IceBRG::galaxy_group::resize( size_t new_num_members )
{
	member_indices.resize( new_num_members, -1 );
	members.resize( new_num_members, 0 );
	num_members = new_num_members;
}

void IceBRG::galaxy_group::set_member( size_t member_index, galaxy * new_member )
{
	if ( member_index >= num_members )
	{
		throw std::out_of_range("Group member index out of range.");
	}
	members[member_index] = new_member;
	member_indices[member_index] = new_member->index();
}
void IceBRG::galaxy_group::set_member_index( size_t member_index,
		size_t new_member_index )
{
	if ( member_index >= num_members )
	{
		throw std::out_of_range("ERROR: Member index out of bounds in galaxy_group::set_member_index\n");
	}
	member_indices[member_index] = new_member_index;
}
void IceBRG::galaxy_group::add_member( galaxy * new_member )
{
	int_type new_num_members = num_members + 1;
	resize( new_num_members );
	members[new_num_members - 1] = new_member;
	member_indices[new_num_members - 1] = new_member->index();
}
void IceBRG::galaxy_group::remove_member( galaxy * rem_member )
{
	size_t i;
	for ( i = 0; i < num_members; i++ )
	{
		if ( members[i]->ID() == rem_member->ID() )
			break;
	}
	if ( i == num_members ) // Then we didn't find it
	{
		handle_error("Could not find member to remove from galaxy_group.");
		return;
	}
	for ( size_t j = i; j < num_members - 1; j++ )
	{
		members[j] = members[j + 1];
		member_indices[j] = member_indices[j + 1];
	}
	resize( num_members - 1 );
}

#endif // End IceBRG::galaxy_group class functions
