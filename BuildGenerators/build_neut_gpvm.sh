# Author: Colin Weber (webe1077@umn.edu)
# Date: 26 March 2024
# Purpose: to build the NEUT Generator on the gpvm machines, following the 
# NEUT folks' README.md file. This file is written in the flavor of 
# build_genie_gpvm.sh.
# The most important aspect of building NEUT is that the source code is NOT 
# open-source and is only available to those who are members of the private 
# neut-devel organization on GitHub. I emailed Luke Pickering for access.

# Command: source build_neut_gpvm.sh \
#	[--USERNAME github_username] [--TOKEN github_NEUT_access_token]

# Parameters
#	USERNAME: The GitHub username associated with neut_dev
#	TOKEN: A personal access token associated with USERNAME

# Exports
#	NEUT_DIR: The directory containing the source code for NEUT

# Sources
#	neut_env.sh: the environment variables necessary for running NEUT

# Outputs
#	neut_env.sh: the environment variables necessary for running NEUT

#!/bin/bash

# Set the NEUT directory
export NEUT_DIR=$CONVENIENT_GEN_DIR/neut

# Get the latest version of NEUT from GitHub
# Since neut-devel/neut.git is a private repository, a github username and 
# personal authentication token (PAT) is needed to clone it. The format for 
# this command is 
# git clone https://username:PAT@github.com/neut-devel-neut.git 
# <landing directory>
git clone https://colinweber27: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@github.com/neut-devel/neut.git $NEUT_DIR

# Cd into the NEUT directory
cd $NEUT_DIR

# Do some other stuff that prevents compiling errors. More info can be found 
# in neut-devel/neut/README.md file
cd src
rm aclocal.m4
autoreconf -if

# Make the build directory and cd into it
cd ../; mkdir build; cd build;

# Configure the build
../src/configure --prefix=$(readlink -f Linux)

# Make and install
make -j 8
make install

# Source NEUT
source $NEUT_DIR/build/Linux/setup.sh
