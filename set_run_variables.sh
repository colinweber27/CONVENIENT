# Author: Colin M Weber (webe1077@umn.edu)
# Date: 7 August 2023
# Purpose: To set the variables needed for running cgone

# Command: source set_run_variables.sh

# Tell us what's going on
echo "Setting Convenient variables..."

## Set incoming neutrino parameters ##
export N_EVENTS=1000000 # Number of events to generate

export HC="RHC" # Horn current polarity

export NEUTRINO_PDG=-14 # The PDG code for the incoming neutrino species.

## Set target parameters ##
export GENIE_ND_SOUP=compositions/NDTargetComposition_GENIE_2.txt 
	# The text file containing the target composition of the NOvA ND

## Set list of event generators to use ##
# export RUNS="NuWro:NuWro/20230821params.txt,GENIE:G18_10a_02_11b,GENIE:N18_10j_02_11a,GENIE:AR23_20i_00_000"
export RUNS="NuWro:NuWro/20230821params.txt"
	# A list of comma-separated generators to run. 
	# The format must be GENERATOR:params/tune.

## Set run-specific parameters ##
export SEED=2 # The MC seed. It must be an integer >= 2 in order to be 
			  # deterministic due to NuWro setting seed=0 and seed=1 to 
			  # other values.

## Set relevant NUISANCE global parameters ## 
export NUISANCE_SETUP_SHELL=/nova/app/users/colweber/nuisance/build/Linux/setup.sh # The file location of the nuisance setup file. This will in general 
# not have to be changed, but it wouldn't hurt to check that this file 
# exists where expected.

## Nothing below this line should be changed.
## Use the above parameters to set other variables.
case "$HC" in
	RHC)
		export HC_lower="rhc"
		;;
	FHC)
		export HC_lower="fhc"
		;;
	*)
		echo "Unknown horn current polarity."
		;;
esac

# Translate the NEUTRINO_PDG code into words for directory naming
case "$NEUTRINO_PDG" in
	12)
		export FLAVOR="nue"
		;;
	-12)
		export FLAVOR="nuebar"
		;;
	14)
		export FLAVOR="numu"
		;;
	-14)
		export FLAVOR="numubar"
		;;
	*)
		echo "Error: Unknown neutrino pdg code."
		return
		;;
esac

export FLUX_FILE=flux/NuMI_naive_flux/$HC/"$HC_lower"_"$FLAVOR"_histos.root 
	# The ROOT file that contains the neutrino energy spectrum to use

export FLUX_HISTO="$FLAVOR"_tot # The neutrino energy spectrum in the flux file

export DATE="$(date '+%Y%m%d%H%M%S')"

# Tell us we're done
echo "Convenient variables set."
