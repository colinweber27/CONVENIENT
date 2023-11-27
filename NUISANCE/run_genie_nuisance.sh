# Author: Colin Weber (webe1077@umn.edu)
# Date: 12 July 2023
# Purpose: To use NUISANCE to process a GENIE output. The script set_nuisance_variables.sh must be run prior.

# Command: source run_genie_nuisance.sh 
# 	[-f flux file,flux histo] [-t target composition in GENIE format] 
# 	[-i nuisflat input in the form (generator:file)]

# Set the input file
export NUISFLAT_INPUT=$NUISFLAT_GENIE_INPUT

# Set the target composition from the GENIE ND soup file. 
# The file should already be in GENIE format, and 
# should have a single line of text.
while read line;
do
	TARGET_COMP=$(echo $line)
done < "$4"

# Prepare the GENIE output by attaching additional xsec data to it
echo "Running PrepareGENIE on GENIE output..."
PrepareGENIE \
	-i $GENIE_OUTPUT \
	-f $2 \
	-t $TARGET_COMP
echo "PrepareGENIE complete."

# Flatten the GENIE tree
echo "Running nuisflat on prepared GENIE output..."
nuisflat \
	-i $NUISFLAT_INPUT \
	-f $NUISANCE_FORMAT
echo "nuisflat complete."
