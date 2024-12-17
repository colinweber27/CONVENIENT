# Author: Colin Weber (webe1077@umn.edu)
# Date: 12 July 2023
# Purpose: To use NUISANCE to process a GENIE output. The script set_nuisance_variables.sh must be run prior.

# Command: bash run_genie_nuisance.sh
#	[-i input file name] [-f flux file,flux histo] 
#	[-t target composition in GENIE format] [-o output file name]
# 
# Parameters
# ----------
#	flux file,flux histo
#		A comma-separated pair of strings. The first string names the 
#		relative path (assuming Convenient as root directory) to the flux 
#		file used to create the input NUISANCE file. The second string names 
#		the histogram within the flux file that contains the flux used.  
#
#	target composition in GENIE format
#		The relative path (assuming Convenient as root director) to the file 
#		containing the target composition, in GENIE format.


# Build NUISANCE
echo "Building NUISANCE..."
source ${10}
echo "Nuisance built."

# Set the target composition from the GENIE ND soup file. 
# The file should already be in GENIE format, and 
# should have a single line of text.
while read line;
do
	target_comp=$(echo $line)
done < "$6"

# Set the input file format to nuisflat
nuisflat_input=GENIE:$2 

# Prepare the GENIE output by attaching additional xsec data to it
echo "Running PrepareGENIE on GENIE output..."
PrepareGENIE \
	-i $2 \
	-f $4 \
	-t $target_comp
echo "PrepareGENIE complete."

# Flatten the GENIE tree
echo "Running nuisflat on prepared GENIE output..."
nuisflat \
	-i $nuisflat_input \
	-f $NUISFLAT_FORMAT \
	-o $8
echo "nuisflat complete."
