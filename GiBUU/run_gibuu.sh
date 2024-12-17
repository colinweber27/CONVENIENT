# Author: Colin M Weber (webe1077@umn.edu)
# Date: 3 June 2024
# Purpose: To generate events with GiBUU

# Command: bash run_gibuu.sh
#	[-i GiBUU job card]
#	[-o output file name] [-n number of events]
# 	[-p neutrino pdg] [-c CC/NC flag] 
#	[--flux_file flux file] [--flux_histo flux_histo] 
#	[-t target composition file in GENIE format]
# 	[--seed MC seed]

# She-bang!
#!/bin/bash

# Each of the parameters except the card file and output file name is 
# set by going into the card file and modifying the appropriate line

# First modify the parameters that will be the same for each run. This 
# includes everything except the target	and the number of events, since for GiBUU, the number of events is approximately target A * numEnsembles * num_runs_SameEnergy.

# Set the neutrino PDG and CC/NC flag
	# Read in from the input
	neutrino_pdg=$8
	cc_nc_flag=${10}

	# Determine the GiBUU inputs flavor_ID and process_ID from the inputs.
	abs_neutrino_pdg=$(( $neutrino_pdg * (($neutrino_pdg > 0) - ($neutrino_pdg < 0)) ))
	if [ $abs_neutrino_pdg -eq 12 ];
	then
		flavor_ID=" 1"
	elif [ $abs_neutrino_pdg -eq 14 ];
	then
		flavor_ID=" 2"
	elif [ $abs_neutino_pdg -eq 16 ];
	then
		flavor_ID=" 3"
	else
		flavor_ID=" 4"
	fi

	if [ $cc_nc_flag == "CC" ];
	then
		process_ID=2
	elif [$cc_nc_flag == "NC" ];
	then
		process_ID=3
	else
		process_ID=1
	fi

	if [ $neutrino_pdg -lt 0 ];
	then
		process_ID="-$process_ID"
	else
		process_ID=" $process_ID"
	fi

	# Modify the line holding the process_ID and flavor_ID by replacing the 
	# current value with the new one
	sed -i 's/^       process_ID .*/       process_ID       = '"${process_ID}"' ! 2:CC, 3:NC, -2:antiCC, -3:antiNC/' $2
	sed -i 's/^       flavor_ID .*/       flavor_ID        = '"${flavor_ID}"' ! 1:electron, 2:muon, 3:tau/' $2

# Ensure that GiBUU looks for a flux histogram. The variables nuXsectionMode 
# and nuExp are set in set_gibuu_variables.sh
	sed -i 's/^       nuXsectionMode .*/       nuXsectionMode   = '"${nuXsectionMode}"' ! 16: EXP_dSigmaMC/' $2
	sed -i 's/^        nuExp .*/        nuExp           = '"${nuExp}"'  ! new, own flux. Must be in buuinput as .dat file/' $2

# Set the flux file. Note that NEUT can't handle neutrinos of less than 100 
# MeV, so we must recreate the flux with the flux zeroed in that range.
	# Read in from the input
	gibuu_flux_file=${12}

	# Modify the line holding the flux file variable by replacing the 
	# current value with the new one
	# In this case, since there are forward slashes in $neut_flux_file, we 
	# use @ to be the delimiter (sed can work with any delimiter). We also 
	# enclose the sed argument in double quotes, allowing us to include 
	# single quotes in the replacement string.
	sed -i "s@^        FileNameFlux = .*@        FileNameFlux = '"${gibuu_flux_file}"'@" $2

# Set the variable numEnsembles. This is related to the amount of memory 
# needed at runtime, and the number of events generated. For debugging 
# purposes (i.e. runs with few [< 1000] events), numEnsembles should be even 
# lower. If < 100, GiBUU requires that the user asserts that this is the 
# correct input by asking the user to put a - sign in front of numEnsembles.
	if [ $numEnsembles -lt 100 ];
	then
		numEnsembles_job="-$numEnsembles"
	else
		numEnsembles_job="$numEnsembles"
	fi

	sed -i "s/^\tnumEnsembles=.*/\tnumEnsembles=${numEnsembles_job}              ! for C12 you can use 4000, for heavier nuclei should be lower; decrease it if there are problems with memory,/" $2

