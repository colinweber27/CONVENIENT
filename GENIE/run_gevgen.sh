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

# Parameters
#	number of events : int
#		The number of events to generate
#	flux file,flux histogram : tuple
#		A comma-separated pair of strings. The first string is the path to a 
#		ROOT file containing a histogram of a neutrino flux to use. The 
#		second is the name of the histogram within that file.
#	neutrino pdg : int
#		The PDG code of the neutrino flavor to use.
#		Must be one of {-14, -12, 12, 14}, corresponding to anti-muon, 
#		anti-electron, electron, and muon neutrinos, respectively.
#	target composition in GENIE form : str
#		The path to a .txt file containing the target composition to use. 
#		The target composition must be in GENIE form.
#	output file name : str
#		The name of the output file
#	MC seed : int
#		The seed for the MC generator to use. Must be greater than 1
#	GENIE tune : str
#		The GENIE tune to run with. Available tunes can be seen at 
#		https://hep.ph.liv.ac.uk/~costasa/genie/tunes.html, although note 
#		that not all tunes will have splines generated. NOvA-maintained xsec 
#		splines for different GENIE versions and tunes can be found at 
#		/cvmfs/nova.opensciencegrid.org/externals/genie_xsec.	

# Outputs
#	output_file_name: The output GENIE file in ghep format

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
