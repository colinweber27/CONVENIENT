# Author: Colin M Weber (webe1077@umn.edu)
# Date: 14 August 2023
# Purpose: To set environment variables for running run_nuwro.sh

# Command: source set_nuwro_variables.sh

# Let us know what's going on
echo "Setting NuWro variables..." 

# Set parameters
export NUWRO_OUTPUT="$params.$DATE.$SEED.eventsout.root" 
	# The NuWro output file name

# Let us know we're done
echo "NuWro variables set."
