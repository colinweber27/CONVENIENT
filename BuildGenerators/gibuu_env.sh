# Purpose: to source the environment variables necessary for running GiBUU 
# within CONVENIENT

# Command: source gibuu_env.sh

# Exports
#	GiBUU: The location of the GiBUU executable
#	GiBUU_BUU_INPUT: The location of the BUU module
#	GiBUU_VERSION
#		The version of GiBUU this version of CONVENIENT is built with. This 
#		version is noted in the output .txt file accompanying each 
#		CONVENIENT output. It must be updated manually if new versions of 
#		GiBUU are used.

# Sources
#	$CONVENIENT_DIR/global_vars.sh
#		Environment variables that name different CONVENIENT directories

#!/bin/bash

# Set up the UPS products needed to build and use GiBUU
source $CONVENIENT_DIR/global_vars.sh

echo "Setting GiBUU environment variables..."

export GiBUU=$CONVENIENT_DIR/GiBUU/GiBUU.x

export GiBUU_BUU_INPUT=$CONVENIENT_GEN_DIR/GiBUU/buuinput

export GiBUU_VERSION=R2023_P3
