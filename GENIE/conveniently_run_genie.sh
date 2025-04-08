# Author: Colin Weber (webe1077@umn.edu)
# Date: 13 December 2024
# Purpose: this is a wrapper to handle running GENIE within Convenient

# Command: conveniently_run_genie.sh
#	[--config GENIE_tune] [--flux_file flux file]
#	[--flux_histo flux histo] [--seed seed] [--target target]

# Parameters
#	config
#		The GENIE tune to run with. Available tunes can be seen at 
#		https://hep.ph.liv.ac.uk/~costasa/genie/tunes.html, although note 
#		that not all tunes will have splines generated. NOvA-maintained xsec 
#		splines for different GENIE versions and tunes can be found at 
#		/cvmfs/nova.opensciencegrid.org/externals/genie_xsec.
#	flux_file: the ROOT file containing the 1D flux histogram to use as input
#	flux_histo: the 1D histogram within the flux_file containing the flux
#	seed: the MC seed to use
#	target
#		A .txt file containing the target information in GENIE format. 
#		Examples are in the directory CONVENIENT/targets

# Exports
#	PRODUCTS (optional)
#		If running with the G21_11a_00_000 tune, this export adds a new 
#		location to the environment variable PRODUCTS that points GENIE to a 
#		build that is new enough to accomodate this tune.

# Sources
#	set_gevgen_variables.sh
#		Script that sets variables necessary for running GENIE's gevgen 
#		command. These include the min and max neutrino energies to 
#		consider, the message level, and the path to the full set of xsec 
#		splines.
#	$CONVENIENT_NUISANCE_DIR/set_nuisance_variables.sh
#		Script that sets variables necessary for running NUISANCE on the 
#		GENIE output. $CONVENIENT_NUISANCE_DIR is set according to the 
#		version of GENIE that was used to generate the events.

# Outputs
#	$nuisance_output_dir/$filepath/$filename_nuisance
#		The NUISANCE file created from the GENIE output
#	$convenient_output_dir/$filepath/$filename_convenient
#		The CONVENIENT file created from the NUISANCE output

# She-bang!
#!/bin/bash

