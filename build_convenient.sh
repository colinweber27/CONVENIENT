# She-bang!
#!/bin.bash

export CONVENIENT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

source global_vars.sh

# Prompt the user to decide which generators to set up
# NuWro?
read -p "Enable NuWro? (ON/OFF): " nuwro_opt
# NEUT?
read -p "Enable NEUT? (ON/OFF). If \"ON\", then also enter GitHub username that is associated with NEUT, and a valid access token: " neut_opt
if [[ $neut_opt == "ON" ]]
then
	read -p "Github Username: " github_username
	read -p "Github access token: " github_NEUT_access_token
fi
# GiBUU?
read -p "Enable GiBUU? (ON/OFF): " gibuu_opt

cd BuildGenerators

source build_generators_gpvm.sh

cd $CONVENIENT_DIR
