# Author: Colin Weber (webe1077@umn.edu)
# Date: 12 July 2023
# Purpose: To use NUISANCE to process a NuWro output. The script set_nuisance_variables.sh must be run prior.

# Command: source run_nuwro_nuisance.sh
# [-f flux file,flux spectrum,beam component,1] [-i nuisflat input file]
# Note: this is only for a beam with a single neutrino flavor

# Set NUISFLAT input file
export NUISFLAT_INPUT=$NUISFLAT_NUWRO_INPUT

# Prepare the NuWro output by appending flux and event count spectra
echo "Running PrepareNuWroEvents on NuWro output..."
PrepareNuWroEvents \
	-F $2 \
	$NUWRO_OUTPUT
echo "Prepare NuWroEvents complete."

# Flatten the prepared NuWro output
echo "Running nuisflat on prepared NuWro output..."
nuisflat \
	-i $NUISFLAT_INPUT \
	-f $NUISFLAT_FORMAT
echo "nuisflat complete."
