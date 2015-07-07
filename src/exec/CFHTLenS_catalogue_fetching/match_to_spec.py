#!/usr/bin/env python

""" CFHTLenS_Mag_get_fields.py

    Created Sep 2014 as part of project CFHTLenS_catalogue_filtering

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

import fetching_data.magic_values as mv
from fetching_data.match_command import *

def main(argv):
    """ Matches the CFHTLenS photo-z catalogues to spectro-z catalogues. Note: Due to the
        size of the required spectro-z catalogue, it isn't distributed with this repo and
        this script isn't normally installed.
    
    """
    
    # Check if we've passed an argument. If so, this will be the data directory to use
    if(len(argv)>=2):
        data_dir = argv[1]
    else:
        data_dir = "./Data"
        
    print("Using " + data_dir + " as data directory. This can be changed by passing")
    print("the desired directory as the first command-line argument.\n")
    
    # Get the replacement command
    
    base_command = get_match_command(data_dir)
    
    # Get the fields
    fields_list = join(data_dir,mv.filtered_tables_path,mv.base_all_fields_filename)
    fields = []
    with open(fields_list,'r') as fin:
        for line in fin:
            words = line.strip().split()
            for word in words:
                field = word[0:6]
                fields.append(field)
                
    matched_fields = [] 
    
    # Loop over fields
    for field in fields:
        phot_file_name = join(data_dir,mv.filtered_tables_path,field + "_lens.dat")
        matched_file_name = join(data_dir,mv.spec_matched_tables_path,field + "_z_matched.fits")
        
        cmd = base_command.replace(mv.input_replace_tag,phot_file_name).replace(mv.output_replace_tag,matched_file_name)
        # Print the command (for testing)
        print(cmd)
        # Execute the command
        sbp.call(cmd,shell=True)
        
        matched_fields.append(matched_file_name)
        
    # Now concatenate all the matched fields
    cmd = "stilts tcat ifmt=fits-plus ofmt=ascii out=" + \
        join(mv.spec_matched_tables_path,mv.spec_matched_output_file)
    for field in matched_fields:
        cmd = cmd + " in=" + str(field)
        
    # Execute the command
    sbp.call(cmd,shell=True)

if __name__ == "__main__":
    main(sys.argv)