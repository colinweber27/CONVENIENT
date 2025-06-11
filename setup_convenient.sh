# Author: Colin Weber (webe1077@umn.edu)
# Date: 31 January 2024
# Purpose: Set up Convenient environment and generators within 
# Convenient

# Command: source setup_convenient.sh

# Exports
# -------
#	CONVENIENT_DIR
#		Assumes the current directory is the top-level CONVENIENT directory, 
#		and assigns it to the variable.
#
# Sources
# -------
#	$CONVENIENT_DIR/global_vars.sh
#		A script defining the global environment variables for CONVENIENT.
#
#	$CONVENIENT_DIR/setup_generators.sh
#		A script defining environment variables needed for running each 
#		generator.

#!/bin/bash

# Set up Convenient environment, including exporting environment variables 
# and sourcing dependencies
echo "Setting up Convenient environment, including exporting environment variables and sourcing dependencies..."

export CONVENIENT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

source $CONVENIENT_DIR/global_vars.sh

# Set up generators
echo "Setting up generators..."
# All generators were built with an older version of ROOT than what NOvA 
# automatically sets up, so first set up the correct ROOT version.
setup root v6_22_08d -q e20:p392:prof
source $CONVENIENT_DIR/setup_generators.sh
