#!/usr/bin/env python

""" get_fields.py

    Created on 11 Sep 2014 as part of project CFHTLenS_catalogue_filtering

	---------------------------------------------------------------------

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
	
"""

import sys
import subprocess as sbp
from os.path import join

from CFHTLenS_Mag_fetching_data.fields_lists import *
from CFHTLenS_Mag_fetching_data.query import *

import CFHTLenS_Mag_fetching_data.magic_values as mv

def main(argv):
    """ TODO Docstring for main.
    
    """
    
    # Check if we've passed an argument. If so, this will be the data directory to use
    if(len(argv)>=2):
        data_dir = argv[1]
    else:
        data_dir = "./Data"
        
    print("Using " + data_dir + " as data directory. This can be changed by passing")
    print("the desired directory as the first command-line argument.\n")
    
    # Check if we've passed another argument, which will enable using all fields
    use_all_fields = False
    if(len(argv)>=3):
        if((argv[2]=="True") or (argv[2]=="true") or (argv[2]=="1")):
            use_all_fields = True
            
    if(use_all_fields):
        print("Creating full fields list. To disable this and only use one field,")
        print("instead pass False (or nothing) as the second command-line argument.\n")
    else:
        print("Creating a fields list with only one field. This is done to lighten the")
        print("load on the CFHTLenS servers for people only testing this code. To")
        print("get all fields, pass True as the second command-line argument.\n")
        
    # Get the full filenames for everything we'll need
    good_fields_filename = join(data_dir,mv.base_good_fields_filename)
    bad_fields_filename = join(data_dir,mv.base_bad_fields_filename)
    all_fields_filename = join(data_dir,mv.base_all_fields_filename)
    
    # Print field lists to files in the data directory
    with open(good_fields_filename,'w') as fgood:
        fgood.write(get_good_fields(use_all_fields=use_all_fields))
    print("Wrote good fields list to " + good_fields_filename + ".")
    with open(bad_fields_filename,'w') as fgood:
        fgood.write(get_bad_fields(use_all_fields=use_all_fields))
    print("Wrote bad fields list to " + bad_fields_filename + ".")
    with open(all_fields_filename,'w') as fgood:
        fgood.write(get_all_fields(use_all_fields=use_all_fields))
    print("Wrote full fields list to " + all_fields_filename + ".\n")
    
    # Check if there's a third command-line argument, which might tell us to stop here
    if(len(argv)>=4):
        if((argv[3]=="False") or (argv[3]=="false") or (argv[2]=="0")):
            print("Done!")
            print("To also fetch fields, pass True (or nothing) as the third command-line argument.")
            return
        else:
            print("Fetching fields now. To only regenerate fields lists, pass False as the third")
            print("command-line argument.\n")
    
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

    # Set up the base query
    query_base = get_query()
    
    # Set up the base command
    command_base = get_wget_command()
    
    # Now loop through every field
    for field_and_filter in fields.items():
        field = field_and_filter[0]
        my_filter = field_and_filter[1]
        
        # Set up the query
        query = query_base.replace(mv.query_field_replace_tag,field).replace(mv.query_i_or_y_replace_tag,my_filter).replace("=","%3D").replace("'","%27")
        
        # Set up the output file name
        output_name = join(data_dir,mv.full_tables_path,
                           field + "_" + my_filter + ".dat")
        temp_name = output_name + ".tmp"
        
        # Set up the command
        cmd = command_base.replace(mv.command_output_replace_tag,temp_name).replace(mv.command_query_replace_tag,query)
        # Execute the command
        sbp.call(cmd,shell=True)
        
        # Now, we want to add a '#' at the beginning of the file so the first line will be
        # recognized as a header
        #
        # It's easiest to do this using the bash shell
        cmd = "echo -n '# ' > " + output_name + "; cat " + temp_name + " >> " + output_name + \
            "; rm " + temp_name
        sbp.call(cmd,shell=True)
        
    print("Done!")
    print("The next step is catalogue filtering.")
    print("Run 'CFHTLenS_Mag_filter_catalogues " + data_dir + "' next.")

if __name__ == "__main__":
    main(sys.argv)
