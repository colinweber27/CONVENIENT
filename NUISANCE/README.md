**Author: Colin Weber (webe1077@umn.edu)
Date: 26 March 2024
Purpose: Describe the contents of the directory Convenient/NUISANCE**

# Summary
This directory contains the shell scripts and C++ macros used to run NUISANCE from within Convenient. 

# `make_convenient_from_convenient.sh`
To read in a list of Convenient files and combine them. This macro does no reweighting, so it assumes that the event weight has already been set appropriately.

# `make_convenient_from_convenient_newNustruct.C`
Reads in files made with the Nu structure and flattens that structure.

# `make_convenient_from_convenient_reweight.C`
Reweights all events in an input convenient file using the input weight. The weights are accounted in the branch `EventWeight`. 

# `make_convenient_from_nuisance.C`
Macro to read a NUISANCE output and create the corresponding Convenient output. This is the macro that pares down the NUISANCE output to a minimal set of variables neeeded for xsec analyses. Also takes a weight as an input, allowing for reweighting.

# `run_*_nuisance.sh`
Runs NUISANCE on an event file created by the generator in the wildcard.

# `set_nuisance_variables.sh`
Sources variables necessary for running NUISANCE.
