# Author: Colin Weber (webe1077@umn.edu)
# Date: 16 December 2024
# Purpose: this is a wrapper to handle running GiBUU within Convenient

# Command: conveniently_run_gibuu.sh
#	[--config GiBUU jobcard] [--flux_file flux file]
#	[--flux_histo flux histo] [--seed seed] [--target target]

# Parameters
#	config
#		The .job file that sets the GiBUU configuration. One example is at 
#		https://gibuu.hepforge.org/trac/wiki/jobcards/T2K_0pi_water.
#	flux_file: the ROOT file containing the 1D flux histogram to use as input
#	flux_histo: the 1D histogram within the flux_file containing the flux
#	seed: the MC seed to use
#	target
#		A .txt file containing the target information in GENIE format. 
#		Examples are in the directory CONVENIENT/targets	

# Sources
#	set_gibuu_variables.sh
#		Script that sets variables necessary for running GiBUU

# Outputs
#	$OUTPUT_DIR/RawGibuu/$filepath/$raw_gibuu_file
#		An unweighted GiBUU output file corresponding to interactions with a 
#		single element. One of these is created per element in the target.
#	$convenient_output_dir/$filepath/$unweighted_convenient_file
#		A CONVENIENT file created from the raw GiBUU file for a specific 
# 		element. One of these is made per element in the input.
#	$convenient_output_dir/$filepath/$filename_convenient (optional)
#		The CONVENIENT output file, which includes all of the elements in 
#		the original detector with weights. Only created if running with a 
#		detector with multiple elements.


# She-bang!
#!/bin/bash

# Read the inputs
gibuu_config=$2
gibuu_flux_file=$4
gibuu_flux=${gibuu_flux_file%\/*}
gibuu_flux=${gibuu_flux##*\/}
gibuu_flux_histo=$6
gibuu_seed=$8
genie_target=${10}
genie_rel_target=${genie_target/$CONVENIENT_TAR_DIR\//}

# In $config, separate out params file from directory
params=${gibuu_config#*/} # Params is everything after the /

# Check the target. This will affect the filenames
if [ ${genie_rel_target:0:4} == "NOvA" ];
then
	nova_switch=1
	convenient_output_dir=$CONVENIENT_NOvA_OUTPUT_DIR
	gibuu_target=$genie_target
