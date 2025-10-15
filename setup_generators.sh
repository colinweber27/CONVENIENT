# Author: Colin Weber (webe1077@umn.edu)
# Date: 31 January 2024
# Purpose: To set up the environment for the generators

# Command: source setup_generators.sh

# Exports
# -------
#	PYTHIA6
#		The location of the Pythia 6 library. The two assignments in the 
#		script are identical if novasoft has been set up, and are both empty 
#		if novasoft has not been set up. The two assignments anticipate a 
#		newer version of CONVENIENT in which generators that haven't been 
#		built aren't set up upon setting up CONVENIENT.
#	NUWRO
#		The top-level NuWro directory
# 	GiBUU
#		The location of the GiBUU executable
#	GiBUU_BUU_INPUT
#		The location of the BUU submodule accompanying GiBUU
#	GiBUU_VERSION
#		The GiBUU version that has been installed. This value is hard-coded 
#		in, so it needs to be updated if a newer version of GiBUU is 
#		installed.
#	PATH [optional]
#		Adds nuwro/bin to path. If novasoft isn't set up, also adds 
#		$GENIE/bin to path
#	LD_LIBRARY_PATH [optional]
#		Adds two NuWro subdirectories to the LD library path, plus another 
#		optional location of Pythia 6. If novasoft is not set up, also adds 
#		$GENIE/lib to LD library path
#	GENIE (optional)
#		If novasoft is not set up, assign this variable to be the location 
#		of the GENIE generator module within CONVENIENT.
#	LHAPDF5_INC (optional)
#		If novasoft is not set up, assign this variable to be the location 
#		of the headers needed for LHAPDF
# 	LHAPDF5_LIB (optional)
#		If novasoft is not set up, assign this variable to be the location
#		of the LHAPDF module library
#	XSECSPLINEDIR (optional)
#		If novasoft is not set up, assign this variable to be the location 
#		of the GENIE xsec splines to use. If novasoft is set up, this is 
#		automatically assigned.

# Sources
# -------
#	$CONVENIENT_GEN_DIR/neut/build/Linux/setup.sh
#		Script for setting up NEUT, as provided by NEUT.
#	$CONVENIENT_GEN_DIR/nuisance_GENIEv3_04_00_nuwro_neut/build/Linux/setup.sh
#		Script for setting up NUISANCE, as provided by NUISANCE.

#!/bin/bash

# GENIE. Don't need to do if novasoft is setup
if [ -z "$NOVA_RELEASE" ];
then
	export GENIE=$CONVENIENT_GEN_DIR/genie/Generator
	export PYTHIA6=$PYTHIA_FQ_DIR/lib
	export LHAPDF5_INC=$LHAPDF_INC
	export LHAPDF5_LIB=$LHAPDF_LIB
	export XSECSPLINEDIR=$GENIEBASE/data
	export PATH=$GENIE/bin:$PATH
	export LD_LIBRARY_PATH=$GENIE/lib:$LD_LIBRARY_PATH
fi

# NuWro

if [[ $NUWRO_SETUP == "ON" ]]
then
	source $CONVENIENT_GEN_BUILD_DIR/nuwro_env.sh
fi

# NEUT
if [[ $NEUT_SETUP == "ON" ]]
then
	source $CONVENIENT_GEN_BUILD_DIR/neut_env.sh
fi

# GiBUU
if [[ $GIBUU_SETUP == "ON" ]]
then
	source $CONVENIENT_GEN_BUILD_DIR/gibuu_env.sh
fi

# NUISANCE
source $CONVENIENT_GEN_BUILD_DIR/nuisance_GENIEv3_04_00_nuwro_neut_env.sh
