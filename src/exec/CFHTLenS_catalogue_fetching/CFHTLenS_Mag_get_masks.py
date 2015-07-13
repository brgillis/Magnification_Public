#!/usr/bin/env python

""" @file get_masks.py

    Created on 14 Oct 2014 as part of project CFHTLenS_mask_testing

	---------------------------------------------------------------------

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
	
"""

import sys
import subprocess as sbp
from os.path import join

import CFHTLenS_Mag_fetching_data.magic_values as mv
from CFHTLenS_Mag_fetching_data.query import get_mask_wget_command

def main(argv):
    """ Main function for CFHTLenS_Mag_get_masks - downloads mask files from the CFHTLenS
        server.
        
        Takes at command-line:
        
        -data_dir <string> (data directory, defaults to './Data')
        -use_all_fields <bool> (whether or not to get all masks (True) or just one
                                (False). Defaults to False.)
    """
    
    # Check if we've passed an argument. If so, this will be the data directory to use
    if(len(argv)>=2):
        data_dir = argv[1]
    else:
        data_dir = "./Data"
        
    print("Using " + data_dir + " as data directory. This can be changed by passing")
    print("the desired directory as the first command-line argument.\n")
        
    # Get the full filenames for everything we'll need
    good_fields_filename = join(data_dir,mv.base_good_fields_filename)
    bad_fields_filename = join(data_dir,mv.base_bad_fields_filename)        
    
    # Initialize dictionary
    fields = {};

    # Add in good fields
    with open(good_fields_filename,'r') as fgood:
        for line in fgood:
            words = line.strip().split()
            for word in words:
                field = word[0:6]
                i_or_y = word[-1:]
                fields[field] = i_or_y                       

    # Add in bad fields now
    with open(bad_fields_filename,'r') as fbad:
        for line in fbad:
            words = line.strip().split()
            for word in words:
                field = word[0:6]
                i_or_y = word[-1:]
                if(not fields.has_key(field)):
                    # If we don't have a good field by this name, add this
                    fields[field] = i_or_y
    
    # Set up the base command
    command_base = get_mask_wget_command()
    
    # Now loop through every field
    for field_and_filter in fields.items():
        field = field_and_filter[0]
        my_filter = field_and_filter[1]
        
        # Set up the output file name
        output_name = join(data_dir,mv.mask_path,field + "_" + my_filter + ".fits")
        
        # Set up the command
        cmd = command_base.replace(mv.output_replace_tag,output_name).replace( \
                        mv.query_field_replace_tag,field).replace(mv.query_i_or_y_replace_tag,my_filter)
        # Execute the command
        sbp.call(cmd,shell=True)
        
        packed_name = output_name + ".fz"
        
        # Set up compression command
        cmd = "rm -f " + packed_name + "; fpack " + output_name + "; rm " + output_name
        # Execute the command
        sbp.call(cmd,shell=True)

if __name__ == "__main__":
    main(sys.argv)
