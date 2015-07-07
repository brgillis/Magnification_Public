""" magic_values.py

    Created on 6 Jul 2015 as part of project CFHTLenS_catalogue_fetching
    
    This module contains the needed commands to download catalogues from
    the CFHTLenS server.

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

def get_query():
    import magic_values as mv

    return "SELECT " + \
        "id, field, SeqNr, Xpos, Ypos, ALPHA_J2000, DELTA_J2000, MU_MAX, MU_THRESHOLD, MaxVal, Flag, " + \
        "A_WORLD, B_WORLD, THETA_J2000, ERRA_WORLD, ERRB_WORLD, ERRTHETA_J2000, FWHM_IMAGE, FWHM_WORLD, " + \
        "KRON_RADIUS, FLUX_RADIUS, CLASS_STAR, PSF_e1, PSF_e2, PSF_Strehl_ratio, e1, e2, weight, fitclass, " + \
        "bulge_fraction, model_flux, SNratio, MASK, ISOAREA_WORLD, Z_B, Z_B_MIN, Z_B_MAX, " + \
        "T_B, ODDS, Z_ML, T_ML, CHI_SQUARED_BPZ, m, c2, star_flag, " + \
        "LP_log10_SM_MED, LP_log10_SM_INF, LP_log10_SM_SUP, MAG_u, MAGERR_u, EXTINCTION_u, " + \
        "MAG_g, MAGERR_g, EXTINCTION_g, MAG_r, MAGERR_r, EXTINCTION_r, " + \
        "MAG_"+mv.query_i_or_y_replace_tag+", MAGERR_"+mv.query_i_or_y_replace_tag + \
        ", EXTINCTION_"+mv.query_i_or_y_replace_tag+", " + \
        "MAG_z, MAGERR_z, EXTINCTION_z " + \
        "FROM " + \
        "cfht.clens " + \
        "WHERE " + \
        "Z_B<=4 " + \
        "AND MAG_I<=98 " + \
        "AND star_flag<=0.01 " + \
        "AND field='"+mv.query_field_replace_tag+"'"
        
def get_wget_command():
    import magic_values as mv
    
    return "wget -O " + mv.command_output_replace_tag + " --recursive --level=inf " + \
        "'http://www.cadc-ccda.hia-iha.nrc-cnrc.gc.ca/community/CFHTLens/cgi/queryt.pl?format=ascii&query=" + \
        mv.command_query_replace_tag + "'"