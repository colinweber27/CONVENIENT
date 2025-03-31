# Author: Colin Weber (webe1077@umn.edu)
# Date: 12 July 2023
# Purpose: To use NUISANCE to process a NEUT output(s). The script 
# set_nuisance_variables.sh must be run prior.

# Command: bash run_nuwro_nuisance.sh
# [-i input file name]
# [-f flux file] [-h flux histogram]

# Parameters
# ----------
#	-i
#		The input filename. Must refer to a NEUT output file.
#	-f
#		The path to a ROOT file containing a 1D histogram named by the -h 
#		parameter to use as the neutrino flux.
#	-h
#		The name of the histogram in the flux file to use as the neutrino 
#		flux.
#	-o
#		The name of the output file. Must end in ".root"

# Sources
# -------
#	$NEUT_NUISANCE_SETUP_SHELL
#		The NUISANCE setup script compatible with NEUT

# Outputs
# -------
#	nuisflat_output
#		The NUISANCE output file. One is created for each element in the 
#		target.

# Build NUISANCE
echo "Building NUISANCE..."
source $NEUT_NUISANCE_SETUP_SHELL
echo "Nuisance built."

# The flux that is inputted is the original flux, which hasn't been modified 
# for NEUT by setting all bins below 100 MeV to 0. However, for PrepareNEUT, 
# we want to use the flux that was used to create the events, that is, the 
# one modified for NEUT. The recipe for doing this is in NEUT/run_neut.sh
	neut_original_flux_file=${4/$CONVENIENT_DIR\//}

	neut_original_flux_histo=$6

	# Change all / or \ in the flux file to _
	neut_original_flux_filename=${neut_original_flux_file//[\/]/_}

	# Construct the full filepath to the NEUT flux file
	neut_root_beam_filepath="$CONVENIENT_DIR/NEUT/beam_energies/"$neut_original_flux_filename"."$neut_original_flux_histo".root"

	# Construct the input to PrepareNEUT
	prepareneut_flux_input=$neut_root_beam_filepath,$neut_original_flux_histo

# There may be several raw NEUT files if a mixed target composition has been 
# used, such as for the NOvA near detector. We therefore loop over all the 
# .raw.root files.
for raw_neut_file in $(ls $2*)
do
	# Construct the nuisflat input
	nuisflat_input=NEUT:$raw_neut_file

	# Prepare the NEUT output by appending flux and event count spectra
	echo "Running PrepareNEUT on NEUT output..."
	PrepareNEUT \
		-i $raw_neut_file \
		-f $prepareneut_flux_input \
		-G
	echo "PrepareNEUT complete."

	# Prepare the nuisflat output
	nuisflat_output=NEUT:"${raw_neut_file/raw/NUISANCE}"

	# Flatten the prepared NEUT output
	echo "Running nuisflat on prepared NEUT output..."
	nuisflat \
		-i $nuisflat_input \
		-f $NUISFLAT_FORMAT \
		-o $nuisflat_output
	echo "nuisflat complete."
done
