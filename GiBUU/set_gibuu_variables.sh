# Author: Colin M Weber (webe1077@umn.edu)
# Date: 30 May 2024
# Purpose: To set environment variables for running run_gibuu.sh

# Command: source set_gibuu_variables.sh \
#			[-f GiBUU flux file] [-h GiBUU flux histo]

# Parameters
#	-f
#		The ROOT fiel containing the 1D flux histogram to use as input
#	-h
#		The 1D histogram within the flux_file containing the flux.

# Exports
#	GiBUU_VERSION
#		The version of GiBUU built in CONVENIENT. This variable is 
#		automatically set when building CONVENIENT.
#	nuXsectionMode
#		Must be one of {6, 16}. 
#		If 6, look for a monoenergetic neutrino source. If 16, look for a 
#		broadband neutrino flux and calculate weights assuming the analyzer 
#		will be calculating a flux-averaged cross section. In general, this 
#		should not be changed from 16.
#	nuExp
#		An integer determining which flux to use. If 99, use a user-inputted 
#		flux histogram. Other integers refer to public fluxes from different #		experiments, with the mapping available in lines 59-76 in 
#		GiBUU/Defaultparams.job. Only relevant if nuXsectionMode=16. In 
#		general, this should not be changed from 99.
#	numEnsembles
#		The number of parallel ensembles open at one time during a run. 
#		numEnsembles is used to determine the number of events to generate. 
#		It is related to the amount of memory the code occupies. If 
#		debugging (i.e. N_EVENTS < 1000), it shoud be set even lower (i.e. 
#		2). The default is 4000. It should be set lower for heavier nuclei 
#		and if there are memory problems.
#	gibuu_dat_flux_file
#		The file name of the .dat file to use as neutrino flux input to 
#		GiBUU. Is determined from the input parameters.

# Outputs
#	$gibuu_dat_flux_file (optional)
#		The file containing a neutrino flux in the format readable by GiBUU. 
#		Only created if it doesn't yet exist.

# Let us know what's going on
echo "Setting GiBUU variables..." 

# Set global parameters. These are included here because they don't change, 
# regardless of other run parameters.
export GiBUU_VERSION=R2023_P3

# The rest of the parameters ensures GiBUU will run as expected.
# The parameters nuXsectionMode and nuExp tell GiBUU to use an input 
# histogram for the flux and calculate weights for a flux-averaged xsec.
export nuXsectionMode=16

export nuExp=99

export numEnsembles=4000
	# This is the number of prallel ensembles to have open at one time when 
	# running the code. numEnsembles is used to determine the number of 
	# events to generate. It is related to the amount of memory the code 
	# occupies. If debugging (i.e. N_EVENTS < 1000), it shoud be set even 
	# lower (i.e. 2). The default is 4000. It should be set lower for 
	# heavier nuclei and if there are memory problems.

# We end up needing the name of the GiBUU flux file in several places later 
# on, so we determine it here, and calculate it if necessary
gibuu_original_flux_file=${2/$CONVENIENT_DIR\//}
gibuu_original_flux_histo=$4
export gibuu_dat_flux_file="$CONVENIENT_DIR/GiBUU/beam_energies/"${gibuu_original_flux_file//[\/]/_}"."$gibuu_original_flux_histo".dat"
if [ ! -f "$gibuu_dat_flux_file" ];
then
	# Translate the original flux file to a .dat file in the appropriate 
	# format
	root -q "make_beam_energy_dat.C(\"${2}\", \"$gibuu_original_flux_histo\", \"$gibuu_dat_flux_file\")"
fi

# Let us know we're done
echo "GiBUU variables set."
