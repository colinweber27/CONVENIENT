# Author: Colin Weber (webe1077@umn.edu)
# Date: 21 July 2023
# Purpose: To generate neutrino events using GENIE using the more-general 
# gevgen tool. Greater generality is important because it will make the GENIE
# output easier to compare to the outputs of other event generators.

# Command: run_gevgen.sh 
# 	[-n number of events]
#	[-f flux file,flux histogram] [-p neutrino pdg]
#	[-t target composition in GENIE form] [-o output file name]
#	[--seed MC seed] [--tune GENIE tune]

# She-bang!
#!/bin/bash

# Set the target composition from the GENIE ND soup file. 
# The file should already be in GENIE format, and 
# should have a single line of text.
while read line;
do
	target_comp=$(echo $line)
done < "$8"


# Generate the GENIE events. See the GENIE user's manual for info on the 
# gevgen command.
echo "Running gevgen w/ tune ${14}"
gevgen \
	-n $2 \
	-e $E_MIN,$E_MAX \
	-f $4 \
	-p $6 \
	-t $target_comp \
	--cross-sections $GENIE_XSEC_FILE \
	--seed "${12}" \
	--message-thresholds $MSGLVL \
	--tune "${14}" \
	-o "${10}"
echo "gevgen w/ tune ${14} finished"
