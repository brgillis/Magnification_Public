""" @file match_command.py

    Created on 6 Jul 2015 as part of project CFHTLenS_catalogue_fetching

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

def get_match_command(data_dir):
    return 'stilts tmatch2 in1=' + data_dir + \
        '/spec_full_tables/all_good.fits ifmt2=ascii in2=REPLACEME_INPUT ' + \
        'out=REPLACEME_OUTPUT matcher=sky values1="ALPHA DELTA" ' + \
        'icmd2="addcol ra_deg 57.29577951*ra_radians; addcol dec_deg 57.29577951*dec_radians" ' + \
        'values2="ra_deg dec_deg" params="1" ofmt=fits-plus'