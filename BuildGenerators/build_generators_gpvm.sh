# Author: Colin Weber (webe1077@umn.edu)
# Date: 31 January 2024
# Purpose: To build event generators within Convenient. This mostly 
# serves as a place to collect the build scripts for the different 
# generators. It should always be run from within Convenient

# Command: ./build_generators_gpvm.sh

#!/bin/bash

# Build GENIE. Don't need to do if novasoft is set up
if [ -z "$NOVA_RELEASE" ];
then
	./build_genie_gpvm.sh
fi

./build_nuwro_gpvm.sh

./build_neut_gpvm.sh

./build_gibuu_gpvm.sh

./build_nuisance_GENIEv3_04_00_nuwro_neut_gpvm.sh

./build_nuisance_GENIEv3_00_06_nuwro_neut_gpvm.sh

./build_nuisance_GENIEv2_12_10_nuwro_neut_gpvm.sh
