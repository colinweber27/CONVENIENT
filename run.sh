# Author: Colin M Weber (webe1077@umn.edu)
# Date: 7 August 2023
# Purpose: This shell runs/is Convenient. 
# It takes as input the number of events, neutrino energy spectra and 
# flavors, target composition, seed, and a list of event generators to run, 
# then runs the generators and processes the outputs, putting them in a 
# useful format.

# Command: source run.sh
# Note that in novasoft, source must be the command we use to execute the 
# shell script because this allows us to setup the different versions of 
# GENIE with UPS.

# Sources
# -------
#	conveniently_run_*.sh
#		This script parses the generator configurations set by "RUNS" in 
#		CONVENIENT/set_run_variables.sh to determine which generators to run 
#		for a given CONVENIENT run. This script then runs the appropriate 
#		generators with the "conveniently_run_*.sh scripts. The * is one of 
#		{GENIE, NuWro, NEUT, GiBUU}.

# Outputs
# -------
#	A set of CONVENIENT files depending on the run variables, with a .txt file corresponding to each CONVENIENT file describing how the file was created. 

# She-bang!
#!/bin/bash

# Tell us we've begun
echo "Convenient run begun."

## Figure out which generators to run, and with which configurations/tunes
echo "Parsing generator list..."

# Separate list by commas
IFS=','

# Read the list of runs into an array, where each item is of the form 
# GENERATOR:tune/params list
generator_list=''
config_list=''
flux_list=''
flux_file_name_list=''
flux_histo_list=''
seed_list=''
target_list=''

