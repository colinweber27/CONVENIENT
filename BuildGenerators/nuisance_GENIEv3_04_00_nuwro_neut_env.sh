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

# Finds the directory where this script is located. This method isn't
# foolproof. See https://stackoverflow.com/a/246128/4081973 if you need
# something more robust for edge cases (e.g., you're calling the script using
# symlinks).
THIS_DIRECTORY="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# Set the NUISANCE base directory and source the setup.sh script within the
# base directory.
export NUISANCEBASE="$(dirname $THIS_DIRECTORY)"/Generators/nuisance_GENIEv3_04_00_nuwro_neut
source $NUISANCEBASE/build/Linux/setup.sh
