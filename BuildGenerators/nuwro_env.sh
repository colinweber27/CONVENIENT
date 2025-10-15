# Purpose: to source the environment variables necessary for running NuWro
# within CONVENIENT

# Command: source nuwro_env.sh

# Exports
#	NUWROBASE: the directory containing the NuWro build
#	PYTHIA6
#		The directory containing the PYTHIA6 build, which NuWro uses for 
#		hadronization
#	NUWRO: Same, as NUWROBASE, except this is the variable NuWro looks for
#	LD_LIBRARY_PATH
#		Adds directory containing NuWro libraries to the library path
#	PATH: Adds directory containing NuWro executable to PATH

# Sources
#	$CONVENIENT_DIR/global_vars.sh
#		Environment variables that name different CONVENIENT directories

#!/bin/bash

# Set up the UPS products needed to build and use NuWro
source $CONVENIENT_DIR/global_vars.sh

echo "Setting NuWro environment variables..."

export NUWROBASE=$CONVENIENT_GEN_DIR/nuwro
export PYTHIA6=$PYTHIA6_LIBRARY
export NUWRO=$NUWROBASE
export LD_LIBRARY_PATH=$CONVENIENT_GEN_BUILD_DIR/pythia6:$NUWRO/lib:$NUWRO/bin:$LD_LIBRARY_PATH
export PATH=$NUWRO/bin:$PATH