# Read the inputs
genie_config=$2
genie_flux_file=$4
genie_flux_histo=$6
genie_seed=$8
genie_target=${10}
genie_rel_target=${genie_target/$CONVENIENT_TAR_DIR\//}

# Tell us what tune we're on
echo "Generating and processing GENIE $genie_config..."

# Check the target. This will affect the filenames
if [ ${genie_rel_target:0:4} == "NOvA" ];
then
	nova_switch=1
	filename="$genie_config.$genie_seed.$HC.${N_EVENTS:0:1}m.$FLAVOR"
	nuisance_output_dir=$CONVENIENT_NUISANCE_NOvA_OUTPUT_DIR
	convenient_output_dir=$CONVENIENT_NOvA_OUTPUT_DIR
else
	nova_switch=0
	target_txt=${genie_rel_target#*elements/}
	target=${target_txt%.txt}
	filename="$genie_config.$genie_seed.$HC.${N_EVENTS:0:1}m.$FLAVOR.$target"
	nuisance_output_dir=$CONVENIENT_NUISANCE_OUTPUT_DIR
	convenient_output_dir=$CONVENIENT_OUTPUT_DIR
fi


# Set the output file names
filename_raw=$filename.raw.root
filename_nuisance=GENIE:$filename.NUISANCE.root
filename_convenient=GENIE:$filename.convenient_output.root

# Setup the version of GENIE appropriate to the given tune.
case "$genie_config" in
	# GENIE tune to include SuSAv2 for QE and 2p2h
	G21_11a_00_000)
		# The splines for this tune are found in a different UPS 
		# directory.
		export PRODUCTS="$PRODUCTS:/cvmfs/fermilab.opensciencegrid.org/products/genie/local"	
		setup genie v3_04_00 -q e20:inclxx:prof
		setup genie_xsec v3_04_00 -q G2111a00000:e1000:k250
		setup genie_phyopt v3_04_00 -q dkcharmtau
		nuisance_setup_shell=$CONVENIENT_GEN_DIR/nuisance_GENIEv3_04_00_nuwro_neut/build/Linux/setup.sh
		generator_version=v3.04.00
		;;

	# Default GENIE tune for NOvA as of 21 August 2023
	G18_10a_02_11b)
		setup genie v3_04_00 -q e20:inclxx:prof
		setup genie_xsec v3_04_00 -q G1810a0211b:k250:e1000
		setup genie_phyopt v3_04_00 -q dkcharmtau
		nuisance_setup_shell=$CONVENIENT_GEN_DIR/nuisance_GENIEv3_04_00_nuwro_neut/build/Linux/setup.sh
		generator_version=v3.04.00
		;;

	# Same tune as above, except tuned to a different dataset
	G18_10a_02_11a)
		setup genie v3_04_00 -q e20:inclxx:prof
		setup genie_xsec v3_04_00 -q G1810a0211a:k250:e1000
		setup genie_phyopt v3_04_00 -q dkcharmtau
		nuisance_setup_shell=$CONVENIENT_GEN_DIR/nuisance_GENIEv3_04_00_nuwro_neut/build/Linux/setup.sh
		generator_version=v3.04.00
		;;

	# Intended GENIE tune used for NOvA Prod. 5 and 5.1
	# This tune was not created in practice. The MC that bears 
	# this tune are always to be reweighted using 
	# kProd5GenieSkewFix to go back to N18_10j_00_000.
	N18_10j_02_11a)
		setup genie v3_02_02a -q e20:inclxx:prof
		setup genie_xsec v3_02_02 -q N1810j0211a:k250:e1000
		setup genie_phyopt v3_02_00 -q dkcharmtau
		nuisance_setup_shell=$CONVENIENT_GEN_DIR/nuisance_GENIEv3_04_00_nuwro_neut/build/Linux/setup.sh
		generator_version=v3.02.02
		;;

	# In practice tune used for NOvA Prod 5 and 5.1
	# The implementation of this tune in Convenient is still in 
	# progress.
	G18_10j_00_000)
	 	setup genie v3_00_06p -q e20:inclxx:prof
		setup genie_xsec v3_00_06 -q G1810j00000:k250:e1000
		setup genie_phyopt v3_00_06 -q dkcharmtau:resfix
		nuisance_setup_shell=$CONVENIENT_GEN_DIR/nuisance_GENIEv3_00_06_nuwro_neut/build/Linux/setup.sh
		generator_version=v3.00.06
		;;

	# Current DUNE tune as of 21 August 2023
	AR23_20i_00_000)
		setup genie v3_04_00 -q e20:inclxx:prof
		setup genie_xsec v3_04_00 -q AR2320i00000:k250:e1000
		setup genie_phyopt v3_04_00 -q dkcharmtau
		nuisance_setup_shell=$CONVENIENT_GEN_DIR/nuisance_GENIEv3_04_00_nuwro_neut/build/Linux/setup.sh
		generator_version=v3.04.00
		;;

	# Resfixfix tune for N1810j0211a
	N18_10j_02_11a-resfixfix)
		setup genie v3_00_06p -q e20:inclxx:prof
		setup genie_xsec v3_00_06 -q N1810j0211a:e1000:k250:resfixfix
		setup genie_phyopt v3_00_06 -q dkcharmtau:resfixfix
		nuisance_setup_shell=$CONVENIENT_GEN_DIR/nuisance_GENIEv3_00_06_nuwro_neut/build/Linux/setup.sh
		generator_version=v3.00.06
		;;

	G18_01a_02_11a)
		setup genie v3_00_06p -q e20:inclxx:prof
		setup genie_xsec v3_00_06 -q G1801a0211a:e1000:k250
		setup genie_phyopt v3_00_06 -q dkcharmtau:resfix
		nuisance_setup_shell=$CONVENIENT_GEN_DIR/nuisance_GENIEv3_00_06_nuwro_neut/build/Linux/setup.sh
		generator_version=v3.00.06
		;;

	G18_02a_02_11a)
		setup genie v3_00_06p -q e20:inclxx:prof
		setup genie_xsec v3_00_06 -q G1802a0211a:e1000:k250
		setup genie_phyopt v3_00_06 -q dkcharmtau:resfixfix
		nuisance_setup_shell=$CONVENIENT_GEN_DIR/nuisance_GENIEv3_00_06_nuwro_neut/build/Linux/setup.sh
		generator_version=v3.00.06
		;;

	G18_02b_02_11a)
		setup genie v3_00_06p -q e20:inclxx:prof
		setup genie_xsec v3_00_06 -q G1802b0211a:e1000:k250
		setup genie_phyopt v3_00_06 -q dkcharmtau:resfixfix
		nuisance_setup_shell=$CONVENIENT_GEN_DIR/nuisance_GENIEv3_00_06_nuwro_neut/build/Linux/setup.sh
		generator_version=v3.00.06
		;;

	DefaultPlusMECWithNC)
		setup genie v2_12_10b -q e15:prof
		setup genie_xsec v2_12_10 -q DefaultPlusMECWithNC
		setup genie_phyopt v2_12_10 -q dkcharmtau
		nuisance_setup_shell=$CONVENIENT_GEN_DIR/nuisance_GENIEv2_12_10/build/Linux/setup.sh
		generator_version=v2.12.10
		;;

	# Everything else, throw an error
	*)
		echo "Error: Unknown how to set up GENIE for this tune."
		exit 1
		;;
