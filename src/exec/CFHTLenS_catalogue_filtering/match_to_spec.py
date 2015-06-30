import sys
import subprocess as sbp

# Magic values

input_replace_tag = "REPLACEME_INPUT"
output_replace_tag = "REPLACEME_OUTPUT"

base_command_file = "/home/brg/git/CFHTLenS_cat/CFHTLenS_catalogue_filtering/src/base_match_command.txt"

fields_list = "/disk2/brg/git/CFHTLenS_cat/Data/fields_list.txt"

input_fields_location = "/disk2/brg/git/CFHTLenS_cat/Data/filtered_tables/"

output_fields_location = "/disk2/brg/git/CFHTLenS_cat/Data/spec_matched_tables/"

merged_output_file = "/disk2/brg/git/CFHTLenS_cat/Data/spec_matched_tables/all_matched_galaxies.dat"

def main(argv):
    """ TODO Docstring for main.
    
    """
    
    # Get the replacement command
    
    with open(base_command_file,'r') as fin:
        for line in fin:
            base_command = line.strip()
            break
    
    # Get the fields
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
        phot_file_name = input_fields_location + field + "_lens.dat"
        matched_file_name = output_fields_location + field + "_z_matched.fits"
        
        cmd = base_command.replace(input_replace_tag,phot_file_name).replace(output_replace_tag,matched_file_name)
        # Print the command (for testing)
        print(cmd)
        # Execute the command
        sbp.call(cmd,shell=True)
        
        matched_fields.append(matched_file_name)
        
    # Now concatenate all the matched fields
    cmd = "stilts tcat ifmt=fits-plus ofmt=ascii out=" + str(merged_output_file)
    for field in matched_fields:
        cmd = cmd + " in=" + str(field)
        
    # Print the command (for testing)
    print(cmd)
    # Execute the command
    sbp.call(cmd,shell=True)

if __name__ == "__main__":
    main(sys.argv)