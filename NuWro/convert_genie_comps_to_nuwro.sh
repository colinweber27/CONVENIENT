# Author: Colin M Weber (webe1077@umn.edu)
# Date: 9 August 2023
# Purpose: To convert a line of text listing some target composition in 
# GENIE format to the equivalent in NuWro format.
# GENIE format is PDG[fraction],..., where PDG is the PDG code of a given 
# nucleus in the target, and fraction is the fraction of the total that is 
# made up of that nuclus.
# NuWro format is [# protons] [# neutrons] [# nuclei] x ..., where # protons 
# is the number of protons in a given nucleus in the target, # neutrons is 
# the number of neutrons in that same nucleus, and # nuclei is the number of # that nucleus in the target.

# Command: bash convert_genie_comps_to_nuwro.sh -i input file -p 4 -o output file name
# Note that all options must be set. The option -p is for the GENIE 
# precision, which we define to be the number of decimal places used in the 
# fractions for the GENIE target composition.
# This function is implemented in run_nuwro.sh, but in that case is modified 
# to pipe the output to the appropriate params.txt file.

# She-bang!
#!/bin/bash

# Read in the GENIE formatted file
while read line;
do
	GENIE_TARGET_COMP=$(echo $line)
done < "$2"

# Set the GENIE precision
GENIE_PRECISION=$4

# Create the output file
touch $6

# Initialize the string to be written to the output file
NUWRO_STRING=""

# Parse the GENIE string and use its pieces to construct the NuWro string
# Elements in the GENIE string are separated by commas, so set the Internal Field Separator (IFS) to be a comma.
IFS=','
# Use the here-string to input the pre-defined string $GENIE_TARGET_COMP to the read command, which separates its items into an array (-a) based on commas
read -a parts <<< "$GENIE_TARGET_COMP"
for part in "${parts[@]}"; # For each item in the array "parts":
do
	protons=${part:3:3} # Extract the number of protons from the PDG code. This may contain leading 0's.
	protons=$((10#$protons)) # Strip leading 0's

	nucleons=${part:6:3} # Extract the number of nucleons from the PDG code. This may contain leading 0's.
	nucleons=$((10#$nucleons)) # Strip leading 0's by converting nucleons to 
							   # base 10
	neutrons=$(($nucleons-$protons)) # Calculate the number of neutrons
	
	# Get the fraction by cutting on the brackets
	nuclei_frac=${part#*[} # Take part, and cut everything before the [
	nuclei_frac=${nuclei_frac%]*} # Take the previous, and cut everything 
								  # after the ]
	# Turn the fraction into a number by multiplying by the GENIE precision
	nuclei=$(echo "scale = 0; $nuclei_frac * 10^$GENIE_PRECISION" | bc)
	nuclei=${nuclei%.*} # Remove trailing 0's and decimal

	# Construct the NuWro string using the info above 
	NUWRO_STRING+="$protons "
	NUWRO_STRING+="$neutrons "
	NUWRO_STRING+="$nuclei x "
done

IFS=' ' # Reset the IFS to default
echo $NUWRO_STRING >> $6 # Write the NuWro string to the output file
