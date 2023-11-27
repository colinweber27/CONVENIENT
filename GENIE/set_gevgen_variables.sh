# Author: Colin Weber (webe1077@umn.edu)
# Date: 6/21/2023
# Purpose: To set some environment variables for use when running GENIE's gevgen function.

# Command: source set_gevgen_variables.sh

echo "Setting variables for gevgen..." # Remind us what's going on

## Set relevant gevgen global parameters ##
export E_MIN=0.01 # The minimum neutrino energy, in GeV, in the flux 
				  # histogram

export E_MAX=20.0 # The maximum neutrino energy, in GeV, in the flux 
				  # histogram

export MSGLVL="Messenger_whisper.xml" # Sets the verbosity of GENIE

export GENIE_OUTPUT="$config.$DATE.$SEED.gntp.ghep.root" 
	# The file name of the GENIE output, declared here for later 
	# convenience

# GENIEXSECFILE is defined already when we wet up NOvA, but we may want to 
# redefine it in order to have access to more splines.
old_xsec_file=$GENIEXSECFILE # Reference the old xsec file
# Rename the file to access the full spline xml file instead of the smaller 
# file.
new_xsec_file=${old_xsec_file//small/big} # Replace small with big
new_xsec_file+=.gz # Tack on .gz at the end
export GENIE_XSEC_FILE=$new_xsec_file

echo "GENIE gevgen variables set."
