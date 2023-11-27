# Author: Colin Weber (webe1077@umn.edu)
# Date: 21 July 2023
# Purpose: To generate neutrino events using GENIE using the more-general 
# gevgen tool. Greater generality is important because it will make the GENIE
# output easier to compare to the outputs of other event generators.

# Command: bash run_gevgen.sh 
# 	[-n number of events] [-e min energy,max energy] 
#	[-f flux file,flux spectrum] [-p beam neutrino pdg]
#	[-t target composition in GENIE form] 
#	[--cross-sections cross section spline xml file] [--seed MC seed] 
#	[--message-thresholds message level xml file] [--tune GENIE tune]

# She-bang!
#!/bin/bash

# Set the target composition from the GENIE ND soup file. 
# The file should already be in GENIE format, and 
# should have a single line of text.
while read line;
do
	TARGET_COMP=$(echo $line)
done < "$8"


# Generate the GENIE events
echo "Running gevgen w/ tune ${12}"
gevgen \
	-n $2 \
	-e $E_MIN,$E_MAX \
	-f $4 \
	-p $6 \
	-t $TARGET_COMP \
	--cross-sections $GENIE_XSEC_FILE \
	--seed "${10}" \
	--message-thresholds $MSGLVL \
	--tune "${12}" \
	-o $GENIE_OUTPUT
echo "gevgen w/ tune ${12} finished"
