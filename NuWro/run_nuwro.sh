# Author: Colin M Weber (webe1077@umn.edu)
# Date: 11 August 2023
# Purpose: To generate events with NuWro

# Command: bash run_nuwro.sh
#	[-o output file name]
#	[-i nuwro parameters file] [-n number of events]
# 	[-p neutrino pdg] [--flux_file flux file]
# 	[--flux_histo flux histo] [-t target composition file in GENIE format] 
#	[--seed MC seed]

# Parameters
# ----------
#	-o
#		The name of the output file
#
#	-i 
#		The name of the input NuWro parameters file to use
#
#	-n
#		The number of events to generate
#
#	-p
#		The PDG code of the neutrino to use in event generation
#
#	--flux_file
#		The name of the file containing a 1D histogram to use as the 
#		neutrino flux
#
#	--flux_histo
#		The name of the histogram within flux_file to use as the neutrino 
#		flux
#
#	-t
#		The target composition, in GENIE format
#
#	--seed
#		The MC seed to input to NuWro

# Outputs
# -------
#	-o
#		The output NuWro file

# She-bang!
#!/bin/bash

## Create the beam energy parameter
	# Construct the file name
	beam_filename_tmp=${10/$CONVENIENT_DIR\//}
	beam_filename=${beam_filename_tmp//[\/]/_}
	original_flux_histo=${12}

	# Construct the full file path
	nuwro_beam_filepath="$CONVENIENT_DIR/NuWro/beam_energies/"$beam_filename"."$original_flux_histo".txt"

	# Check if the beam energy has already been created. If not, create it.
	if [ ! -f "$nuwro_beam_filepath" ];
	then
		root -q "make_beam_energy.C(\"${10}\", \"${12}\", \"${nuwro_beam_filepath}\")"
	fi

	# Read in the beam energy content
	beam_energy=$(<"$nuwro_beam_filepath")

### Create the target content input
	## Read in the GENIE target content
	while read line;
	do
		genie_target_comp=$(echo $line)
	done < "${14}"

	## Construct the string to write to the parameters text input, and 
	## write it
		counter=0 # For determining if we're on the first rep or not

		# Get the line number of the section to modify
		lineNum="$(grep -n "# Set target_content if target_type == 1 #" $4  \
			| head -n 1 | cut -d: -f1)"

		# Delete all previous target content definitions to start with a 
		# clean slate
		sed -i '/^target_content/d' $4

		# Change the Internal Field Separator to a comma, which we use for 
		# parsing the GENIE target content input
		IFS=','

		# Create the array parts to be made of the comma-separated components			# of the GENIE input
		read -a parts <<< "$genie_target_comp"
		for part in "${parts[@]}" # For each part(icle)
		do
			counter=$((counter + 1))
			lineNum=$((lineNum + 1))
			nucleus_construction=''
			
			# Extract the number of protons from the PDG code
			protons=${part:3:3}
			# Strip leading 0's
			protons=$((10#$protons))	
			# Append to the nucleus construction
			nucleus_construction+="$protons " 

			# Extract the number of nucleons from the PDG code
			nucleons=${part:6:3} 
			# Strip leading 0's
			nucleons=$((10#$nucleons))
			# Calculate the number of neutrons
			neutrons=$(($nucleons - $protons))	
			# Append the number of neutrons to the nucleus construction
			nucleus_construction+="$neutrons "

			# Get the fraction by cutting on the brackets
			nuclei_frac=${part#*[}
			nuclei_frac=${nuclei_frac%]*}
			# Get the precision of the GENIE fraction by counting the 
			# characters in the string and subtracting the first two, which 
			# are '0.' of '1'.
			precision=$((${#nuclei_frac} - 2))
			# Turn the fraction into a number by mulptiplying by the 
			# precision
			nuclei=$(echo "scale = 0; $nuclei_frac * 10^$precision" | bc)
			nuclei=${nuclei%.*} # Remove trailing 0's and decimal
			if [ $nuclei -eq 0 ];
			then
				nuclei=1
			fi
			nucleus_construction+="$nuclei x " # Append

			if ((counter == 1)) # If it's the first nucleus, use '='
			then
				# Modify params.txt
				# At the given line number, insert the string 
				# 'target_content ...'
				sed -i "${lineNum} i \
					target_content = ${nucleus_construction}" $4
			else # If it's not the first nucleus, use "+="
				sed -i "${lineNum} i \
					target_content += ${nucleus_construction}" $4
			fi
		done

		# Return IFS to its original value
		IFS=' '

### Generate NuWro events by inputting a general parameters file, then 
### modifying certain parameters as necessary. Target content has already 
### been set in the parameters file, and beam energy is defined above.
nuwro \
	-o $2 \
	-i $4 \
	-p "number_of_events = $6" \
	-p "beam_particle = $8" \
	-p "beam_energy = $beam_energy" \
	-p "random_seed = ${16}" \
	-p "beam_type = 0" \
	-p "target_type = 1"
