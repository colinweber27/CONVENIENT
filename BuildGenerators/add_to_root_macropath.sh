# Author: Colin Weber (webe1077@umn.edu)
# Date: 3 March 2025
# Purpose: To automatically add necessary CONVENIENT directories to user's 
# ROOT MacroPath

# She-bang!
#!/bin/bash

# Construct array of directories to add
directories_list="$CONVENIENT_DIR $CONVENIENT_DIR/GiBUU $CONVENIENT_DIR/NEUT $CONVENIENT_DIR/NUISANCE $CONVENIENT_DIR/NuWro $CONVENIENT_DIR/specific_xsec_analysis_scripts"
read -a directories_array <<< $directories_list

# First check that we have the file. If not, make it and add to it
if [ ! -f ~/.rootrc ];
then
	touch ~/.rootrc
	macro_path_line="Unix.*.Root.MacroPath: "
	for ((a=0; a<${#directories_array[@]}; a++))
	do
		if [[ $a == "0" ]]
		then
			macro_path_line+="${directories_array[$a]}"
		else
			macro_path_line+=":${directories_array[$a]}"
		fi
	done
	echo "$macro_path_line" > ~/.rootrc
else
	# Get list of directories we do have
	n_lines=$(wc -l < ~/.rootrc)
	macro_path_line_no="$(sed -n "1,$n_lines"p ~/.rootrc | grep -w -x -n "^Unix.*.Root.MacroPath:.*" | cut -d : -f 1)"
	macro_path_line="$(sed -n "$macro_path_line_no"p ~/.rootrc)"
	macro_path_line_list=${macro_path_line/Unix.*.Root.MacroPath: /}
	IFS=:
	read -ra existing_array <<< "$macro_path_line_list"
	IFS=' '

	# For each item in the directories to add list, loop over the existing 
	# ones to see if we have it. If not, append it to the end.
	for ((a=0; a<${#directories_array[@]}; a++))
	do
		exists=false
		for ((b=0; b<${#existing_array[@]}; b++))
		do
			if [[ ${directories_array[$a]} == ${existing_array[$b]} ]]
			then
				exists=true
				break
			fi
		done
		if [[ $exists == "false" ]]
		then
			macro_path_line+=":${directories_array[$a]}"
			echo "${directories_array[$a]} added to MacroPath."
		fi
	done

	echo "Writing to ~/.rootrc..."
	sed -i ''"${macro_path_line_no}"' c'"${macro_path_line}"'' ~/.rootrc
fi

