# Author: Colin Weber (webe1077@umn.edu)
# Date: 12 July 2023
# Purpose: To use NUISANCE to process a NuWro output. The script set_nuisance_variables.sh must be run prior.

# Command: bash run_nuwro_nuisance.sh
# [-i input file name]
# [-F flux file,flux spectrum,beam component,1]
# [-o output file name]
# Note: this is only for a beam with a single neutrino flavor, as indicated 
# by the 1 at the end of the -F parameter string.

# Build NUISANCE
echo "Building NUISANCE..."
source $NUWRO_NUISANCE_SETUP_SHELL
echo "Nuisance built."

# Set NUISFLAT input file
nuisflat_input=NuWro:$2

# Prepare the NuWro output by appending flux and event count spectra
echo "Running PrepareNuWroEvents on NuWro output..."
PrepareNuWroEvents \
	-F $4 \
	$2
echo "Prepare NuWroEvents complete."

# Flatten the prepared NuWro output
echo "Running nuisflat on prepared NuWro output..."
nuisflat \
	-i $nuisflat_input \
	-f $NUISFLAT_FORMAT \
	-o $6
echo "nuisflat complete."
