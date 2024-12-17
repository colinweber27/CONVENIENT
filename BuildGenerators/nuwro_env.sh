#!/bin/bash

# Set up the UPS products needed to build and use NuWro
source $CONVENIENT_DIR/global_vars.sh

echo "Setting NuWro environment variables..."

# Finds the directory where this script is located. This method isn't
# foolproof. See https://stackoverflow.com/a/246128/4081973 if you need
# something more robust for edge cases (e.g., you're calling the script using
# symlinks).
THIS_DIRECTORY="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

export NUWROBASE="$(dirname $THIS_DIRECTORY)"/Generators/nuwro
export PYTHIA6=$PYTHIA6_LIBRARY
export NUWRO=$NUWROBASE
export LD_LIBRARY_PATH=$CONVENIENT_GEN_BUILD_DIR/pythia6:$NUWRO/lib:$NUWRO/bin:$LD_LIBRARY_PATH
export PATH=$NUWRO/bin:$PATH