esac

# Set the GENIE run variables. These include the range of 
# neutrino energies, the verbosity, the cross section splines 
# file, and the output file name.
source set_gevgen_variables.sh

# Run GENIE
# Format is 
	# -n number of events -f flux file,flux spectrum 
	# -p neutrino pdg 
	# -t target composition file in GENIE format
	# -o output file name
	# --seed MC seed to use --tune GENIE tune to use
# If we are running on the resfixfix version of N18_10j_02_11a, 
# then we switch the configuration only for the running of 
# gevgen so that the tune is correct.
if [ $genie_config == "N18_10j_02_11a-resfixfix" ];
then
	config=N18_10j_02_11a
	resfixfixswitch=1
else
	resfixfixswitch=0
fi
bash run_gevgen.sh \
	-n $N_EVENTS \
	-f $genie_flux_file,$genie_flux_histo \
	-p $NEUTRINO_PDG \
	-t $genie_target \
	-o $filename_raw \
	--seed $genie_seed \
	--tune $genie_config
if [ $resfixfixswitch -eq 1 ];
then
	config=N18_10j_02_11a-resfixfix
	resfixfixswitch=0
fi

# Run NUISANCE on the output from above.
# Format is
	# -i input file name	
	# -f flux file,flux spectrum 
	# -t target composition file in GENIE format
	# -o output file name
source $CONVENIENT_NUISANCE_DIR/set_nuisance_variables.sh

bash $CONVENIENT_NUISANCE_DIR/run_genie_nuisance.sh \
	-i $filename_raw \
	-f $genie_flux_file,$genie_flux_histo \
	-t $genie_target \
	-o $filename_nuisance \
	-n $nuisance_setup_shell

# Post-process the NUISANCE output to output the Convenient file.
# If we just used an older GENIE tune, it won't work anymore due 
# to a ROOT conflict, so switch back!
if [ $genie_config == "G18_10j_00_000" ] || [ $genie_config == "N18_10j_02_11a-resfixfix" ] || [ $genie_config == "DefaultPlusMECWithNC" ];
then
	setup genie v3_04_00 -q e20:inclxx:prof
fi
root -q "${CONVENIENT_NUISANCE_DIR}/make_convenient_from_nuisance.C(\"${filename_nuisance}\", \"${filename_convenient}\", 1)"

# Move the output files to the folders matching the program that 
# created them.
echo "GENIE run complete, moving files..."

# Delete the GENIE files to save space.
rm $filename_raw
rm $filename.raw.status
rm input-flux.root

# We want to save the files to either the NOvA specific 
# location, or the general location, depending on which target 
# was used.
if [ $nova_switch -eq 1 ];
then
	filepath=GENIE/${generator_version}_${genie_config}/"$HC$flux"/"$FLAVOR"_only
else
	filepath=GENIE/${generator_version}_${genie_config}/"$HC$flux"/"$FLAVOR"_only/$target
fi

# mkdir format is straightforward. 
# -p means make parent directories as well, if missing
mkdir -p $nuisance_output_dir/$filepath
mkdir -p $convenient_output_dir/$filepath
# mv format is [file to move] [new directory and/or name]
mv $filename_nuisance $nuisance_output_dir/$filepath/$filename_nuisance
mv $filename_convenient $convenient_output_dir/$filepath/$filename_convenient

