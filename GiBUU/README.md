**Author: Colin Weber (webe1077@umn.edu)
Date: 26 March 2024
Purpose: Describe the contents of the directory Convenient/GiBUU**

# Summary
Contains scripts for running GiBUU within Convenient.

# `beam_energies`
Directory containing .dat files that are used to input a neutrino flux into GiBUU. They are constructed by `Convenient/GiBUU/make_beam_energy_dat.C` by translating a ROOT histogram containing the flux into a .dat file properly formatted for GiBUU. The histograms do not have to be normalized.

# `conveniently_run_gibuu.sh`
Wrapper for calling `run_gibuu.sh`. Is called by `Convenient/run.sh`.

# `*params.job`
GiBUU parameters files that can be used as input for a GiBUU run. The wildcard is used to briefly summarize the parameters set. `Defaultparams.job` is the default configuration.

# `GiBUU.x`
The link to the GiBUU executable, which is in `Convenient/Generators/GiBUU`.

# `make_beam_energy_dat.C`
Macro for translating a ROOT histogram containing the flux into a .dat file properly formatted for GiBUU. The GiBUU format is of the form "`left_edge_0`    `bin_height_0`\n`left_edge_1    bin_height_1`\n...". The histograms do not have to be normalized.

# `make_convenient_from_gibuu.C`
A macro for making a Convenient file from the raw GiBUU output. NUISANCE can't be used for this at the moment due to a bug, so this macro accomplishes the same thing.

# `run_gibuu.sh`
Runs GiBUU from within Convenient. Is called by `Convenient/GiBUU/conveniently_run_gibuu.sh`.

# `set_gibuu_variables.sh`
Sets variables for running GiBUU from within Convenient.
