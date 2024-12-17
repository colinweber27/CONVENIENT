# Author: Colin Weber (webe1077@umn.edu)
# Date: 31 January 2024
# Purpose: Set up Convenient environment and generators within 
# Convenient

# Command: source setup_convenient.sh

#!/bin/bash

# Set up Convenient environment, including exporting environment variables 
# and sourcing dependencies
echo "Setting up Convenient environment, including exporting environment variables and sourcing dependencies..."

export CONVENIENT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

source $CONVENIENT_DIR/global_vars.sh

# Set up generators
echo "Setting up generators..."
source $CONVENIENT_DIR/setup_generators.sh
