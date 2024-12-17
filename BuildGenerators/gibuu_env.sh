#!/bin/bash

# Set up the UPS products needed to build and use GiBUU
source $CONVENIENT_DIR/global_vars.sh

echo "Setting GiBUU environment variables..."

export GiBUU=$CONVENIENT_DIR/GiBUU/GiBUU.x

export GiBUU_BUU_INPUT=$CONVENIENT_GENERATOR_DIR/GiBUU/buuinput

export GiBUU_VERSION=2023

