# Author: Colin M Weber (webe1077@umn.edu)
# Date: 8 April 2024
# Purpose: To generate events with NEUT

# Command: bash run_neut.sh
#	[-i NEUT card file]
#	[-o output file name] [-n number of events]
# 	[-p neutrino pdg] [--flux_file flux file]
# 	[--flux_histo flux_histo] [-t target composition file in GENIE format]

# Parameters
#	-i
#		The .card file that sets the NEUT configuration. One example is on 
#		GitHub at neut-devel/neut/src/generators/nuclsmpl/neut.card
#	-o
# 		The output NEUT file name. If the target is a mixutre of elements, 
#		then one output is created per element in the mixture, and the 
#		output filenames have the particle information appended.
# 	-n
#		The approximate number of events to generate
#	-p
#		The neutrino flavor to use in event generation, as a PDG code
#	--flux_file
#		The ROOT file containing the 1D flux histogram to use as input
#	--flux_histo
#		The 1D histogram within the flux_file containing the flux
#	-t
#		A .txt file containing the target information in GENIE format.
#		Examples are in the directory CONVENIENT/targets.

# Outputs
#	$neut_elemental_outputs
#		The NEUT output file. One of these is created for each element in 
#		the target.

# She-bang!
#!/bin/bash

# Each of the parameters except the card file and output file name is 
# set by going into the card file and modifying the appropriate line

# First modify the parameters that will be the same for each run. This 
# includes everything except the target	

# Set the number of events
	# Read in from the input
	n_events=$6

	# Modify the line holding the variable by replacing the current 
	# value with the new one
	sed -i 's/^EVCT-NEVT  .*/EVCT-NEVT  '"${n_events}"'/' $2

# Set the neutrino PDG
	# Read in from the input
	neutrino_pdg=$8

	# Modify the line holding the neutrino pdg variable by replacing the 
	# current value with the new one
	sed -i 's/^EVCT-IDPT .*/EVCT-IDPT '"${neutrino_pdg}"'/' $2

# Ensure that NEUT looks for a flux histogram. The variable NEUT_MPV is set 
# in set_neut_variables.sh
	sed -i 's/^EVCT-MPV .*/EVCT-MPV '"${NEUT_MPV}"'/' $2

# Set the flux file. Note that NEUT can't handle neutrinos of less than 100 
# MeV, so we must recreate the flux with the flux zeroed in that range.
	# Read in from the input
	neut_original_flux_file=${10/$CONVENIENT_DIR\//}
	neut_original_flux_filename=${neut_original_flux_file//[\/]/_}
	neut_original_flux_histo=${12}

	# Construct the full filepath to the NEUT flux file
	neut_root_beam_filepath="$CONVENIENT_DIR/NEUT/beam_energies/"$neut_original_flux_filename"."$neut_original_flux_histo".root"
	neut_txt_beam_filepath="$CONVENIENT_DIR/NEUT/beam_energies/"$neut_original_flux_filename"."$neut_original_flux_histo".txt"
	
	# Check to see if the NEUT flux file has been created yet. If it has not 
	# been created:
	if [ ! -f "$neut_root_beam_filepath" ];
	then
		# Translate the original flux file to a .txt file, zeroing the 
		# relevant bins in the process
		root -q "make_beam_energy_txt.C(\"${10}\", \"$neut_original_flux_histo\", \"$neut_txt_beam_filepath\")"

		# Translate the .txt file to a .root file
		root -q "make_flux_hist_root.C(\"$neut_txt_beam_filepath\", \"$neut_root_beam_filepath\", \"$neut_original_flux_histo\")"
	fi

	# Assign the variable
	neut_flux_file=${neut_root_beam_filepath/"$PWD/"/}

	# Modify the line holding the flux file variable by replacing the 
	# current value with the new one
	# In this case, since there are forward slashes in $NEUT_flux_file, we 
	# use @ to be the delimiter (sed can work with any delimiter). We also 
	# enclose the sed argument in double quotes, allowing us to include 
	# single quotes in the replacement string.
	sed -i "s@^EVCT-FILENM  .*@EVCT-FILENM  '"${neut_flux_file}"'@" $2

# Set the flux histogram
	# The histogram has already been read in as part of setting the flux
	
	# Modify the line holding the flus histogram variable by replacing the 
	# current value with the new one
	sed -i "s/^EVCT-HISTNM  .*/EVCT-HISTNM  '"${neut_original_flux_histo}"'/" $2

# Ensure the histograms are read in the correct units. The variable 
# NEUT_INMEV is set in set_neut_variables.sh
	sed -i 's/^EVCT-INMEV .*/EVCT-INMEV '"${NEUT_INMEV}"'/' $2

# Ensure that we are using the user-inputted seed. The variable NEUT_RAND is 
# set in set_nuisance_variables.sh
	sed -i 's/^NEUT-RAND .*/NEUT-RAND '"${NEUT_RAND}"'/' $2

# Read in the target. We will run NEUT once for each element in the target
while read line;
do
	neut_target_comp=$(echo $line)
done < "${14}"

# Change the Internal Field Separator to a comma, which we use for 
# parsing the target content input
IFS=','

# Loop over the targets, running NEUT for each
read -a parts <<< "$neut_target_comp"
for part in "${parts[@]}" # For each part(icle)
do
	# Extract the number of protons from the PDG code
	protons=${part:3:3}
	# Strip leading 0's
	protons=$((10#$protons))	
	# Alter the line in the card file
	sed -i 's/^NEUT-NUMBNDP .*/NEUT-NUMBNDP '"${protons}"'/' $2

	# Extract the number of nucleons from the PDG code
	nucleons=${part:6:3} 
	# Strip leading 0's
	nucleons=$((10#$nucleons))
	# Alter the line in the card file
	sed -i 's/^NEUT-NUMATOM .*/NEUT-NUMATOM '"${nucleons}"'/' $2

	# Calculate the number of neutrons
	neutrons=$(($nucleons - $protons))	
	# Alter the line in the card file
	sed -i 's/^NEUT-NUMBNDN .*/NEUT-NUMBNDN '"${neutrons}"'/' $2

	# Calculate the number of free protons. This is given by the last digit 
	# in the 10 digit NEUT-modified PDG string.
	free_protons=${part:9:1}
	# Alter the line in the card file
	sed -i 's/^NEUT-NUMFREP .*/NEUT-NUMFREP '"${free_protons}"'/' $2

	### Generate NEUT events by inputting the card file that has just been 
	### modified and specifying the name of the output file.
	### Only generate events if they haven't yet been generated
	neut_elemental_outputs=$4.${part%[*}.root
	if [ ! -f $neut_elemental_outputs ];
	then
		neutroot2 $2 $neut_elemental_outputs
	else
		continue
	fi
done

# Return IFS to its original value
IFS=' '

