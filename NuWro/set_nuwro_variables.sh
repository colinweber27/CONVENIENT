# Author: Colin M Weber (webe1077@umn.edu)
# Date: 14 August 2023
# Purpose: To set environment variables for running run_nuwro.sh

# Command: source set_nuwro_variables.sh

# Exports
# -------
#	NUWRO_VERSION
#		The version of NuWro that's being run. This is set when building 
#		CONVENIENT.
#
#	NUWRO_NUISANCE_SETUO_SHELL
#		The path to a NUISANCE setup shell that was built against NuWro.

# Let us know what's going on
echo "Setting NuWro variables..." 

# Set global parameters. These are included here because they don't change, 
# regardless of other run parameters.
export NUWRO_VERSION=21.09

export NUWRO_NUISANCE_SETUP_SHELL=$CONVENIENT_GEN_DIR/nuisance_GENIEv3_04_00_nuwro_neut/build/Linux/setup.sh

# Let us know we're done
echo "NuWro variables set."
