# Author: Colin Weber (webe1077@umn.edu)
# Date: 31 January 2024
# Purpose: To export global environment variables for Convenient and 
# set up dependencies if they are not already set up.

# Command: source global_vars.sh

#!/bin/bash

# Global variables
export OUTPUT_DIR=/exp/nova/data/users/$USER
export CONVENIENT_GEN_DIR=$CONVENIENT_DIR/Generators
export CONVENIENT_GEN_BUILD_DIR=$CONVENIENT_DIR/BuildGenerators
export CONVENIENT_FLUX_DIR=$CONVENIENT_DIR/flux
export CONVENIENT_TAR_DIR=$CONVENIENT_DIR/targets
export CONVENIENT_NUISANCE_DIR=$CONVENIENT_DIR/NUISANCE

export CONVENIENT_NOvA_OUTPUT_DIR=$OUTPUT_DIR/ConvenientOutputs_NOvA
export CONVENIENT_NUISANCE_NOvA_OUTPUT_DIR=$OUTPUT_DIR/NUISANCEOutputs_NOvA
export CONVENIENT_OUTPUT_DIR=$OUTPUT_DIR/ConvenientOutputs
export CONVENIENT_NUISANCE_OUTPUT_DIR=$OUTPUT_DIR/NUISANCEOutputs

# Dependencies
# Set up the UPS products needed to build and use Convenient
# If novasoft has been set up, this shouldn't be done. Use all existing 
# dependencies.
if [ -z "$NOVA_RELEASE" ]; # If there is no NOVA_RELEASE
then
	source /grid/fermiapp/products/larsoft/setups
	setup root v6_12_06a -q e17:debug
	setup lhapdf v5_9_1k -q e17:debug
	setup log4cpp v1_1_3a -q e17:debug
	setup pdfsets v5_9_1b
	setup gdb v8_1
	setup git v2_15_1
	setup cmake
fi
