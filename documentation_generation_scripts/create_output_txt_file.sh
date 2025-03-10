# Author: Colin Weber (webe1077@umn.edu)
# Date: 10 January 2024
# Purpose: To generate a text file describing the inputs that created 
# a particular CONVENIENT output. All inputs refer to the values that 
# were used to create the output.

# Command: bash create_output_txt_file.sh
#	[-l filepath,convenient filename]
# 	[-n number of events] [-h horn current polarity (caps)]
#	[-p neutrino pdg] [-f flux file,flux histogram]
# 	[-t path to file containing target composition in GENIE form]
# 	[-d date in yyyymmddhhmmss format] [-v genereator version]
# 	[--seed MC seed]

# Outputs
#	file
#		a .txt file decribing how the associated CONVENIENT file was made. 
#		It's located in the same directory as the CONVENIENT file and has 
#		the same file name, except the extension is changed from .root to 
#		.txt

# She-bang!
#!/bin/bash

# Create the text file. The directory will already have been made
filepath_filename=$2 # Read the input
abs_filepath=${filepath_filename%,*} 
	# Everything before the comma is the absolute filepath
txt_filename=${filepath_filename#*,} # Everything after the comma is filename
file=$abs_filepath/"${txt_filename:0:-5}".txt
touch $file

# Add the number of events
in_n=$4 # Extract the number from the inputs
digits=$((${#in_n}-1)) # Count the length of string in_n, then subtract 1
while [ ${in_n:(-1):1} == 0 ] # While in_n still has a 0 at the end:
do
	in_n=${in_n%0} # Delete the last 0
done
num=${in_n:0:1}.${in_n:1} # Put a decimal point after the first digit
echo "N_EVENTS=$4 (${num}e${digits})" >> $file # Write in human-readable 
											   # format
echo "" >> $file # Add a blank line

# Add the horn current polarity
echo "HC="'"'"$6"'"'" # Horn current polarity" >> $file # Consistent format
echo "" >> $file # Blank line

# Add the neutrino PDG
echo "NEUTRINO_PDG=$8" >> $file
echo "" >> $file

# Add the flux file and flux histogram
flux_string=${10} # Read the input
flux_file=${flux_string%,*} # Everything before the comma is the flux file
flux_histo=${flux_string#*,} # Everything after the comma is the flux histo
echo "FLUX_FILE=$flux_file" >> $file 
echo "" >> $file
echo "FLUX_HISTO=$flux_histo" >> $file
flux_histo_info_string=$'	For information on how the flux files and histograms were \n	created, see the README.txt in Convenient/flux, or \n	Convenient/README.txt'
echo "$flux_histo_info_string" >> $file
echo "" >> $file

# Add the target composition info
target_comp=${12}
target_comp_name=${target_comp/$CONVENIENG_TAR_DIR\//}
if [[ ${target_comp_name:0:4} == "NOvA" ]];
then
	echo "GENIE_ND_SOUP=$target_comp" >> $file
else
	echo "TARGET=$target_comp" >> $file
fi
target_comp_string=$'	The text file containing the target composition of the NOvA ND\n	For information on how this file was created, see the \n	README.txt in Convenient/compositions, or \n	Convenient/README.txt'
echo "$target_comp_string" >> $file
echo "" >> $file

# Add the seed
seed=${18}
echo "SEED=$seed # The MC seed" >> $file
echo "" >> $file

# Add the date
date=${14} # Read the input
year=${date:0:4} # Extract the year
day=${date:6:2} # Extract the day
month_num=${date:4:2} # Extract the month
declare -A months # Declare a dictionary to hold the month_num / month_word 
				  # pairs
months=( ["01"]="January" ["02"]="February" ["03"]="March" ["04"]="April" ["05"]="May" ["06"]="June" ["07"]="July" ["08"]="August" ["09"]="September" ["10"]="October" ["11"]="November" ["12"]="December") # Populate the dictionary
month_word="${months[$month_num]}" # Get the month word value using the 
								   # month num key
echo "DATE=$year $month_word $day" >> $file
echo "" >> $file

# Add the generator version
echo "GENERATOR_VERSION=${16}" >> $file
