# Author: Colin M Weber (webe1077@umn.edu)
# Date: 7 August 2023
# Purpose: This shell runs/is Convenient. 
# It takes as input the number of events, neutrino energy spectra and 
# flavors, target composition, seed, and a list of event generators to run, 
# then runs the generators and processes the outputs, putting them in a 
# useful format.

# Command: source run.sh

# She-bang!
#!/bin/bash

# Tell us we've begun
echo "Convenient run begun."

# Build NUISANCE
echo "Building NUISANCE..."
source $NUISANCE_SETUP_SHELL
echo "Nuisance built."

## Figure out which generators to run, and with which configurations/tunes
echo "Parsing generator list..."

# Separate list by commas
IFS=','

# Read the list of runs into an array, where each item is of the form 
# GENERATOR:tune/params list
generator_list=''
config_list=''
read -a runs <<< "$RUNS"
for run in "${runs[@]}" # For each run
do
	generator=${run%:*} # To get the generator, remove everything after the :
	generator_list+="$generator " # Append to the generator list
	config=${run#*:} # To get the config, remove everything before the :
	config_list+="$config " # Append to the appropriate list
done
IFS=' '
echo "Generator list parsed."

## Run through the list
# Create an array of generators
read -a generator_array <<< $generator_list

# Create a corresponding array of configurations (tunes and parameter files)
read -a config_array <<< $config_list

# Loop through the generators and configurations
for ((i=0; i<${#generator_array[@]}; i++))
do
	# Grab the generator and configuration
	generator=${generator_array[$i]}
	config=${config_array[$i]}

	# Depending on the generator, execute the appropriate commands
	case $generator in
		GENIE)
			# Tell us what tune we're on
			echo "Generating and processing GENIE $config..."

			# Setup the version of GENIE appropriate to the given tune.
			case "$config" in
	
				# Default GENIE tune for NOvA as of 21 August 2023
				G18_10a_02_11b)
					setup genie v3_04_00 -q e20:inclxx:prof
					setup genie_xsec v3_04_00 -q G1810a0211b:k250:e1000
					setup genie_phyopt v3_04_00 -q dkcharmtau
					;;

				# Intended GENIE tune used for NOvA Prod. 5 and 5.1
				# This tune had an initial "GENIE tune skew" added after the 
				# fact.
				N18_10j_02_11a)
					setup genie v3_02_02a -q e20:inclxx:prof
					setup genie_xsec v3_02_02 -q N1810j0211a:k250:e1000
					setup genie_phyopt v3_02_00 -q dkcharmtau
					;;

				# In practice tune used for NOvA Prod 5 and 5.1
				# The implementation of this tune in Convenient is still in 
				# progress.
				# G18_10j_00_000)
				# 	setup genie v3_00_04a -q e17:prof
				#	setup genie_xsec v3_00_04a -q G1810j00000:e1000:k250
				#	setup genie_phyopt v3_00_04 -q dkcharmtau
				#	;;

				# Current DUNE tune as of 21 August 2023
				AR23_20i_00_000)
					setup genie v3_04_00 -q e20:inclxx:prof
					setup genie_xsec v3_04_00 -q AR2320i00000:k250:e1000
					setup genie_phyopt v3_04_00 -q dkcharmtau
					;;

				# Everything else, throw an error
				*)
					echo "Error: Unknown how to set up GENIE for this tune."
					return
					;;
			esac

			# Set the GENIE run variables. These include the range of 
			# neutrino energies, the verbosity, and possibly the cross 
			# section splines file.
			source GENIE/set_gevgen_variables.sh

 			# Run GENIE
			# Format is 
				# -n number of events -f flux file,flux spectrum 
				# -p beam neutrino pdg 
				# -t target composition file in GENIE format
				# --seed MC seed to use --tune GENIE tune to use
 			bash GENIE/run_gevgen.sh \
				-n $N_EVENTS \
 				-f $FLUX_FILE,$FLUX_HISTO \
				-p $NEUTRINO_PDG \
 				-t $GENIE_ND_SOUP \
				--seed $SEED \
 				--tune $config

			# Run NUISANCE on the output from above.
			# Format is 	
				# -f flux file,flux spectrum 
				# -t target composition file in GENIE format 
			source NUISANCE/set_nuisance_variables.sh

			source NUISANCE/run_genie_nuisance.sh \
				-f $FLUX_FILE,$FLUX_HISTO \
				-t $GENIE_ND_SOUP

			# Post-process the NUISANCE output
			root -q NUISANCE/make_usable_root_via_nuisance.C

			# Move the output files to the folders matching the program that 
			# created them.
			echo "GENIE run complete, moving files..."
			# mv format is [file to move] [new directory and/or name]
			# Also, delete the GENIE files to save space.
			rm $GENIE_OUTPUT
			rm $config.$DATE.$SEED.gntp.ghep.status
			mv $NUISFLAT_INPUT.$NUISFLAT_FORMAT.root \
				NUISANCE/GENIE/$HC/"$FLAVOR"_only/$NUISFLAT_INPUT.$HC.$NUISFLAT_FORMAT.root
			mv convenient_output.root \
				generator_outputs/GENIE/$HC/"$FLAVOR"_only/$NUISFLAT_INPUT.$HC.convenient_output.root
			;;

		NuWro)
			# In $config, separate out params file from directory
			params=${config#*/} # Params is everything after the /

			# Set the  run variables. These include the beam type and target 
			# type, as well as the default NuWro output filename			
			source NuWro/set_nuwro_variables.sh
	
			# Tell us what tune we're on
			echo "Generating and processing NuWro w/ params $params..."

			# Run NuWro
			# Format is 
				# -i input parameters file -n number of events 
				# -p neutrino pdg --flux_file flux file 
				# --flux_histo flux histo
				# -t target composition file in GENIE format --seed MC seed
			
			bash NuWro/run_nuwro.sh \
				-i $config \
				-n $N_EVENTS \
				-p $NEUTRINO_PDG \
				--flux_file $FLUX_FILE \
				--flux_histo $FLUX_HISTO \
 				-t $GENIE_ND_SOUP \
				--seed $SEED

			# Run NUISANCE on the output from above.
			# Format is (for a beam with only one neutrino flavor)
				# -f flux file,flux spectrum,beam component,1 
			source NUISANCE/set_nuisance_variables.sh

			source NUISANCE/run_nuwro_nuisance.sh \
				-F $FLUX_FILE,$FLUX_HISTO,$NEUTRINO_PDG,1

			# Post-process the NUISANCE output
			root -q NUISANCE/make_usable_root_via_nuisance.C

			# Move the output files to the folders matching the program that 
			# created them.
			echo "NuWro run complete, moving files..."
			# mv format is [file to move] [new directory and/or name]
			# Also, delete the GENIE files to save space.
			rm $NUWRO_OUTPUT
			rm $NUWRO_OUTPUT.par
			rm $NUWRO_OUTPUT.txt
			rm q0.txt
			rm q2.txt
			rm qv.txt
			rm T.txt
			rm random_seed
			rm totals.txt
			mv $NUISFLAT_INPUT.$NUISFLAT_FORMAT.root \
				NUISANCE/NuWro/$HC/"$FLAVOR"_only/$NUISFLAT_INPUT.$HC.$NUISFLAT_FORMAT.root
			mv convenient_output.root \
				generator_outputs/NuWro/$HC/"$FLAVOR"_only/$NUISFLAT_INPUT.$HC.convenient_output.root
			;;

		*)
			echo "Error: Unknown generator input."
			exit 1
			;;
	esac
done

# Let us know we're done
echo "Convenient run complete."
