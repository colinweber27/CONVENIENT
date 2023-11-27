Author: Colin M Weber (webe1077@umn.edu)
Date: 5 November 2023
Purpose: Describethe contents of the directory /nova/app/users/colweber/Convenient

GENIE
	Holds scripts for running GENIE from within the Convenient program.

NUISANCE
	Contains the NUISANCE files outputted from processing the generator outputs. Also contains shells and macros for running NUISANCE within the Convenient program.

NuWro
	Holds scripts for running NuWro from within the Convenient program.

UsersGuide.txt
	File explaining how to generate events and use the xsec_analysis_macro.

compositions
	Directory containing information about the composition of the NOvA detectors to use as inputs to Convenient. See the README.txt file within this folder for more information. 

flux
	Directory containing flux histograms to use as input to Convenient. For more info, look at the README.txt files in e.g. the subdirectory flux/NuMI_naive_flux/FHC

generator_outputs
	Directory containing the Convenient outputs. This is where the data resides that will be most useful to analyzers. See the README.txt file within this folder for more information.

output_root_reader.C
	Macro for checking whether the Convenient outputs are giving expected results.

run.sh
	Shell script for generating events with Convenient. This does not need to be changed before generating events.

set_run_variables.sh
	Shell script for setting variables for a Convenient run. This script determines the Convenient inputs, so all elements should be checked prior to a run.

xsec_analysis_macro.C
	A macro that extracts cross sections from Convenient outputs. This is the macro that analyzers should use when performing their own analyses.
