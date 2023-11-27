# Author: Colin Weber (webe1077@umn.edu)
# Date: 12 July 2023
# Purpose: To set some environment variables for use when running run_nuisance.sh.

# Command: source set_nuisance_variables.sh

echo "Setting variables for nuisance..." # Remind us what's going on

## Set relevant NUISANCE global parameters ##
export NUISFLAT_GENIE_INPUT="GENIE:$GENIE_OUTPUT" # The generator whose output NUISANCE is flattening, followed by the name of the GENIE output

export NUISFLAT_NUWRO_INPUT="NuWro:$NUWRO_OUTPUT" # The generator whose output NUISANCE is flattening, followed by the name of the NuWro output.

export NUISFLAT_FORMAT=GenericVectors # The NUISANCE output format. Can be either GenericFlux or GenericVectors

echo "NUISANCE variables set." # Let us know it's done
