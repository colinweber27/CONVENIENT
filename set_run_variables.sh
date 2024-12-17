# Author: Colin M Weber (webe1077@umn.edu)
# Date: 7 August 2023
# Purpose: To set the variables needed for running Convenient
# Note that the variables 'FLUXES', 'FLUX_FILE_NAMES', 'FLUX_HISTOS', 
# 'TARGETS', 'RUNS', and 'SEEDS' are to be entered as comma-separated lists 
# of the same length. The macro to actually run Convenient will throw an 
# error if this is not the case.

# Command: source set_run_variables.sh

# Tell us what's going on
echo "Setting Convenient variables..."

## Set incoming neutrino parameters ##
export N_EVENTS=20 # Number of events to generate

export HC="RHC" # Horn current polarity

export FLUXES="DeriveFlux50MeVwidth_ppfx" 
	# Directory within Convenient/flux/NuMI/$HC that contains the desired 
	# flux file. May be a comma-separated list.

export FLUX_FILE_NAMES="numubar.root"
	# File that contains the desired flux histogram. May be a 
	# comma-separated list.

export FLUX_HISTOS="numubar"
	# The histogram containing the neutrino flux in the flux file. May be a 
	# comma-separated list.

export NEUTRINO_PDG=-14 # The PDG code for the incoming neutrino species.

export GIBUU_CC_NC="CC" # If generating GiBUU events, generate CC or NC events

## Set target parameters ##
export TARGETS="NOvA_ND/NDTargetComposition_GENIE_1.txt" 
	# The text file containing the target composition, in GENIE format. File 
	# path is relative to Convenient/targets directory.

## Set list of event generators to use ##
export RUNS="GiBUU:Defaultparams.job"
	# A list of comma-separated generators to run. 
	# The format must be GENERATOR:params or tune, where params or tune is 
	# either a GENIE tune or a parameters file relative to that generator's 
	# directory.
	# BUG ALERT: The tunes G18_10j_00_000 and N18_10j_02_11a-resfixfix must 
	# always come last, because there is a bug related to switching from 
	# the version of NUISANCE built for that tune, to the version of 
	# NUISANCE built for the other tunes.

## Set run-specific parameters ##
export SEEDS="2" 
	# The MC seed. It must be an integer >= 2 in order to be deterministic 
	# due to NuWro setting seed=0 and seed=1 to other values. May be a 
	# comma-separated list.

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

export DATE="$(date '+%Y%m%d%H%M%S')"

# Tell us we're done
echo "Convenient variables set."
