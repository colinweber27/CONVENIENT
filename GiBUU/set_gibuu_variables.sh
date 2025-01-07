# Author: Colin M Weber (webe1077@umn.edu)
# Date: 30 May 2024
# Purpose: To set environment variables for running run_gibuu.sh

# Command: source set_gibuu_variables.sh

# Let us know what's going on
echo "Setting GiBUU variables..." 

# Set global parameters. These are included here because they don't change, 
# regardless of other run parameters.
export GiBUU_VERSION=R2023_P3

# The rest of the parameters ensures GiBUU will run as expected.
# The parameters nuXsectionMode and nuExp tell GiBUU to use an input 
# histogram for the flux
export nuXsectionMode=16

export nuExp=99

export numEnsembles=4000
	# This is the number of prallel ensembles to have open at one time when 
	# running the code. numEnsembles is used to determine the number of 
	# events to generate. It is related to the amount of memory the code 
	# occupies. If debugging (i.e. N_EVENTS < 1000), it shoud be set even 
	# lower (i.e. 2). The default is 4000. It should be set lower for heavier 
	# nuclei and if there are memory problems.

# We end up needing the name of the GiBUU flux file in several places later 
# on, so we determine it here, and calculate it if necessary
gibuu_original_flux_file=${2/$CONVENIENT_DIR\//}
gibuu_original_flux_histo=$4
export gibuu_dat_flux_file="$CONVENIENT_DIR/GiBUU/beam_energies/"${gibuu_original_flux_file//[\/]/_}"."$gibuu_original_flux_histo".dat"
if [ ! -f "$gibuu_dat_flux_file" ];
then
	# Translate the original flux file to a .dat file in the appropriate 
	# format
	root -q "make_beam_energy_dat.C(\"$gibuu_original_flux_file\", \"$gibuu_original_flux_histo\", \"$gibuu_dat_flux_file\")"
fi

# Let us know we're done
echo "GiBUU variables set."
