# Author: Colin Weber (webe1077@umn.edu)
# Date: 8 April 2024
# Purpose: To build NUISANCE w/ GENIE v3_00_06 and NuWro within Convenient
# Assumes GENIE v3_00_06 has already been set up via ups in the novasoft
# environment, NuWro has been built (but not necessarily set up), and NEUT 
# has been built.

# Command: ./build_nuisance_GENIEv3_00_06_nuwro_neut_gpvm.sh

# Sources
#	$CONVENIENT_DIR/setup_generators.sh
#		Script to set all environment variables necessary for running the 
#		various generators. NUISANCE uses these variables to determine which
#		generator versions to build against.
#	nuisance_GENIEv3_00_06_env.sh
#		Script to source variables necessary for running this version of 
#		NUISANCE

# Outputs
#	nuisance_GENIEv3_00_06_env.sh
#		Script to source variables necessary for running this version of 
#		NUISANCE

# Write a script holding all the environment variables, such that sourcing 
# this script properly sets up the NUISANCE environment.
write_nuisance_GENIEv3_00_06_nuwro_neut_env_script() {
cat > ./nuisance_GENIEv3_00_06_nuwro_neut_env.sh << 'EOF'
# Purpose: to source the environment variables necessary for running 
# NUISANCE within CONVENIENT

# Command: source nuisance_GENIEv3_00_06_env.sh

# Exports
#	NUISANCEBASE: the directory within which NUISANCE is built

# Sources
#	$CONVENIENT_DIR/global_vars.sh
#		Environment variables that name different CONVENIENT directories
#	$NUISANCEBASE/build/Linux/setup.sh
#		Environment variables necessary for running NUISANCE
#!/bin/bash

# Set up the UPS products needed to build and use Nuisance
source $CONVENIENT_DIR/global_vars.sh

echo "Setting NUISANCE environment variables..."

# Finds the directory where this script is located. This method isn't
# foolproof. See https://stackoverflow.com/a/246128/4081973 if you need
# something more robust for edge cases (e.g., you're calling the script using
# symlinks).
THIS_DIRECTORY="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# Set the NUISANCE base directory and source the setup.sh script within the
# base directory.
export NUISANCEBASE=$CONVENIENT_GEN_DIR/nuisance_GENIEv3_00_06_nuwro_neut
source $NUISANCEBASE/build/Linux/setup.sh
EOF
}

# Ensure all generators are set up, including the right version of GENIE
source $CONVENIENT_DIR/setup_generators.sh
setup genie v3_00_06p -q e20:inclxx:prof
setup genie_xsec v3_00_06 -q G1810j00000:k250:e1000
setup genie_phyopt v3_00_06 -q dkcharmtau:resfix

# Set the NUISANCE directory variable.
export NUISANCE_DIR=$CONVENIENT_GEN_DIR/nuisance_GENIEv3_00_06_nuwro_neut

# Get latest NUISANCE version.
git clone https://github.com/NUISANCEMC/nuisance.git $NUISANCE_DIR

# Write the environment script above, and cd into the NUISANCE directory
write_nuisance_GENIEv3_00_06_nuwro_neut_env_script
cd $NUISANCE_DIR

# Slightly adjust the GENIE environment variable
GENIE=${GENIE%\/*}

# Make the build directory, cd into it, and use cmake to write the install 
# script.
mkdir -p build
cd build
cmake -DUSE_MINIMIZER=1 -DNuWro_ENABLED=$2 -DGENIE_ENABLED=ON -DEXPERIMENTS_ENABLED=OFF -DNEUT_ENABLED=$4 ../

# Make (build) NUISANCE.
make install

# Cd back to this directory and source the NUISANCE environment for use.
cd $CONVENIENT_GEN_BUILD_DIR
source nuisance_GENIEv3_00_06_nuwro_neut_env.sh
echo "DONE!"