else
	nova_switch=0
	target_txt=${genie_rel_target#*elements/}
	target=${target_txt%.txt}
	gibuu_target=$genie_target
	convenient_output_dir=$CONVENIENT_OUTPUT_DIR
fi
filename="$params.$gibuu_seed.$HC.$N_EVENTS.$FLAVOR$GIBUU_CC_NC"

# Set the output file names
filename_raw=$filename.raw.root

# Set the run variables
# Format is
	# -f input flux file
	# -h histogram to use in input flux file
source set_gibuu_variables.sh -f $gibuu_flux_file -h $gibuu_flux_histo

# Tell us what tune we're on
echo "Generating and processing GiBUU w/ params $params..."

# Run GiBUU
# Format is
	# -i input jobcard
	# -o output file name -n number of events 
	# -p neutrino pdg -c CC/NC flag 
	# --flux_file gibuu .dat flux file 
	# -t target composition file in GENIE format
	# --seed MC seed
bash run_gibuu.sh \
	-i $gibuu_config \
	-o $filename_raw \
	-n $N_EVENTS \
	-p $NEUTRINO_PDG \
	-c $GIBUU_CC_NC \
	--flux_file $gibuu_dat_flux_file \
	-t $gibuu_target \
	--seed $gibuu_seed

# Post-process the GiBUU output to output the Convenient file
# We want a Convenient file for each GiBUU elemental file, plus 
# an aggregate Convenient file in which the relative abundances 
# of different nuclei are accounted for in the event weights, if 
# the target is a mixture.
if [ $nova_switch -eq 0 ];
then
	raw_gibuu_file=$(ls $filename_raw*)
	filename_convenient=GiBUU:${raw_gibuu_file/raw/convenient_output}
	filename_convenient=${filename_convenient/.dat/.root}
	# Get the nucleus PDG from the filename
	nucleus=${raw_gibuu_file/$filename_raw./}
	nucleus=${nucleus/.dat/}
	# Get the number of protons and nucleons from the nucleus PDG
	protons=${nucleus:3:3}
	protons=$((10#$protons))
	nucleons=${nucleus:6:3}
	nucleons=$((10#$nucleons))				
	root -q "make_convenient_from_gibuu.C(\"${raw_gibuu_file}\", \"${gibuu_dat_flux_file}\", \"${NEUTRINO_PDG}\", \"${GIBUU_CC_NC}\", \"${protons}\", \"${nucleons}\", \"${filename_convenient}\", \"1\")"
else
	# First create a dictionary where the keys are the elements 
	# in the mixture, and the values are the fractional 
	# composition of the mixture
	# Initialize the dictionary
	filename_convenient=GiBUU:$filename.convenient_output.root
	declare -A fractional_compositions
	# Read in the target info from the GiBUU target .txt file
	while read line;
	do
		gibuu_target_comp=$(echo $line)
	done < "${gibuu_target}"
	# Change the IFS to a comma, which we use for parsing the 
	# target content input
	IFS=","
	# Loop over the elements and add each to the dictionary. 
	read -a elements_fracs <<< "$gibuu_target_comp"
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
	# For each GiBUU file, create an unweighted Convenient 
	# output and a Convenient output weighted by the fractional 
	# composition. Also keep track of the weighted Convenient 
	# outputs and unweighted Convenient outputs
	unweighted_convenient_outputs=""
	weighted_convenient_outputs=""
	while IFS= read -r raw_gibuu_file
	do
		# Get the element that is the target for the file
		element_from_filename=${raw_gibuu_file/$filename_raw./}
		element_from_filename=${element_from_filename/.dat/}
		# Get the weight for that element from the dictionary
		weight=${fractional_compositions[$element_from_filename]}
		# Get the number of protons and nucleons for the element
		protons=${element_from_filename:3:3}
		protons=$((10#$protons))
		nucleons=${element_from_filename:6:3}
		nucleons=$((10#$nucleons))
		# Construct the output name for the unweighted 
		# Convenient file, and run Convenient
		convenient_elemental_output=${raw_gibuu_file/raw/convenient_output}
		convenient_elemental_output=${convenient_elemental_output/dat/root}
		convenient_elemental_output=GiBUU:$convenient_elemental_output
		root -q "make_convenient_from_gibuu.C(\"${raw_gibuu_file}\", \"${gibuu_dat_flux_file}\", \"${NEUTRINO_PDG}\", \"${GIBUU_CC_NC}\", \"${protons}\", \"${nucleons}\", \"${convenient_elemental_output}\", \"1\")"
		unweighted_convenient_outputs+="$convenient_elemental_output"
		unweighted_convenient_outputs+=$'\n'
		# Construct the output name for the weighted Convenient 
		# file, and run Convenient w/ the appropriate weight
		convenient_weighted_output=${convenient_elemental_output/.$element_from_filename./.$element_from_filename.$weight.}

		root -q "make_convenient_from_gibuu.C(\"${raw_gibuu_file}\", \"${gibuu_dat_flux_file}\", \"${NEUTRINO_PDG}\", \"${GIBUU_CC_NC}\", \"${protons}\", \"${nucleons}\", \"${convenient_weighted_output}\", \"${weight}\")"
		weighted_convenient_outputs+="$convenient_weighted_output "
	done <<< $(ls $filename_raw.*)
	# Combine all the weighted files into one file
	root -q "${CONVENIENT_NUISANCE_DIR}/make_convenient_from_convenient.C(\"${weighted_convenient_outputs}\", \"${filename_convenient}\")"
fi

# Move the output files to the folders matching the program that 
# created them.
echo "GiBUU run complete, moving files..."

# Move the raw GiBUU files
while IFS= read -r raw_gibuu_file
do
	# Get the PDG from the filename
	pdg_from_filename=${raw_gibuu_file/$filename_raw./}
	pdg_from_filename=${pdg_from_filename/.dat/}

	# Look up the element
	target=$(awk -v key=${pdg_from_filename} '$1==key { print $2 }' ${CONVENIENT_TAR_DIR}/NOvA_ND/PDG_element_lookup_table.txt)

	# Form the filepath
	filepath=GiBUU/${GiBUU_VERSION}_${params%.job}/"$HC$flux"/"$FLAVOR""$GIBUU_CC_NC"_only/$target
	# Move the file
	outdir=$OUTPUT_DIR/RawGiBUU/$filepath
	mkdir -p $outdir
	mv $raw_gibuu_file $outdir/$raw_gibuu_file
done <<< $(ls $filename_raw.*)

# We want to save the files to either the NOvA specific 
# location, or the general location, depending on which target 
# was used.
if [ $nova_switch -eq 1 ];
then
	# Take care of the Convenient file first
	filepath=GiBUU/${GiBUU_VERSION}_${params%.job}/"$HC$flux"/"$FLAVOR""$GIBUU_CC_NC"_only
	mkdir -p $convenient_output_dir/$filepath
	mv $filename_convenient $convenient_output_dir/$filepath/$filename_convenient
	# Move the unweighted Convenient files
	while IFS= read -r unweighted_convenient_file
	do
		# Get the PDG from the filename
		pdg_from_filename=${unweighted_convenient_file/GiBUU:$filename/}
		pdg_from_filename=${pdg_from_filename/.convenient_output.root./}
		pdg_from_filename=${pdg_from_filename/.root/}

		# Look up the element
		target=$(awk -v key=${pdg_from_filename} '$1==key { print $2 }' ${CONVENIENT_TAR_DIR}/NOvA_ND/PDG_element_lookup_table.txt)
		if [[ $target == "" ]];
		then
			break
		fi

		# Form the filepath
		filepath=GiBUU/${GiBUU_VERSION}_${params%.job}/"$HC$flux"/"$FLAVOR""$GIBUU_CC_NC"_only/$target
		# Move the file
		outdir=$CONVENIENT_OUTPUT_DIR/$filepath
		mkdir -p $outdir
		mv $unweighted_convenient_file $outdir/$unweighted_convenient_file
		# Add to data list and write a .txt file
		bash $CONVENIENT_DIR/documentation_generation_scripts/add_to_data_list.sh -g GiBUU -t ${GiBUU_VERSION}_${gibuu_config} -f $unweighted_convenient_file --flux $gibuu_flux --nova_switch 0
		bash $CONVENIENT_DIR/documentation_generation_scripts/create_output_txt_file.sh -l $outdir,$unweighted_convenient_file -n $N_EVENTS -h $HC -p $NEUTRINO_PDG -f $gibuu_flux_file,$gibuu_flux_histo -t $target -d $DATE -v $GiBUU_VERSION --seed $gibuu_seed	
	done <<< $unweighted_convenient_outputs

	# Reset the filepath
	filepath=GiBUU/${GiBUU_VERSION}_${params%.job}/"$HC$flux"/"$FLAVOR""$GIBUU_CC_NC"_only

	# Remove the weighted Convenient files
	while IFS= read -r weighted_convenient_file
	do
		rm $weighted_convenient_file
	done <<< $weighted_convenient_outputs
else
	# We just need to move the final convenient file. Documentation will be 
	# added at the end
	# Form the filepath
	filepath=GiBUU/${GiBUU_VERSION}_${params%.job}/"$HC$flux"/"$FLAVOR""$GIBUU_CC_NC"_only/$target
	# Move the file
	mkdir -p $convenient_output_dir/$filepath
	mv $filename_convenient $convenient_output_dir/$filepath/$filename_convenien
fi
