# Author: Colin Weber (webe1077@umn.edu)
# Date: 12 July 2023
# Purpose: To use NUISANCE to process a NuWro output. The script set_nuisance_variables.sh must be run prior.

# Command: bash run_nuwro_nuisance.sh
# [-i input file name]
# [-F flux file,flux spectrum,beam component,1]
# [-o output file name]
# Note: this is only for a beam with a single neutrino flavor, as indicated 
# by the 1 at the end of the -F parameter string.

# Parameters
# ----------
#	-i
#		The name of the input NuWro file.
#	-F
#		A comma-separated string of 4 items. The first item names the path 
#		to the flux file used to create the input NuWro file. The second 
#		item names the histogram within the flux file that contains the flux 
#		used. The third component is the PDG code of the neutrinos in the 
#		input file. The fourth item tells NUISANCE that the input file was 
#		created using a single neutrino flavor.
#	-o
#		The name of the output file. Must end in ".root"

# Sources
# -------
#	$NUWRO_NUISANCE_SETUP_SHELL
#		The setup script corresponding to the version of NUISANCE that is 
#		built against NuWro.

# Outputs
# -------
#	-o
#		The NUISANCE file created from the input NuWro file.

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