# Set the path to the BUU input and version. These variables are declared 
# when setting up Convenient
	sed -i "s@^\tpath_to_input=.*@\tpath_to_input='"${GiBUU_BUU_INPUT}"'  ! for local run cluster@" $2
	gibuu_year=${GiBUU_VERSION%_*}
	gibuu_year=${gibuu_year/R/}
	sed -i "s/^\tversion=.*/\tversion=${gibuu_year}/" $2

# Set the seed
	# Read in from the inputs
	gibuu_seed=${16}

	# Modify the appropriate line
	sed -i 's/^      Seed = .*/      Seed = '"${gibuu_seed}"'             ! seed for the random number/' $2

# Read in the target. We will run GiBUU once for each element in the target
while read line;
do
	gibuu_target_comp=$(echo $line)
done < "${14}"

# Change the Internal Field Separator to a comma, which we use for 
# parsing the target content input
IFS=','

# Loop over the targets, running GiBUU for each
read -a parts <<< "$gibuu_target_comp"
for part in "${parts[@]}" # For each part(icle)
do
	# Extract the number of protons from the PDG code
	protons=${part:3:3}
	# Strip leading 0's
	protons=$((10#$protons))	
	# Alter the line in the card file
	sed -i "s/^\tZ=.*/\tZ=${protons}\t\t\t\t\!/" $2

	# Extract the number of nucleons from the PDG code
	nucleons=${part:6:3} 
	# Strip leading 0's
	nucleons=$((10#$nucleons))
	# Alter the line in the card file
	sed -i "s/^\tA=.*/\tA=${nucleons}\t\t\t\t\!/" $2

	# The parameter densitySwitch_Static determines the nuclear density 
	# model to use. By default, it is set to 2, corresponding to the NPA 554 
	# model. However, this model has only been worked out for some nuclei, 
	# so GiBUU suggests that the switch be set to 1 (Woods-Saxon potential) 
	# for all other nuclei.
	if [[   ($protons -eq 6     && $nucleons -eq 12) || 
    	    ($protons -eq 1     && $nucleons -eq 1 ) || 
        	($protons -eq 8     && $nucleons -eq 18) || 
        	($protons -eq 8     && $nucleons -eq 16) || 
        	($protons -eq 16    && $nucleons -eq 32) || 
        	($protons -eq 20    && $nucleons -eq 40) ]];
	then
    	densitySwitch_Static=2
	else
   		densitySwitch_Static=1
	fi
	sed -i "s/^\tdensitySwitch_Static=.*/\tdensitySwitch_Static=${densitySwitch_Static}          ! 0: density=0.0, 1: Wood-Saxon by Lenske, 2 : NPA 554, 3: Wood-Saxon by Lenske, different neutron and proton radii,/" $2

	# Set the number of events using the formula n_events ~ target A * 
	# numEnsembles * num_runs_SameEnergy. The last constant is the one to 
	# modify to get the desired n_events, since we have already set target A 
	# and numEnsembles.
	gibuu_n_events=$6
	num_runs_SameEnergy=`echo "$gibuu_n_events / $nucleons / $numEnsembles" | bc`
	sed -i "s/^\tnum_runs_SameEnergy=.*/\tnum_runs_SameEnergy=${num_runs_SameEnergy}       ! (100) increase these if you want to increase statistics (= number of generated events)/" $2

	### Generate GiBUU events by inputting the job card that has just been 
	### modified and specifying the name of the output file.
	gibuu_elemental_outputs=$4.${part%[*}.dat
	if [ ! -f $gibuu_elemental_outputs ];
	then
		$GiBUU < $2 
		mv FinalEvents.dat $gibuu_elemental_outputs
	else
		continue
	fi

	# GiBUU gives us a crazy number of output files, so we need to delete as 
	# we go.
	rm DensTab_target.dat
	rm diff*
	rm Event*
	rm GiBUU_*
	rm main.run
	rm mass*
	rm Multiplicity*
	rm neutrino*
	rm NucleonVacuumMass.dat
	rm OutChannels*
	rm PYR.RG
	rm ReAdjust*
	rm reconstruction*
	rm sigma*
done

# Return IFS to its original value
IFS=' '

