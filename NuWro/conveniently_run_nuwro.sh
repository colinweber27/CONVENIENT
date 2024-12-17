# Author: Colin Weber (webe1077@umn.edu)
# Date: 16 December 2024
# Purpose: this is a wrapper to handle running NuWro within Convenient

# Command: conveniently_run_nuwro.sh
#	[--config NuWro parameters file] [--flux_file flux file]
#	[--flux_histo flux histo] [--seed seed] [--target target]

# She-bang!
#!/bin/bash

# Read the inputs
nuwro_config=$2
nuwro_flux_file=$4
nuwro_flux_histo=$6
nuwro_seed=$8
nuwro_target=${10}
nuwro_rel_target=${nuwro_target/$CONVENIENT_TAR_DIR\//}

# In $config, separate out params file from directory
params=${nuwro_config#*/} # Params is everything after the /

# Check the target. This will affect the filenames
if [ ${nuwro_rel_target:0:4} == "NOvA" ];
then
	nova_switch=1
	filename="$params.$nuwro_seed.$HC.${N_EVENTS:0:1}m.$FLAVOR"
	nuisance_output_dir=$CONVENIENT_NUISANCE_NOvA_OUTPUT_DIR
	convenient_output_dir=$CONVENIENT_NOvA_OUTPUT_DIR
else
	nova_switch=0
	target_txt=${nuwro_rel_target#*elements/}
	target=${target_txt%.txt}
	filename="$params.$nuwro_seed.$HC.${N_EVENTS:0:1}m.$FLAVOR.$target"
	nuisance_output_dir=$CONVENIENT_NUISANCE_OUTPUT_DIR
	convenient_output_dir=$CONVENIENT_OUTPUT_DIR
fi


# Set the output file names
filename_raw=$filename.raw.root
filename_nuisance=NuWro:$filename.NUISANCE.root
filename_convenient=NuWro:$filename.convenient_output.root

# Set the run variables. This includes the NuWro output filename
source set_nuwro_variables.sh

# Tell us what tune we're on
echo "Generating and processing NuWro w/ params $params..."

# Run NuWro
# Format is
	# -o output file name
	# -i input parameters file -n number of events 
	# -p neutrino pdg --flux_file flux file 
	# --flux_histo flux histo
	# -t target composition file in GENIE format --seed MC seed
bash run_nuwro.sh \
	-o $filename_raw \
	-i $nuwro_config \
	-n $N_EVENTS \
	-p $NEUTRINO_PDG \
	--flux_file $nuwro_flux_file \
	--flux_histo $nuwro_flux_histo \
	-t $nuwro_target \
	--seed $nuwro_seed

# Run NUISANCE on the output from above.
# Format is (for a beam with only one neutrino flavor)
	# -i input file name
	# -F flux file,flux spectrum,beam component,1 
	# -o output filename
source $CONVENIENT_NUISANCE_DIR/set_nuisance_variables.sh

bash $CONVENIENT_NUISANCE_DIR/run_nuwro_nuisance.sh \
	-i $filename_raw \
	-F $nuwro_flux_file,$nuwro_flux_histo,$NEUTRINO_PDG,1 \
	-o $filename_nuisance

# Post-process the NUISANCE output to output the Convenient file 
root -q "${CONVENIENT_NUISANCE_DIR}/make_convenient_from_nuisance.C(\"${filename_nuisance}\", \"${filename_convenient}\", 1)"

# Move the output files to the folders matching the program that 
# created them.
echo "NuWro run complete, moving files..."

# Delete the extra NuWro files to save space.
rm $filename_raw
rm $filename_raw.par
rm $filename_raw.txt
rm q0.txt
rm q2.txt
rm qv.txt
rm T.txt
rm random_seed
rm totals.txt

# We want to save the files to either the NOvA specific 
# location, or the general location, depending on which target 
# was used.
if [ $nova_switch -eq 1 ];
then
	filepath=NuWro/${params%.txt}/"$HC$flux"/"$FLAVOR"_only
else
	filepath=NuWro/${params%.txt}/"$HC$flux"/"$FLAVOR"_only/$target
fi

# mkdir format is straightforward. 
# -p means make parent directories as well, if missing
mkdir -p $nuisance_output_dir/$filepath
mkdir -p $convenient_output_dir/$filepath
# mv format is [file to move] [new directory and/or name]
mv $filename_nuisance $nuisance_output_dir/$filepath/$filename_nuisance
mv $filename_convenient $convenient_output_dir/$filepath/$filename_convenient