read -a runs <<< "$RUNS"
for run in "${runs[@]}" # For each run
do
	generator=${run%:*} # To get the generator, remove everything after the :
	generator_list+="$generator " # Append to the generator list
	config=${run#*:} # To get the config, remove everything before the :
	config_list+="$config " # Append to the appropriate list
done
read -a fluxes <<< "$FLUXES"
for flux in "${fluxes[@]}"
do
	flux_list+="$flux "
done
read -a flux_file_names <<< "$FLUX_FILE_NAMES"
for flux_file_name in "${flux_file_names[@]}"
do
	flux_file_name_list+="$flux_file_name "
done
read -a flux_histos <<< "$FLUX_HISTOS"
for flux_histo in "${flux_histos[@]}"
do
	flux_histo_list+="$flux_histo "
done
read -a seeds <<< "$SEEDS"
for seed in "${seeds[@]}"
do
	seed_list+="$seed "
done
read -a targets <<< "$TARGETS"
for Target in "${targets[@]}"
do
	target_list+="$Target "
done
IFS=' '
echo "Run variables parsed."

## Run through the list
# Create an array of generators
read -a generator_array <<< $generator_list

# Create a corresponding array of configurations (tunes and parameter files)
read -a config_array <<< $config_list

read -a flux_array <<< $flux_list
read -a flux_file_name_array <<< $flux_file_name_list
read -a flux_histo_array <<< $flux_histo_list

read -a seed_array <<< $seed_list

read -a target_array <<< $target_list

# Check that all these strings are of equal length
lengths=(${#config_array[@]} ${#flux_array[@]} ${#flux_file_name_array[@]} ${#flux_histo_array[@]} ${#seed_array[@]} ${#target_array[@]})
for i in "${lengths[@]:1}"; do
	if [ "$i" != "${lengths[0]}" ]; then
		echo "Not all lists of inputs have the same length."
		echo "Ensure that all comma-separated lists in 'set_run_variables.sh'"
		echo "have the same number of elements."
		exit 1
	fi
done

# Loop through the generators and configurations
for ((j=0; j<${#generator_array[@]}; j++))
do
	# Grab the generator and configuration
	generator=${generator_array[$j]}
	config=${config_array[$j]}

	flux=${flux_array[$j]}
	flux_file_name=${flux_file_name_array[$j]}
	flux_histo=${flux_histo_array[$j]}
	
	flux_file=$CONVENIENT_FLUX_DIR/NuMI/$HC/$flux/$flux_file_name

	seed=${seed_array[$j]}

	Target=$CONVENIENT_TAR_DIR/${target_array[$j]}

	# Depending on the generator, execute the appropriate commands
	case $generator in
		GENIE)
			cd GENIE
			source conveniently_run_genie.sh \
				--config $config --flux_file $flux_file \
				--flux_histo $flux_histo --seed $seed --target $Target
			cd ../

			# Write the accompanying text file describing the inputs of the 
			# new file. All inputs refer to the values that were used to 
			# create the output.
			# Format is
			# -l filepath,convenient filename -n number of events 
			# -h horn current polarity (caps) -p neutrino pdg 
			# -f flux file,flux histogram
			# -t path to file containing target composition in GENIE form
			# -d date in yyyymmddhhmmss format -v generator version 
			# --seed MC seed
			echo "Generating output .txt file..."
			bash documentation_generation_scripts/create_output_txt_file.sh \
				-l $convenient_output_dir/$filepath,$filename_convenient \
				-n $N_EVENTS \
				-h $HC \
				-p $NEUTRINO_PDG \
				-f $flux_file,$flux_histo \
				-t $Target \
				-d $DATE \
				-v $generator_version \
				--seed $seed
			echo "Output .txt file generated."
			# Add the run to the data list using add_to_data_list.sh
			# Format is 
			# -g generator -t tune/parameter file -f filename --flux flux
			# --nova_switch 1=used ND element weights
			echo "Adding run to data list..."
			bash documentation_generation_scripts/add_to_data_list.sh \
				-g GENIE -t $config \
				-f $filename_convenient \
				--flux $flux --nova_switch $nova_switch
			echo "Run added to data list."	
			;;

		NuWro)
			cd NuWro
			source conveniently_run_nuwro.sh \
				--config $config --flux_file $flux_file \
				--flux_histo $flux_histo --seed $seed --target $Target
			cd ../
			
			# Write the accompanying text file describing the inputs of the 
			# new file. All inputs refer to the values that were used to 
			# create the output.
			# Format is
			# -l filepath,convenient filename -n number of events 
			# -h horn current polarity (caps) -p neutrino pdg 
			# -f flux file,flux histogram
			# -t path to file containing target composition in GENIE form
			# -d date in yyyymmddhhmmss format -v generator version 
			# --seed MC seed
			echo "Creating output .txt file..."
			bash documentation_generation_scripts/create_output_txt_file.sh \
				-l $convenient_output_dir/$filepath,$filename_convenient \
				-n $N_EVENTS \
				-h $HC \
				-p $NEUTRINO_PDG \
				-f $flux_file,$flux_histo \
				-t $Target \
				-d $DATE \
				-v $NUWRO_VERSION \
				--seed $seed
			echo "Output .txt file created."
			# Add the run to the data list using add_to_data_list.sh
			# Format is 
			# -g generator -t tune/parameter file -f filename --flux flux
			# --nova_switch 1=used ND element weights
			echo "Adding run to data list..."
			bash documentation_generation_scripts/add_to_data_list.sh \
				-g NuWro -t $config \
				-f $filename_convenient \
				--flux $flux --nova_switch $nova_switch
			echo "Run added to data list."
			;;

		NEUT)
			cd NEUT
			source conveniently_run_neut.sh \
				--config $config --flux_file $flux_file \
				--flux_histo $flux_histo --seed $seed --target $Target
			cd ../	
			
			# Write the accompanying text file describing the inputs of the 
			# new file. All inputs refer to the values that were used to 
			# create the output.
			# Format is
			# -l filepath,convenient filename -n number of events 
			# -h horn current polarity (caps) -p neutrino pdg 
			# -f flux file,flux histogram
			# -t path to file containing target composition in GENIE form
			# -d date in yyyymmddhhmmss format -v generator version 
			# --seed MC seed
			echo "Creating output .txt file..."
			bash documentation_generation_scripts/create_output_txt_file.sh \
				-l $convenient_output_dir/$filepath,$filename_convenient \
				-n $N_EVENTS \
				-h $HC \
				-p $NEUTRINO_PDG \
				-f $flux_file,$flux_histo \
				-t $neut_target \
				-d $DATE \
				-v $NEUT_VERSION \
				--seed $seed
			echo "Output .txt file created."
			# Add the run to the data list using add_to_data_list.sh
			# Format is 
			# -g generator -t tune/parameter file -f filename --flux flux
			# --nova_switch 1=used ND element weights
			echo "Adding run to data list..."
			bash documentation_generation_scripts/add_to_data_list.sh \
				-g $generator -t $config \
				-f $filename_convenient \
				--flux $flux --nova_switch $nova_switch
			echo "Run added to data list."
			;;

		GiBUU)
			cd GiBUU
			source conveniently_run_gibuu.sh \
				--config $config --flux_file $flux_file \
				--flux_histo $flux_histo --seed $seed --target $Target
			cd ../	
	
			# Write the accompanying text file describing the inputs of the 
			# new file. All inputs refer to the values that were used to 
			# create the output.
			# Format is
			# -l filepath,convenient filename -n number of events 
			# -h horn current polarity (caps) -p neutrino pdg 
			# -f flux file,flux histogram
			# -t path to file containing target composition in GENIE form
			# -d date in yyyymmddhhmmss format -v generator version 
			# --seed MC seed
			echo "Creating output .txt file..."
			bash documentation_generation_scripts/create_output_txt_file.sh \
				-l $convenient_output_dir/$filepath,$filename_convenient \
				-n $N_EVENTS \
				-h $HC \
				-p $NEUTRINO_PDG,$GIBUU_CC_NC \
				-f $flux_file,$flux_histo \
				-t $gibuu_target \
				-d $DATE \
				-v $GiBUU_VERSION \
				--seed $seed
			echo "Output .txt file created."
			# Add the run to the data list using add_to_data_list.sh
			# Format is 
			# -g generator -t tune/parameter file -f filename --flux flux
			# --nova_switch 1=used ND element weights
			echo "Adding run to data list..."
			bash documentation_generation_scripts/add_to_data_list.sh \
				-g $generator -t $config \
				-f $filename_convenient \
				--flux $flux --nova_switch $nova_switch
			echo "Run added to data list."
			;;

		*)
			echo "Error: Unknown generator input."
			exit 1
			;;
	esac
done

# Let us know we're done
echo "Convenient run complete."
