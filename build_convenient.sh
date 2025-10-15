# Author: Colin Weber (webe1077@umn.edu)
# Date: 14 January 2025
# Purpose: To build all the generators in Convenient following a user query 
# as to which generators should be built. Should be run from within the root 
# CONVENIENT directory. 

# Command: source build_convenient.sh

# Exports:
#	CONVENIENT_DIR: the root Convenient directory

# Declares:
#	nuwro_opt: switch for building NuWro. Must be one of {'ON', 'OFF'}.
#	neut_opt: switch for building NEUT. Must be one of {'ON', 'OFF'}.
#	gibuu_opt: switch for building GiBUU. Must be one of {'ON', 'OFF'}.

# Optional declares:

#	Options activated if neut_opt == 'ON'
#	github_username: 			the GitHub username associated with neut_dev
#	github_NEUT_access_token: 	a github access token associated with the 
#								given GitHub username
#	NEUT_version:				The NEUT version to build

# Options activated if nuwro_opt == 'ON'
#	NuWro_version:				The NuWro version to build

# Sources:
#	global_vars.sh: 			various variables primarily labelling 
#								different CONVENIENT directories relative to 
#								the root directory.
#	build_generators_gpvm.sh: 	Shell script for building the generators, 
#								based on the user-inputted options queried 
#								in this script

# She-bang!
#!/bin.bash

export CONVENIENT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

source global_vars.sh

# Prompt the user to decide which generators to set up
# NuWro?
read -p "Enable NuWro? (ON/OFF): " nuwro_opt
if [[ $nuwro_opt == "ON" ]]
then
	echo -e "Please enter the NuWro version to build. \nAvailable versions found on https://github.com/NuWro/nuwro/tags"
	read -p "NuWro version: " NuWro_version
	sed -i "/export NUWRO_SETUP=/s/OFF/ON/" setup_convenient.sh
	export NUWRO_SETUP="ON"
fi

# NEUT?
read -p "Enable NEUT? (ON/OFF). If \"ON\", then also enter GitHub username that is associated with NEUT, and a valid access token: " neut_opt
if [[ $neut_opt == "ON" ]]
then
	read -p "Github Username: " github_username
	read -p "Github access token: " github_NEUT_access_token
	echo -e "Please enter the NEUT version to build. \nUse form X.X.X., e.g. 5.9.0\nAvailable versions found on https://github.com/neut-devel/neut/tags"
	read -p "NEUT version: " NEUT_version
	sed -i "/export NEUT_SETUP=/s/OFF/ON/" setup_convenient.sh
	export NEUT_SETUP="ON"
fi

# GiBUU?
read -p "Enable GiBUU? (ON/OFF): " gibuu_opt
if [[ $gibuu_opt == "ON" ]]
then
	echo -e "Please enter the GiBUU version to build. \nUse form YEAR_PATCH, e.g. 2023_03\nAvailable versions found on https://gibuu.hepforge.org/downloads?f=archive"
	read -p "GiBUU version: " GiBUU_version
	sed -i "/export GIBUU_SETUP=/s/OFF/ON/" setup_convenient.sh
	export GIBUU_SETUP="ON"
fi

cd BuildGenerators

source build_generators_gpvm.sh

cd $CONVENIENT_DIR
