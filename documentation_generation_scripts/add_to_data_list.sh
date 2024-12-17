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
	flavor_string="__$FLAVOR""$GiBUU_CC_NC""_only"
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
	output_file=ConvenientOutputs_NOvAList.txt
else
	output_file=ConvenientOutputsList.txt
fi

# Now, for each of the lines created, we have to figure out where, if 
# anywhere, to put it.

# Start with the generator tune string.
# In the output file, search for the generator tune string only and get the 
# line numbers. Cut the output text to show only the line numbers.
generator_tune_string_line="$(grep -w -x -n "$generator_tune_string.*" \
	$output_file | cut -d : -f 1)"
# If the generator tune string doesn't have an entry yet, create one. Place 
# it alphabetically in the list.
# If the generator tune string line is empty:
if [ -z "$generator_tune_string_line" ]; 
then
	# Create a list-string of generator:tune strings, items separated by 
	# spaces
	tunelist="$(grep -v '^$' $output_file | grep -v '^_' )"
	tunelist="$(echo "$tunelist" | tr '\n' ' ')"
	# Change the string to an array
	read -a generator_tune_array <<< $tunelist
	# Get the length of the array
	len_tunelist=${#generator_tune_array[@]}
	# Assume the new generator:tune comes after the first generator:tune in 
	# the list 
	after=true
	i=0
	# While this is true
	while [ $after == true ];
	do
		# Check to see if we're at the end of the list. If we are, set the 
		# line number to be the last line + 1 and exit. Create another line 
		# to accomodate.
		if [ $i -ge $len_tunelist ];
		then
			generator_tune_string_line="$(sed -n "$=" \
				$output_file)"
			generator_tune_string_line=$(($generator_tune_string_line + 1))
			last_tune=""
			echo "" >> $output_file
			after=false
			continue # Break
		fi
		# If we're still in the list, grab the next generator:tune from the 
		# list
		gen_tune=${generator_tune_array[$i]}
		# If the new generator:tune comes alphabetically after the 
		# generator:tune from the list (it has a higher ASCII value):
		if [[ "$gen_tune" < "$generator_tune_string" ]]; 
		then
			i=$((i+1)) # Go to the next entry
		# Otherwise:
		else
			# The current gen:tune is the first entry that the new 
			# generator:tune comes before. The line of gen:tune is where we 
			# want to place the new generator:tune
			generator_tune_string_line="$(grep -w -x -n "$gen_tune" \
				$output_file | cut -d : -f 1)"
			last_tune=$gen_tune
			# Break the loop
			after=false
		fi
	done
	# Insert the new generator:tune at the appropriate line
	sed -i "$generator_tune_string_line i $generator_tune_string\n" \
		$output_file
# If we already have the gen:tune in the list, we still want to know the 
# tune that comes next in the list.
else
	# Create a list-string of generator:tune strings, items separated by 
	# spaces
	tunelist="$(grep -v '^$' $output_file | grep -v '^_' )"
	tunelist="$(echo "$tunelist" | tr '\n' ' ')"
	# Change the string to an array
	read -a generator_tune_array <<< $tunelist
	# Get the length of the array
	len_tunelist=${#generator_tune_array[@]}
	# Loop through the list again
	after=true
	i=0
	while [ $after == true ];
	do
		# Check to see if we're at the end of the list. If we are, set the 
		# last_tune to be empty and exit
		if [ $i -ge $(($len_tunelist - 1)) ];
		then
			last_tune=""
			after=false
			continue
		fi
		# If we're still in the list, grab the next generator:tune from the 
		# list
		gen_tune=${generator_tune_array[$i]}
		# If the new generator:tune comes alphabetically after the 
		# generator:tune from the list (it has a higher ASCII value):
		if [[ "$gen_tune" < "$generator_tune_string" ]]; 
		then
			i=$((i+1)) # Go to the next entry
		# Otherwise:
		else
			# The current gen:tune is the first entry that the new 
			# generator:tune comes before. This is the last_tune we want to 
			# use
			last_tune=${generator_tune_array[$(($i + 1))]}
			# Break the loop
			after=false
		fi
	done
fi

# Now repeat for the flux line.
# Get the range of lines we're working in
first_flux_line=$generator_tune_string_line
if [ -z "$last_tune" ];
then
	last_flux_line="$(sed -n "$=" $output_file)"
else
	last_flux_line="$(grep -w -x -n "$last_tune.*" \
	$output_file | cut -d : -f 1)"
fi

# Everything else is the same, except now we're working within a range of 
# lines

# In the output file, search for the flux string only and get the line 
# numbers. Cut the output text to show only the line numbers. Eliminate all 
# numbers not in the range.
flux_string_lines="$(sed -n "$first_flux_line","$last_flux_line"p $output_file | grep -w -x -n "$flux_string" | cut -d : -f 1)"
# If the flux string doesn't have an entry yet, create one. Place 
# it alphabetically in the list.
# If the flux string line is empty:
if [ -z "$flux_string_lines" ];
then
	# Create a list-string of flux strings, items separated by 
	# spaces
	fluxlist="$(sed -n "$first_flux_line","$last_flux_line"p $output_file | grep "_Flux:")"
	fluxlist="$(echo "$fluxlist" | tr '\n' ' ')"
	# Change the flux list into an array
	read -a fluxarray <<< $fluxlist
	# Get the length of the array
	len_fluxlist=${#fluxarray[@]}
	# Assume the new flux comes after the first flux in 
	# the list 
	after=true
	i=0
	# While this is true
	while [ $after == true ];
	do
		# Check to see if we're at the end of the list. If we are, set the 
		# line number to be the last line and exit. 
		if [ $i -ge $len_fluxlist ];
		then
			if [ $i -eq 0 ];
			then
				flux_string_line=$(($first_flux_line + 1))
			else
				flux_string_line=$last_flux_line
			fi
			last_flux=$last_tune
			after=false
			continue # Break
		fi
		# If we're still in the list, grab the next flux from the 
		# list
		next_flux=${fluxarray[$i]}
		# If the new flux comes alphabetically after the 
		# flux from the list (it has a higher ASCII value):
		if [[ "$next_flux" < "$flux_string" ]]; 
		then
			i=$((i+1)) # Go to the next entry
		# Otherwise:
		else
			# The current flux is the first entry that the new flux
			# comes before. The line of flux is where we 
			# want to place the new flux
			flux_string_line="$(sed -n "$first_flux_line","$last_flux_line"p $output_file | grep -w -x -n "$next_flux" | cut -d : -f 1)"
			flux_string_line=$(($flux_string_line - 1 + $first_flux_line))
			last_flux=$next_flux
			# Break the loop
			after=false
		fi
	done
	# Insert the new flux at the appropriate line
	sed -i "$flux_string_line i $flux_string\n" \
		$output_file
	last_flux_line=$(($last_flux_line + 2))
# If we already have the flux in the list, we still want to know the 
# flux that comes next in the list.
else
	flux_string_line=$(($flux_string_lines - 1 + $first_flux_line))
	# Create a list-string of flux strings, items separated by 
	# spaces
	fluxlist="$(sed -n "$first_flux_line","$last_flux_line"p $output_file | grep "_Flux:")"
	fluxlist="$(echo "$fluxlist" | tr '\n' ' ')"
	# Change the string to an array
	read -a fluxarray <<< $fluxlist
	# Get the length of the array
	len_fluxlist=${#fluxarray[@]}
	# Loop through the list again
	after=true
	i=0
	while [ $after == true ];
	do
		# Check to see if we're at the end of the list. If we are, set the 
		# last_flux to be the last tune and exit
		if [ $i -ge $(($len_fluxlist - 1)) ];
		then
			last_flux=$last_tune
			after=false
			continue
		fi
		# If we're still in the list, grab the next flux from the list
		next_flux=${fluxarray[$i]}
		# If the new flux comes alphabetically after the 
		# flux from the list (it has a higher ASCII value):
		if [[ "$next_flux" < "$flux_string" ]]; 
		then
			i=$((i+1)) # Go to the next entry
		# Otherwise:
		else
			# The current flux is the first entry that the new 
			# flux comes before. This is the last_flux we want to 
			# use
			last_flux=${fluxarray[$(($i + 1))]}
			# Break the loop
			after=false
		fi
	done
fi

# Now repeat for the flavor line.
# Get the range of lines we're working in
first_flavor_line=$flux_string_line
if [ -z "$last_flux" ];
then
	last_flavor_line="$(sed -n "$=" $output_file)"
else
	last_flavor_line="$(sed -n "$first_flavor_line","$last_flux_line"p $output_file | grep -w -x -n "$last_flux" | cut -d : -f 1)"
	last_flavor_line=$(($last_flavor_line - 1 + $first_flavor_line))
fi

# In the output file, search for the flavor string only and get the line 
# numbers. Cut the output text to show only the line numbers. Eliminate all 
# numbers not in the range.
flavor_string_lines="$(sed -n "$first_flavor_line","$last_flavor_line"p $output_file | grep -w -x -n "$flavor_string" | cut -d : -f 1)"
# If the flavor string doesn't have an entry yet, create one. Place 
# it alphabetically in the list.
# If the flavor string line is empty:
if [ -z "$flavor_string_lines" ];
then
	# Create a list-string of flavor strings, items separated by 
	# spaces
	flavorlist="$(sed -n "$first_flavor_line","$last_flavor_line"p $output_file | grep "__nu")"
	flavorlist="$(echo "$flavorlist" | tr '\n' ' ')"
	# Change the flavor list into an array
	read -a flavor_array <<< $flavorlist
	# Get the length of the array
	len_flavorlist=${#flavor_array[@]}
	# Assume the new flavor comes after the first flavor in 
	# the list 
	after=true
	i=0
	# While this is true
	while [ $after == true ];
	do
		# Check to see if we're at the end of the list. If we are, set the 
		# line number to be the last line and exit. 
		if [ $i -ge $len_flavorlist ];
		then
			if [ $i -eq 0 ];
			then
				flavor_string_line=$(($first_flavor_line + 1))
			else
				flavor_string_line=$last_flavor_line
			fi
			last_flavor=$last_flux
			after=false
			continue # Break
		fi
		# If we're still in the list, grab the next flavor from the 
		# list
		flavor=${flavor_array[$i]}
		# If the new flavor comes alphabetically after the 
		# flavor from the list (it has a higher ASCII value):
		if [[ "$flavor" < "$flavor_string" ]]; 
		then
			i=$((i+1)) # Go to the next entry
		# Otherwise:
		else
			# The current flavor is the first entry that the new flavor
			# comes before. The line of flavor is where we 
			# want to place the new flavor
			flavor_string_line="$(sed -n "$first_flavor_line","$last_flavor_line"p $output_file | grep -w -x -n "$flavor" | cut -d : -f 1)"
			flavor_string_line=$(($flavor_string_line - 1 + $first_flavor_line))
			last_flavor=$flavor
			# Break the loop
			after=false
		fi
	done
	# Insert the new flavor at the appropriate line
	sed -i "$flavor_string_line i $flavor_string\n" \
		$output_file
	last_flavor_line=$(($last_flavor_line + 2))
# If we already have the flavor in the list, we still want to know the 
# flavor that comes next in the list.
else
	flavor_string_line=$(($flavor_string_lines - 1 + $first_flavor_line))
	# Create a list-string of flavor strings, items separated by 
	# spaces
	flavorlist="$(sed -n "$first_flavor_line","$last_flavor_line"p $output_file | grep "__nu")"
	flavorlist="$(echo "$flavorlist" | tr '\n' ' ')"
	# Change the string to an array
	read -a flavor_array <<< $flavorlist
	# Get the length of the array
	len_flavorlist=${#flavor_array[@]}
	# Loop through the list again
	after=true
	i=0
	while [ $after == true ];
	do
		# Check to see if we're at the end of the list. If we are, set the 
		# last_flavor to be the last flux and exit
		if [ $i -ge $(($len_flavorlist - 1)) ];
		then
			last_flavor=$last_flux
			after=false
			continue
		fi
		# If we're still in the list, grab the next flavor from the list
		flavor=${flavor_array[$i]}
		# If the new flavor comes alphabetically after the 
		# flavor from the list (it has a higher ASCII value):
		if [[ "$flavor" < "$flavor_string" ]]; 
		then
			i=$((i+1)) # Go to the next entry
		# Otherwise:
		else
			# The current flavor is the first entry that the new 
			# flavor comes before. This is the last_flavor we want to 
			# use
			last_flavor=${flavor_array[$(($i + 1))]}
			# Break the loop
			after=false
		fi
	done
fi

# Now repeat for the number line.
# For this line, if the line exists, we add
# Get the range of lines we're working in
first_num_line=$flavor_string_line
if [ -z "$last_flavor" ];
then
	last_num_line="$(sed -n "$=" $output_file)"
else
	last_num_line="$(sed -n "$first_num_line","$last_flavor_line"p $output_file | grep -w -x -n "$last_flavor" | cut -d : -f 1)"
	last_num_line=$(($last_num_line + $first_num_line - 1))
fi
# In the output file, search for the number string only and get the line 
# numbers. Cut the output text to show only the line numbers. Eliminate all 
# numbers not in the range.
num_string_line=$(($first_num_line + 1))

# If the number string doesn't have an entry yet, create one.
# This is true if there were no flavor string lines
# If the number string line is empty:
if [ -z "$flavor_string_lines" ];
then
	num=$N_EVENTS
	digits=$((${#num}-1))
	while [ ${num:(-1):1} == 0 ]
	do
		num=${num%0}
	done
	num_string="___${num:0:1}.${num:1}e$digits"
	num_string_line=$(($first_num_line + 1))
	# Insert the new flavor at the appropriate line
	sed -i "$num_string_line i $num_string" \
		$output_file
	last_num_line=$(($last_num_line + 2))
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
	last_num_line=$(($last_num_line + 2))
fi
last_num=$last_flavor

# Finally, add the files themselves alphabetically
# Get the range of lines we're working in
first_file_line=$num_string_line
if [ -z "$last_num" ];
then
	last_file_line="$(sed -n "$=" $output_file)"
else
	last_file_line="$(sed -n "$first_file_line","$last_num_line"p $output_file | grep -w -x -n "$last_flavor" | cut -d : -f 1)"
	last_file_line=$(($last_file_line - 1 + $first_file_line))
fi
# In the output file, search for lines with ____ only and get the line 
# numbers. Cut the output text to show only the line numbers. Eliminate all 
# numbers not in the range.
file_string_lines="$(sed -n "$first_file_line","$last_file_line"p $output_file | grep -w -x -n "^____.*")"
# The file string doesn't have an entry yet. Place 
# it alphabetically in the list.
# If there are no files yet:
if [ -z "$file_string_lines" ];
then
	file_string_line=$(($first_file_line + 1))
# If there are files in the list:
else
	# Create a list-string of file strings, items separated by 
	# spaces
	filelist="$(sed -n "$first_file_line","$last_file_line"p $output_file | grep "____.*")"
	filelist="$(echo "$filelist" | tr '\n' ' ')"
	# Change the string to an array
	read -a file_array <<< $filelist
	# Get the length of the array
	len_filelist=${#file_array[@]}
	# Loop through the list again
	after=true
	i=0
	while [ $after == true ];
	do
		file=${file_array[$i]}
		# Check to see if we're at the end of the list. If we are, set the 
		# file line to be past the last file and exit
		if [ $i -ge $(($len_filelist - 1)) ];
		then
			file_string_line=$(($(sed -n "$first_file_line","$last_file_line"p $output_file | grep -w -x -n "$file" | cut -d : -f 1) + $first_file_line - 1))
			file_string_line=$(($file_string_line + 1))
			after=false
			continue
		fi
		# If the new file comes alphabetically after the 
		# file from the list (it has a higher ASCII value):
		if [[ "$file" < "$file_string" ]]; 
		then
			i=$((i+1)) # Go to the next entry
		# Otherwise:
		else
			# The current file is the first entry that the new 
			# file comes before. This is the line we want to 
			# use
			file_string_line=$(($(sed -n "$first_file_line","$last_file_line"p $output_file | grep -w -x -n "$file" | cut -d : -f 1) + $first_file_line - 1))
			# Break the loop
			after=false
		fi
	done
fi
# Insert the new file at the appropriate line
sed -i "$file_string_line i $file_string" $output_file

