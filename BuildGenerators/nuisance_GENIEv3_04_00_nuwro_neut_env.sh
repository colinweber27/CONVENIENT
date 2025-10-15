# Purpose: to source the environment variables necessary for running 
# NUISANCE within CONVENIENT

# Command: source nuisance_GENIEv3_04_00_env.sh

# Exports
#	NUISANCEBASE: the directory within which NUISANCE is built

# Sources
#	$CONVENIENT_DIR/global_vars.sh
#		Environment variables that name different CONVENIENT directories
#	$NUISANCEBASE/build/Linux/setup.sh
#		Environment variables necessary for running NUISANCE
#!/bin/bash

# Set up the UPS products needed to build and use Nuisance
source $CONVENIENT_DIR/global_vars.sh

echo "Setting NUISANCE environment variables..."

# Set the NUISANCE base directory and source the setup.sh script within the
# base directory.
export NUISANCEBASE=$CONVENIENT_GEN_DIR/nuisance_GENIEv3_04_00_nuwro_neut
source $NUISANCEBASE/build/Linux/setup.sh
