# Author: Colin Weber (webe1077@umn.edu)
# Date: 30 May 2024
# Purpose: To build GiBUU within Convenient. This file is written in the 
# style of build_genie_gpvm.sh.

# Command ./build_gibuu_gpvm.sh

#!/bin/bash

# Function that writes a script that can then be used to source environment 
# variables for running GiBUU.
write_gibuu_env_script() {
cat > ./gibuu_env.sh << 'EOF'
#!/bin/bash

# Set up the UPS products needed to build and use GiBUU
source $CONVENIENT_DIR/global_vars.sh

echo "Setting GiBUU environment variables..."

export GiBUU=$CONVENIENT_DIR/GiBUU/GiBUU.x

export GiBUU_BUU_INPUT=$CONVENIENT_GEN_DIR/GiBUU/buuinput

export GiBUU_VERSION=2023
EOF
}

# Write the environment script above. This can then be used to setup the 
# GiBUU environment after building.
write_gibuu_env_script

# Set the GiBUU directory and cd into it
export GiBUU_DIR=$CONVENIENT_GEN_DIR/GiBUU
mkdir -p $GiBUU_DIR
cd $GiBUU_DIR

# Download and unpack GiBUU.
wget --content-disposition https://gibuu.hepforge.org/downloads?f=release2025.tar.gz
wget --content-disposition https://gibuu.hepforge.org/downloads?f=buuinput2025.tar.gz

tar -xzvf buuinput2025.tar.gz
tar -xzvf release2025.tar.gz

# Cd into the release directory to build, then run make
cd release
make

# Move the executable to Convenient/GiBUU
mv testRun/GiBUU.x $CONVENIENT_DIR/GiBUU/GiBUU.x

# Cd back into this directory and source the GiBUU environment.
cd $CONVENIENT_GEN_BUILD_DIR
source gibuu_env.sh
echo "DONE!"
