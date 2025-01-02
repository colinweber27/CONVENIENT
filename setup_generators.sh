# Author: Colin Weber (webe1077@umn.edu)
# Date: 31 January 2024
# Purpose: To set up the environment for the generators

# Command: source setup_generators.sh

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
source $CONVENIENT_GEN_BUILD_DIR/nuwro_env.sh

# NEUT
source $CONVENIENT_GEN_BUILD_DIR/neut_env.sh

# GiBUU
source $CONVENIENT_GEN_BUILD_DIR/gibuu_env.sh

# NUISANCE
source $CONVENIENT_GEN_DIR/nuisance_GENIEv3_04_00_nuwro_neut/build/Linux/setup.sh

