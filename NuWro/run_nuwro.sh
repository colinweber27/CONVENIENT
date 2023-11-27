# Author: Colin M Weber (webe1077@umn.edu)
# Date: 11 August 2023
# Purpose: To generate events with NuWro

# Command: bash run_nuwro.sh
# 	[-p neutrino pdg] [--beam_type beam type] [--flux_file flux file]
# 	[--flux histo flux histo] [--target_type target type]
# 	[-t target composition file in GENIE forma]t [--seed MC seed]

# She-bang!
#!/bin/bash

## Create the beam energy parameter
	# Construct the file name
	filename=${8//[\/]/_}

	# Construct the full file path
	filepath="/nova/app/users/colweber/Convenient/NuWro/beam_energies/$filename.txt"

	# Check if the beam energy has already been created. If not, create it.
	if [ ! -f "$filepath" ];
	then
		root -q "NuWro/make_beam_energy.C(\"$8\", \"${10}\")"
	fi

	# Read in the beam energy content
	BEAM_ENERGY=$(<"$filepath")

### Create the target content input
	## Read in the GENIE target content
	while read line;
	do
		GENIE_TARGET_COMP=$(echo $line)
	done < "${12}"

	## Construct the string to write to the parameters text input, and 
	## write it
		counter=0 # For determining if we're on the first rep or not

		# Get the line number of the section to modify
		lineNum="$(grep -n "# Set target_content if target_type == 1 #" $2  \
			| head -n 1 | cut -d: -f1)"

		# Delete all previous target content definitions to start with a 
		# clean slate
		sed -i '/^target_content/d' $2

		# Change the Internal Field Separator to a comma, which we use for 
		# parsing the GENIE target content input
		IFS=','

		# Create the array parts to be made of the comma-separated components			# of the GENIE input
		read -a parts <<< "$GENIE_TARGET_COMP"
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
			# are '0.'
			precision=$((${#nuclei_frac} - 2))
			# Turn the fraction into a number by mulptiplying by the 
			# precision
			nuclei=$(echo "scale = 0; $nuclei_frac * 10^$precision" | bc)
			nuclei=${nuclei%.*} # Remove trailing 0's and decimal
			nucleus_construction+="$nuclei x " # Append

			if ((counter == 1)) # If it's the first nucleus, use '='
			then
				# Modify params.txt
				# At the given line number, insert the string 
				# 'target_content ...'
				sed -i "${lineNum} i \
					target_content = ${nucleus_construction}" $2
			else # If it's not the first nucleus, use "+="
				sed -i "${lineNum} i \
					target_content += ${nucleus_construction}" $2
			fi
		done

		# Return IFS to its original value
		IFS=' '

### Generate NuWro events by inputting a general parameters file, then 
### modifying certain parameters as necessary. Target content has already 
### been set in the parameters file, and beam energy is defined above.
nuwro \
	-o $NUWRO_OUTPUT \
	-i $2 \
	-p "number_of_events = $4" \
	-p "beam_particle = $6" \
	-p "beam_energy = $BEAM_ENERGY" \
	-p "random_seed = ${14}" \
	-p "beam_type = 0" \
	-p "target_type = 1"
