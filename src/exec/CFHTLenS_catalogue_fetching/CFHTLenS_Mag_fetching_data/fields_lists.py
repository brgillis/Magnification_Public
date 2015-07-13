""" @file fields_lists.py

    Created on 6 Jul 2015 as part of project CFHTLenS_catalogue_fetching
    
    This module contains hardcoded lists of the CFHTLenS field names.

    ---------------------------------------------------------------------

    Copyright (C) 2015  Bryan R. Gillis

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

def get_good_fields(use_all_fields=False):
    if(use_all_fields):
        return "W1m0m0_i W1m0m3_i W1m0m4_y W1m0p1_i W1m0p2_i W1m0p3_i W1m1m0_i\n" + \
            "W1m1m2_i W1m1m3_i W1m1m4_y W1m1p3_i W1m2m1_i W1m2m2_i W1m2m3_i\n" + \
            "W1m2p1_i W1m2p2_i W1m3m0_i W1m3m2_i W1m3m4_y W1m3p1_i W1m3p3_i\n" + \
            "W1m4m0_i W1m4m1_i W1m4m3_i W1m4m4_y W1m4p1_i W1p1m1_i W1p1m2_i\n" + \
            "W1p1m3_i W1p1m4_y W1p1p1_y W1p1p2_i W1p1p3_i W1p2m0_i W1p2m2_i\n" + \
            "W1p2m3_i W1p2m4_y W1p2p1_i W1p2p2_i W1p2p3_i W1p3m1_i W1p3m2_i\n" + \
            "W1p3m3_i W1p3m4_y W1p3p1_y W1p3p2_i W1p3p3_i W1p4m0_i W1p4m1_i\n" + \
            "W1p4m2_i W1p4m3_i W1p4m4_y W1p4p1_i W1p4p2_i W1p4p3_i\n" + \
            "\n" + \
            "W2m0m0_i W2m0m1_i W2m0p1_i W2m0p2_i W2m1m0_i W2m1m1_i W2m1p1_i\n" + \
            "W2m1p3_i W2p1m0_i W2p1p1_i W2p1p2_i W2p2m0_i W2p2m1_i W2p2p1_i\n" + \
            "W2p2p2_i W2p3m0_i W2p3m1_i W2p3p1_i W2p3p3_i\n" + \
            "\n" + \
            "W3m0m1_y W3m0m2_i W3m0m3_i W3m0p2_i W3m0p3_i W3m1m0_i W3m1m2_i\n" + \
            "W3m1m3_i W3m1p1_i W3m1p2_i W3m1p3_i W3m2m1_y W3m2m2_i W3m2m3_i\n" + \
            "W3m2p1_y W3m2p2_i W3m3m0_i W3m3m1_i W3m3m2_i W3m3m3_i W3m3p1_i\n" + \
            "W3m3p2_i W3p1m0_i W3p1m1_i W3p1m2_i W3p1m3_i W3p1p2_i W3p1p3_i\n" + \
            "W3p2m0_i W3p2m3_y W3p2p3_i W3p3m1_i W3p3m3_i W3p3p1_i W3p3p2_i\n" + \
            "W3p3p3_i\n" + \
            "\n" + \
            "W4m0m2_i W4m0p1_i W4m1m0_i W4m1m1_i W4m1m2_i W4m1p1_y W4m2m0_i\n" + \
            "W4m2p1_i W4m2p3_y W4m3m0_i W4m3p1_i W4m3p2_i W4m3p3_y W4p1m0_i\n" + \
            "W4p1m1_i W4p1m2_i W4p2m0_i W4p2m1_i W4p2m2_i\n"
    else:
        return "W1m0m0_i"
        
def get_bad_fields(use_all_fields=False):
    if(use_all_fields):
        return "W1m0m1_i W1m0m2_i W1m1m1_i W1m1p1_i W1m1p2_i W1m2m0_i W1m2m4_y\n" + \
            "W1m2p3_i W1m3m1_i W1m3m3_i W1m3p2_i W1m4m2_i W1m4p2_i W1m4p3_i\n" + \
            "W1p1m0_i W1p2m1_i W1p3m0_i\n" + \
            "\n" + \
            "W2m0p3_i W2m1p2_i W2p1m1_i W2p1p3_i W2p2p3_i W2p3p2_i\n" + \
            "\n" + \
            "W3m0m0_i W3m0p1_i W3m1m1_i W3m2m0_i W3m2p3_i W3m3p3_i W3p1p1_i\n" + \
            "W3p2m1_i W3p2m2_i W3p2p1_i W3p2p2_i W3p3m0_i W3p3m2_i\n" + \
            "\n" + \
            "W4m0m0_i W4m0m1_i W4m1p2_y W4m1p3_y W4m2p2_y W4p1p1_i\n"
    else:
        return ""
        
    
def get_all_fields(use_all_fields=False):
    return get_good_fields(use_all_fields) + "\n" + get_bad_fields(use_all_fields)
