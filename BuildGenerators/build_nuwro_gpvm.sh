# Author: Colin Weber (webe1077@umn.edu)
# Date: 31 January 2024
# Purpose: To build NuWro within Convenient. This file is written in the 
# style of build_genie_gpvm.sh.

# Command ./build_nuwro_gpvm.sh

#!/bin/bash

# Function that writes a script that can then be used to source environment 
# variables for running NuWro.
write_nuwro_env_script() {
cat > ./nuwro_env.sh << 'EOF'
#!/bin/bash

# Set up the UPS products needed to build and use NuWro
source $CONVENIENT_DIR/global_vars.sh

echo "Setting NuWro environment variables..."

# Finds the directory where this script is located. This method isn't
# foolproof. See https://stackoverflow.com/a/246128/4081973 if you need
# something more robust for edge cases (e.g., you're calling the script using
# symlinks).
THIS_DIRECTORY="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# Export variables needed for running NuWro.
export NUWROBASE=$CONVENIENT_GEN_DIR/Generators/nuwro
export PYTHIA6=$PYTHIA6_LIBRARY
export NUWRO=$NUWROBASE
export LD_LIBRARY_PATH=$CONVENIENT_GEN_BUILD_DIR/pythia6:$NUWRO/lib:$NUWRO/bin:$LD_LIBRARY_PATH
export PATH=$NUWRO/bin:$PATH
EOF
}

# Set the NuWro directory.
export NUWRO_DIR=$CONVENIENT_GEN_DIR/nuwro

# Get the latest version of NuWro.
git clone https://github.com/NuWro/nuwro.git $NUWRO_DIR

# Write the environment script above. This can then be used to setup the 
# NuWro environment after building.
write_nuwro_env_script

# Cd into the Nuwro directory
cd $NUWRO_DIR

# Export variables that are necessary for setting up NuWro properly. These 
# are not needed after NuWro has been built.
export PATH=$PATH:$ROOTSYS/bin
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$ROOTSYS/lib
export PYTHIA6=${PYTHIA_FQ_DIR}/lib
export LIBRARY_PATH=${LIBRARY_PATH}:${PYTHIA6}

# Make (build) NuWro.
make

# Cd back into this directory and source the NuWro environment.
cd $CONVENIENT_GEN_BUILD_DIR
source nuwro_env.sh
echo "DONE!"
