#!/bin/bash

# Set up the UPS products needed to build and use NuWro
source $CONVENIENT_DIR/global_vars.sh

echo "Setting NEUT environment variables..."

export NEUT_DIR=$CONVENIENT_GEN_DIR/neut
source $NEUT_DIR/build/Linux/setup.sh
