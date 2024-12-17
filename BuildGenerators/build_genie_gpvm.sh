# Author: Colin Weber (webe1077@umn.edu)
# Date: 26 March 2024
# Purpose: to clone and build the GENIE Generator
# on the gpvm machines. The original version of this is from Afro at 
# https://github.com/afropapp13/BuildEventGenerators/blob/master/\
# build_genie.sh. It was modified for NOvA by Amit Pal at 
# /exp/nova/app/users/apal/XSec_ND/mc/GENIE/build_genie_gpvm.sh, and I've 
# adapted it to my environment. Convenient currently doesn't set up GENIE 
# using this script, but instead uses the GENIE available in novasoft. As 
# such, this is very much a work in progress.

# Command: ./build_genie_gpvm.sh \
#	[-v version] [-R release]


#!/bin/bash

# Write the GENIE environment script. This can then be called to set all 
# appropriate environment variables when sourcing after the build.
write_genie_env_script() {
cat > ./genie_env.sh << 'EOF'
#!/bin/bash

# Set up the UPS products needed to build and use GENIE
source $CONVENIENT_DIR/build/global_vars.sh

echo "Setting GENIE environment variables..."

# Finds the directory where this script is located. This method isn't
# foolproof. See https://stackoverflow.com/a/246128/4081973 if you need
# something more robust for edge cases (e.g., you're calling the script using
# symlinks).
THIS_DIRECTORY="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# Export all variables needed to run GENIE. Most of these are the locations 
# of executables for different dependencies. The last two add GENIE to 
# PATH.
export GENIEBASE="$(dirname $THIS_DIRECTORY)"/Generators/genie
export GENIE=$GENIEBASE/Generator
export PYTHIA6=$PYTHIA_FQ_DIR/lib
export LHAPDF5_INC=$LHAPDF_INC
export LHAPDF5_LIB=$LHAPDF_LIB
export XSECSPLINEDIR=$GENIEBASE/data
export PATH=$GENIE/bin:$PATH
export LD_LIBRARY_PATH=$GENIE/lib:$LD_LIBRARY_PATH
unset GENIEBASE
EOF
}

# Write a script to call that will call GENIE's configure command.
write_do_configure_script() {
cat > ./do_configure.sh << 'EOF'
#!/bin/bash

# Source the environment variables using the previously written script.
source ../../../build/BuildGenerators/genie_env.sh

# Config
./configure \
  --enable-gsl \
  --enable-rwght \
  --with-optimiz-level=O2 \
  --with-log4cpp-inc=${LOG4CPP_INC} \
  --with-log4cpp-lib=${LOG4CPP_LIB} \
  --with-libxml2-inc=${LIBXML2_INC} \
  --with-libxml2-lib=${LIBXML2_LIB} \
  --with-lhapdf5-lib=${LHAPDF_LIB} \
  --with-lhapdf5-inc=${LHAPDF_INC} \
  --with-pythia6-lib=${PYTHIA_LIB} \
  --enable-numi \
  --enable-fnal
EOF
}

# Set the main GENIE directory
export GENIE_DIR=$CONVENIENT_GEN_DIR/genie

# Get the source code
git clone https://github.com/GENIE-MC/Generator.git $GENIE_DIR/Generator

# Write the environment script, then cd into the GENIE Generator directory
write_genie_env_script
cd $GENIE_DIR/Generator

# Checkout the version and release inputted from the command line
git checkout -b $2 $4

# Write the script to call GENIE's configure shell
write_do_configure_script

# Configure and make GENIE. Want to use source here because do_configure.sh 
# sources the environment as well, which we want done before make.
source do_configure.sh
make -j4

# Cd into this directory and source the environment one last time for good 
# measure.
cd $CONVENIENT_GEN_BUILD_DIR
source genie_env.sh
echo "DONE!"
