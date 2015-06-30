#!/usr/bin/env python

""" get_masks.py

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

# Magic values

base_good_fields_filename = "/disk2/brg/git/CFHTLenS_cat/Data/good_fields_list.txt"
base_bad_fields_filename = "/disk2/brg/git/CFHTLenS_cat/Data/bad_fields_list.txt"

base_command_filename = "/disk2/brg/git/CFHTLenS_cat/CFHTLenS_mask_testing/src/wget_cmd.txt"

field_replace_tag = "REPLACEME_FIELD"
i_or_y_replace_tag = "REPLACEME_I_OR_Y"
output_replace_tag = "REPLACEME_OUTPUT_NAME"

mask_output_path = "/disk2/brg/git/CFHTLenS_cat/Data/masks/"

def main(argv):
    """ TODO Docstring for main.
    
    """
    
    # Check if we've passed an argument. If so, this will be the subset of fields to get
    if(len(argv)>=2):
        subset = argv[1] + "_"
        good_fields_filename = base_good_fields_filename.replace("good_fields_list.txt",
                                                                 subset+"good_fields_list.txt")
        bad_fields_filename = base_bad_fields_filename.replace("bad_fields_list.txt",
                                                                 subset+"bad_fields_list.txt")
    else:
        good_fields_filename = base_good_fields_filename
        bad_fields_filename = base_bad_fields_filename
        
    
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
    command_base = ""
    with open(base_command_filename) as fcmd:
        for line in fcmd:
            command_base = command_base + line.strip() + " "
    
    # Now loop through every field
    for field_and_filter in fields.items():
        field = field_and_filter[0]
        filter = field_and_filter[1]
        
        # Set up the output file name
        output_name = mask_output_path + field + "_" + filter + ".fits"
        
        # Set up the command
        cmd = command_base.replace(output_replace_tag,output_name).replace(field_replace_tag,field).replace(i_or_y_replace_tag,filter)
        # Print the command (for testing)
        print(cmd)
        # Execute the command
        sbp.call(cmd,shell=True)
        
        packed_name = output_name + ".fz"
        
        # Set up compression command
        cmd = "rm -f " + packed_name + "; fpack " + output_name + "; rm " + output_name
        # Print the command (for testing)
        print(cmd)
        # Execute the command
        sbp.call(cmd,shell=True)

if __name__ == "__main__":
    main(sys.argv)
