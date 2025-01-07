# Author: Colin Weber (webe1077@umn.edu)
# Date: 16 December 2024
# Purpose: this is a wrapper to handle running NEUT within Convenient

# Command: conveniently_run_neut.sh
#	[--config NEUT jobcard] [--flux_file flux file]
#	[--flux_histo flux histo] [--seed seed] [--target target]

# She-bang!
#!/bin/bash

# Read the inputs
neut_config=$2
neut_flux_file=$4
neut_flux=${neut_flux_file%\/*}
neut_flux=${neut_flux##*\/}
neut_flux_histo=$6
neut_seed=$8
genie_target=${10}
genie_rel_target=${genie_target/$CONVENIENT_TAR_DIR\//}

# In $config, separate out params file from directory
params=${neut_config#*/} # Params is everything after the /

# Check the target. This will affect the filenames
if [ ${genie_rel_target:0:4} == "NOvA" ];
then
	nova_switch=1
	convenient_output_dir=$CONVENIENT_NOvA_OUTPUT_DIR
	neut_target=${genie_target/GENIE/NEUT}
else
	nova_switch=0
	target_txt=${genie_rel_target#*elements/}
	target=${target_txt%.txt}
	convenient_output_dir=$CONVENIENT_OUTPUT_DIR
	neut_target=$genie_target
fi
filename="$params.$neut_seed.$HC.${N_EVENTS:0:1}m.$FLAVOR"
nuisance_output_dir=$CONVENIENT_NUISANCE_OUTPUT_DIR

# Set the output file names
filename_raw=$filename.raw.root
filename_nuisance=NEUT:$filename.NUISANCE.root

# Set the run variables. This includes the NEUT output filename
source set_neut_variables.sh -s $neut_seed

# Tell us what tune we're on
echo "Generating and processing NEUT w/ params $params..."

# Run NEUT
# Format is
	# -i input parameters file
	# -o output file name -n number of events 
	# -p neutrino pdg --flux_file flux file 
	# --flux_histo flux histo
	# -t target composition file in NEUT format
bash run_neut.sh \
	-i $neut_config \
	-o $filename_raw \
	-n $N_EVENTS \
	-p $NEUTRINO_PDG \
	--flux_file $neut_flux_file \
	--flux_histo $neut_flux_histo \
	-t $neut_target

# Run NUISANCE on the output from above.
# Format is (for a beam with only one neutrino flavor)
	# -i input file name
	# -f flux file
	# -h flux histo
# Ouptut names are handled within run_neut_nuisance 
source $CONVENIENT_NUISANCE_DIR/set_nuisance_variables.sh

bash $CONVENIENT_NUISANCE_DIR/run_neut_nuisance.sh \
	-i $filename_raw \
	-f $neut_flux_file \
	-h $neut_flux_histo

# Post-process the NUISANCE output to output the Convenient file
# We want a Convenient file for each NUISANCE file, plus an 
# aggregate Convenient file in which the relative abundances of 
# different nuclei are accounted for in the event weights, if 
# the target is a mixture.
if [ $nova_switch -eq 0 ];
then
	raw_neut_file=$(ls $filename_raw*)
	nuisance_neut_file=NEUT:${raw_neut_file/raw/NUISANCE}
	filename_convenient=NEUT:${raw_neut_file/raw/convenient_output}
	root -q "${CONVENIENT_NUISANCE_DIR}/make_convenient_from_nuisance.C(\"${nuisance_neut_file}\", \"${filename_convenient}\", 1)"
else
	# First create a dictionary where the keys are the elements 
	# in the mixture, and the values are the fractional 
	# composition of the mixture
	# Initialize the dictionary
	filename_convenient=NEUT:$filename.convenient_output.root
	declare -A fractional_compositions
	# Read in the target info from the neut target .txt file
	while read line;
	do
		neut_target_comp=$(echo $line)
	done < "${neut_target}"
	# Change the IFS to a comma, which we use for parsing the 
	# target content input
	IFS=","
	# Loop over the elements and add each to the dictionary. 
	read -a elements_fracs <<< "$neut_target_comp"
	for element_frac in "${elements_fracs[@]}"
	do
		# Get the element
		element=${element_frac%[*}
		# Get the fractional composition
		frac=${element_frac#*[}
		frac=${frac%]*}
		# Add to the dictionary
		fractional_compositions[$element]=$frac
	done
	IFS=' '
	# For each NUISANCE file, create an unweighted Convenient 
	# output and a Convenient output weighted by the fractional 
	# composition. Also keep track of the weighted Convenient 
	# outputs and unweighted Convenient outputs
	unweighted_convenient_outputs=""
	weighted_convenient_outputs=""
	while IFS= read -r nuisance_neut_file
	do
		# Get the element that is the target for the file
		element_from_filename=${nuisance_neut_file/$filename_nuisance./}
		element_from_filename=${element_from_filename/.root/}
		# Get the weight for that element from the dictionary
		weight=${fractional_compositions[$element_from_filename]}
		# Construct the output name for the unweighted 
		# Convenient file, and run Convenient
		convenient_elemental_output=${nuisance_neut_file/NUISANCE/convenient_output}
		root -q "${CONVENIENT_NUISANCE_DIR}/make_convenient_from_nuisance.C(\"${nuisance_neut_file}\", \"${convenient_elemental_output}\", 1)"
		unweighted_convenient_outputs+="$convenient_elemental_output"
		unweighted_convenient_outputs+=$'\n'
		# Construct the output name for the weighted Convenient 
		# file, and run Convenient w/ the appropriate weight
		convenient_weighted_output=${convenient_elemental_output/.$element_from_filename./.$element_from_filename.$weight.}
		weighted_convenient_outputs+="$convenient_weighted_output "
		root -q "${CONVENIENT_NUISANCE_DIR}/make_convenient_from_nuisance.C(\"${nuisance_neut_file}\", \"${convenient_weighted_output}\", ${weight})"
	done <<< $(ls $filename_nuisance.*)
	# Combine all the weighted files into one file
	root -q "${CONVENIENT_NUISANCE_DIR}/make_convenient_from_convenient.C(\"${weighted_convenient_outputs}\", \"${filename_convenient}\")"
fi

# Move the output files to the folders matching the program that 
# created them.
echo "NEUT run complete, moving files..."

# Delete the extra NEUT files to save space.
rm $filename_raw.* # Takes care of the raw NEUT outputs
rm "$neut_flux_histo"_o.root # Takes care of this NEUT output
rm random.txt	# Takes care of the random number .txt file

# We want to save the files to either the NOvA specific 
# location, or the general location, depending on which target 
# was used.
if [ $nova_switch -eq 1 ];
then
	# Take care of the Convenient file first
	filepath=NEUT/${params%.card}/"$HC$flux"/"$FLAVOR"_only
	mkdir -p $convenient_output_dir/$filepath
	mv $filename_convenient $convenient_output_dir/$filepath/$filename_convenient

	# Move the NUISANCE files
	while IFS= read -r nuisance_neut_file
	do
		# Get the PDG from the filename
		pdg_from_filename=${nuisance_neut_file/$filename_nuisance./}
		pdg_from_filename=${pdg_from_filename/.root/}

		# Look up the element
		target=$(awk -v key=${pdg_from_filename} '$1==key { print $2 }' $CONVENIENT_TAR_DIR/NOvA_ND/PDG_element_lookup_table.txt)

		# Form the filepath
		filepath=NEUT/${params%.card}/"$HC$flux"/"$FLAVOR"_only/$target
		# Move the file
		outdir=$CONVENIENT_NUISANCE_OUTPUT_DIR/$filepath
		mkdir -p $outdir
		mv $nuisance_neut_file $outdir/$nuisance_neut_file
	done <<< $(ls $filename_nuisance.*)

	# Move the unweighted Convenient files
	while IFS= read -r unweighted_convenient_file
	do
		# Get the PDG from the filename
		pdg_from_filename=${unweighted_convenient_file/$filename_convenient./}
		pdg_from_filename=${pdg_from_filename/.root/}

		# Look up the element
		target=$(awk -v key=${pdg_from_filename} '$1==key { print $2 }' $CONVENIENT_TAR_DIR/NOvA_ND/PDG_element_lookup_table.txt)
		if [[ $target == "" ]];
		then
			break
		fi

		# Form the filepath
		filepath=NEUT/${params%.card}/"$HC$flux"/"$FLAVOR"_only/$target
		# Move the file
		outdir=$CONVENIENT_OUTPUT_DIR/$filepath
		mkdir -p $outdir
		mv $unweighted_convenient_file $outdir/$unweighted_convenient_file
		# Add to data list and write a .txt file
		bash $CONVENIENT_DIR/documentation_generation_scripts/add_to_data_list.sh -g NEUT -t $neut_config -f $unweighted_convenient_file --flux $neut_flux --nova_switch 0
		bash $CONVENIENT_DIR/documentation_generation_scripts/create_output_txt_file.sh -l $outdir,$unweighted_convenient_file -n $N_EVENTS -h $HC -p $NEUTRINO_PDG -f $neut_flux_file,$neut_flux_histo -t $target -d $DATE -v $NEUT_VERSION --seed $neut_seed

	done <<< $unweighted_convenient_outputs

	# Reset the filepath
	filepath=NEUT/${params%.card}/"$HC$flux"/"$FLAVOR"_only

	# Remove the weighted Convenient files
	while IFS= read -r weighted_convenient_file
	do
		rm $weighted_convenient_file
	done <<< $weighted_convenient_outputs
else
	filepath=NEUT/${params%.card}/"$HC$flux"/"$FLAVOR"_only/$target
	# mkdir format is straightforward. 
	# -p means make parent directories as well, if missing
	mkdir -p $nuisance_output_dir/$filepath
	mkdir -p $convenient_output_dir/$filepath
	# mv format is [file to move] [new directory and/or name]
	mv $nuisance_neut_file $nuisance_output_dir/$filepath/$nuisance_neut_file
	mv $filename_convenient $convenient_output_dir/$filepath/$filename_convenient
fi
