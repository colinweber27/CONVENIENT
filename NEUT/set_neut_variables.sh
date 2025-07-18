# Author: Colin M Weber (webe1077@umn.edu)
# Date: 14 August 2023
# Purpose: To set environment variables for running run_neut.sh

# Command: source set_neut_variables.sh
#	[-s seed]

# Parameters
#	-s : The MC seed to use

# Exports
#	NEUT_VERSION
#		The version of NEUT built in CONVENIENT. This variable is 
#		automatically set when building CONVENIENT.
#	NEUT_NUISANCE_SETUP_SHELL
#		The path to a NUISANCE setup script for a version that has been 
#		built w/ NEUT compatibility.
#	NEUT_INMEV
#		Must be one of {0, 1}. If 0, the input flux histogram is assumed to 
#		be in GeV. If 1, the input flux histogram is in units of MeV.
#	NEUT_RAND (defaults to 0)
#		If 0, NEUT wll use $RANFILE to seed the MC simulation. $RANFILE is 
#		created as a part of this script.
#	NEUT_MPV (defaults to 3)
#		If 3, NEUT will look for a .root file as flux input
#	RANFILE
#		A string pointing to the random number file for NEUT to use as MC 
#		seed.

# Outputs
#	random.txt
#		A file seeding NEUT's MC generator.

# Let us know what's going on
echo "Setting NEUT variables..." 

# Set global parameters. These are included here because they don't change, 
# regardless of other run parameters.
export NEUT_VERSION=v5.7.0

export NEUT_NUISANCE_SETUP_SHELL=$CONVENIENT_GEN_DIR/nuisance_GENIEv3_04_00_nuwro_neut/build/Linux/setup.sh

export NEUT_INMEV=0
	# This parameter says that the flux histograms we are inputting are in 
	# GeV

export NEUT_RAND=0
	# This parameter says that NEUT should use $RANFILE as the seed

export NEUT_MPV=3
	# This parameter tells NEUT to look for a .root file for the flux

# Set run-specific parameters
# To set the seed, create a file called random that has the seed number in 
# it, then export the variable RANFILE which should point to the random 
# number file.
touch random.txt # Create the file
echo    -1290784521  387739155  23874124  39827  $2 >> random.txt 
	# Direct the seed to the file. This is copied from the NEUT random 
	# number file neut/src/neutgeo/random.tbl, but we're substituting the 
	# last number for the seed.
export RANFILE=random.txt # Point RANFILE to the file

# Let us know we're done
echo "NEUT variables set."
