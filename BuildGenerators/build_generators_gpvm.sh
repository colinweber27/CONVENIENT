# Author: Colin Weber (webe1077@umn.edu)
# Date: 31 January 2024
# Purpose: A wrapper to run all of the different build scripts in the 
# BuildGenerators directory.

# Command: source build_generators_gpvm.sh

# Sources:
#	build_nuisance_GENIEv3_04_00_nuwro_neut_gpvm.sh:
#		Builds nuisance against GENIE v3_04_00, with options for building 
#		NuWro and NEUT as well.
#	build_nuisance_GENIEv3_00_06_nuwro_neut_gpvm.sh:
#		Builds nuisance against GENIE v3_00_06, with options for building 
#		NuWro and NEUT as well.
#	add_to_root_macropath.sh:
#		Adds necessary directories to ROOT MacroPath

# Optional sources:
# Options activated by quering the variables NOVA_RELEASE, nuwro_opt, 
# neut_opt, and gibuu_opt. NOVA_RELEASE is set by setting up NOvA, and if 
# NOvA hasn't been set up (meaning NOVA_RELEASE is empty), then GENIE is 
# built. The other variables are set by sourcing 
# CONVENIENT/build_convenient.sh
#	build_genie_gpvm.sh: builds GENIE if NOvA hasn't been set up
#	build_nuwro_gpvm.sh: builds NuWro if nuwro_opt == 'ON'
#	build_neut_gpvm.sh: builds NEUT if neut_opt == 'ON'
#	build_gibuu_gpvm: builds GiBUU if gibuu_opt == 'ON'

#!/bin/bash

# Build GENIE. Don't need to do if novasoft is set up
if [ -z "$NOVA_RELEASE" ];
then
	source build_genie_gpvm.sh
fi

if [[ $nuwro_opt == "ON" ]]
then
	source build_nuwro_gpvm.sh --VERSION $NuWro_version
fi

if [[ $neut_opt == "ON" ]]
then
	source build_neut_gpvm.sh --USERNAME $github_username --TOKEN $github_NEUT_access_token --VERSION $NEUT_version
fi

if [[ $gibuu_opt == "ON" ]] 
then
	source build_gibuu_gpvm.sh --VERSION $GiBUU_version
fi

source build_nuisance_GENIEv3_04_00_nuwro_neut_gpvm.sh --NuWro $nuwro_opt --NEUT $neut_opt

source build_nuisance_GENIEv3_00_06_nuwro_neut_gpvm.sh --NuWro $nuwro_opt --NEUT $neut_opt

source add_to_root_macropath.sh
