# Author: Colin Weber (webe1077@umn.edu)
# Date: 11 January 2024
# Purpose: When new CONVENIENT events are generated, automatically update 
# the list that enumerates all the CONVENIENT outputs. Inputs refer to the 
# values used to generate the CONVENIENT output. Other relevant inputs (e.g. 
# HC) are global variables and thus don't need to be inputted.

# Command: bash add_to_data_list.sh
# 	[-g generator] [-t tune/parameter file]
#	[-f filename] [--flux flux] [--nova_switch 1=used ND element weights]

# She-bang!
#!/bin/bash

# Define a function that places an input string alphabetically between lines 
# l0 and l1 in the output file.
# Format is place_alphabetically str_type string l0 l1 output
# str_type must be one of {gen_tune, flux, flavor, target, file}
place_alphabetically() {
	# Assign variables
	str_type=$1
	string=$2
	l0=$3
	l1=$4
	output=$5

	# Check if l0 = 0. If so, add one (sed counts from 1!)
	if [[ $l0 -eq 0 ]]
	then
		l0=$(($l0 + 1))
	fi

	# In the output file, search for the input string only and get the 
	# line numbers. Cut the output text to show only the line numbers.
	string_lines="$(sed -n "$l0","$l1"p $output | grep -w -x -n "$string.*" | cut -d : -f 1)"
	# Create a list-string of strings of the same type, items separated 
	# by spaces
	if [[ $str_type == "gen_tune" ]]
	then
		list="$(sed -n "$l0","$l1"p $output | grep -v '^_')"
	elif [[ $str_type == "flux" ]]
	then
		list="$(sed -n "$l0","$l1"p $output | grep "_Flux:")"
	elif [[ $str_type == "flavor" ]]
	then
		list="$(sed -n "$l0","$l1"p $output | grep "__nu")"
	elif [[ $str_type == "target" ]] 
	then
		list="$(sed -n "$l0","$l1"p $output | grep "___Target:")"
	elif [[ $str_type == "file" ]]
	then
		list="$(sed -n "$l0","$l1"p $output | grep "____.*")"
	fi
	list="$(echo "$list" | tr '\n' ' ')"

	# Change the string to an array
	read -a array <<< $list
	# Get the length of the array
	len_list=${#array[@]}

	# If the string doesn't have an entry yet, create one. Place 
	# it alphabetically in the list.
	# If the string line is empty:
	if [ -z "$string_lines" ]; 
	then
		# Assume the string comes after the first string in the list. This 
		# method still works if this assumption is false. 
		after=true
		i=0
		# While this is true
		while [ $after == true ];
		do
			# Check to see if we're at the end of the list. If we are, set 
			# the line number to be the last line and exit. 
			if [ $i -ge $len_list ];
			then
				if [ $i -eq 0 ];
				then
					string_line=$(($l0 + 1))
				else
					if [[ $str_type == "file" ]]
					then
						string_line=$((l1 - 1))
					else
						string_line=$l1
					fi
				fi
				last_line=$l1
				after=false
				continue # Break
			fi
			# If we're still in the list, grab the next item from the 
			# list
			next_item=${array[$i]}
			# If the string comes alphabetically after the 
			# item from the list (it has a higher ASCII value):
			if [[ "$next_item" < "$string" ]]; 
			then
				i=$((i+1)) # Go to the next entry
			# Otherwise:
			else
				# The current item is the first entry that the new string
				# comes before. This line is where we want to place the new 
				# string.
				string_line="$(sed -n "$l0","$l1"p $output | grep -w -x -n "$next_item" | cut -d : -f 1)"
				string_line=$(($string_line - 1 + $l0))
				last_line=$(($string_line + 2))
				# Break the loop
				after=false
			fi
		done
		# Insert the new string at the appropriate line
		if [[ $str_type == "file" ]]
		then
			sed -i "$string_line i $string" $output
		else
			sed -i "$string_line i $string\n" $output
		fi
	# If we already have the string in the list, we still want to know the 
	# item that comes next in the list.
	else
		string_line=$(($string_lines - 1 + $l0))
		# Loop through the list again
		after=true
		i=0
		while [ $after == true ];
		do
			# Get the item
			item=${array[$i]}
			# If it doesn't match the string, get the next item
			if [[ "$item" != "$string" ]]
			then
				i=$((i+1))
			# If it does match the string and we're at the end of the list, 
			# return l1
			else
				if [ $i == $(($len_list - 1)) ]
				then
					last_line=$l1
					after=false
				# If we're not at the end of the list, get the line of the 
				# next item
				else
					next_item=${array[$((i+1))]}
					last_line="$(sed -n "$l0","$l1"p $output | grep -w -x -n "$next_item" | cut -d : -f 1)"
					last_line=$(($last_line - 1 + $l0))
					after=false
				fi
			fi
		done
	fi
	# Return two values by echoing them. The first is the line that the new 
	# string was placed at, and the second is the line that delimits the 
	# range of lines in the file that the placed line applies to.
	echo "$string_line $last_line"
}

# Read inputs
generator=$2
tune_params=$4
datalist_filename=$6
flux_folder=$8
ND_switch=${10}

# Create the lines
generator_tune_string=$generator:$tune_params
flux_string="_Flux:$HC$flux_folder"
if [ $generator == "GiBUU" ];
then
	flavor_string="__$FLAVOR""$GIBUU_CC_NC""_only"
else
	flavor_string="__$FLAVOR""_only"
fi
# Getting the # events line will have to wait until we know if we already 
# have events generated with this configuration.
file_string="____$datalist_filename"

# Select the output file to modify based on if the NOvA ND weights have been 
# applied.
if [ $ND_switch -eq 1 ];
then
	output_file=$CONVENIENT_DIR/ConvenientOutputs_NOvAList.txt
else
	output_file=$CONVENIENT_DIR/ConvenientOutputsList.txt
	# In this case, also get the element from the filename
	pdg_from_filename=${datalist_filename##*\.root\.}
	pdg_from_filename=${pdg_from_filename/\.root/}
	target=$(awk -v key=${pdg_from_filename} '$1==key { print $2 }' ${CONVENIENT_TAR_DIR}/NOvA_ND/PDG_element_lookup_table.txt)
	target_string="___Target:$target"
fi

# Get the number of lines in the output file
n_lines=$(wc -l < $output_file)

# Place each line accordingly
# Generator:tune line
gen_tune_output=$(place_alphabetically gen_tune $generator_tune_string 0 $n_lines $output_file)
gen_tune_line=$(echo "$gen_tune_output" | awk '{print $1}')
gen_tune_limit_line="$(echo "$gen_tune_output" | awk '{print $2}')"

# Flux line
flux_output=$(place_alphabetically flux $flux_string $gen_tune_line $gen_tune_limit_line $output_file)
flux_line=$(echo "$flux_output" | awk '{print $1}')
flux_limit_line=$(echo "$flux_output" | awk '{print $2}')

# Flavor line
flavor_output=$(place_alphabetically flavor $flavor_string $flux_line $flux_limit_line $output_file)
flavor_line=$(echo "$flavor_output" | awk '{print $1}')
flavor_limit_line=$(echo "$flavor_output" | awk '{print $2}')

# Target line if necessary
if [ $ND_switch -eq 0 ]
then
	target_output=$(place_alphabetically "target" $target_string $flavor_line $flavor_limit_line $output_file)
	flavor_line=$(echo "$target_output" | awk '{print $1}')
	flavor_limit_line=$(echo "$target_output" | awk '{print $2}')
fi

# Now repeat for the number line.
# For this line, if the line exists, we add
# Get the range of lines we're working in
first_num_line=$flavor_line
last_num_line=$flavor_limit_line
# In the output file, search for the number string only and get the line 
# numbers. Cut the output text to show only the line numbers. Eliminate all 
# numbers not in the range.
num_string_line=$(($first_num_line + 1))

# If the number string doesn't have an entry yet, create one.
# This is true if the number string line doesn't begin with "___"
# Get the first three characters of the line on the number string line
line_on_num_string_line=$(sed -n "$num_string_line"p $output_file)
line_on_num_string_line_first_3=${line_on_num_string_line:0:3}

# If the number string line is empty:
if [[ $line_on_num_string_line_first_3 != "___" ]];
then
	num=$N_EVENTS
	digits=$((${#num}-1))
	while [ ${num:(-1):1} == 0 ]
	do
		num=${num%0}
	done
	num_string="___${num:0:1}.${num:1}e$digits"
	# Insert the new flavor at the appropriate line
	sed -i "$num_string_line i $num_string" \
		$output_file
	last_num_line=$(($last_num_line + 1))
# If we already have numbers in the list, we just add the two together.
else
	# Get the old number
	old_num="$(sed -n "$num_string_line"p $output_file)"
	old_num=${old_num:3}
	old_num_power=${old_num#*e}
	old_num_num=${old_num%e*}
	old_num_full=`echo "$old_num_num*10^$old_num_power" | bc`
	new_num=$N_EVENTS
	num=`echo "$old_num_full+$new_num" | bc`
	while [[ $num == *"."* ]]
	do
    	while [ ${num:(-1):1} == 0 ]
    	do
        	num=${num%0}
		done
		num=${num%.}
	done
	digits=$((${#num}-1))
	while [ ${num:(-1):1} == 0 ]
	do
		num=${num%0}
	done
	num_string="___${num:0:1}.${num:1}e$digits"
	sed -i "$num_string_line s/.*/$num_string/" $output_file
fi

# Finally, add the files themselves alphabetically
file_output=$(place_alphabetically file $file_string $num_string_line $last_num_line $output_file)
file_line=$(echo "$file_output" | awk '{print $1}')
file_limit_line=$(echo "$file_output" | awk '{print $2}')
