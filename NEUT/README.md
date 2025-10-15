**Author: Colin Weber (webe1077@umn.edu)
Date: 26 March 2024
Purpose: Describe the contents of the directory Convenient/NEUT**

## Summary
Contains scripts for running NEUT within Convenient.

### `beam_energies`
Directory containing .root files that are used to input a neutrino flux into NEUT. They are constructed by `Convenient/NEUT/make_beam_energy_txt.C` and `make_flux_hist_root.C` by translating a ROOT histogram containing the flux into another .root file properly formatted for NEUT. NEUT can't handle flux bins less than 100 MeV, so what these files do is remove the flux in those bins. The histograms do not have to be normalized.

### `conveniently_run_neut.sh`
Wrapper for calling `run_neut.sh`. Is called by `Convenient/run.sh`.

### `*params.card`
NEUT parameters files that can be used as input for a NEUT run. The wildcard is used to briefly summarize the parameters set. `Defaultparams.card` is the default configuration.

### `make_beam_energy_txt.C`
Macro for translating a ROOT histogram containing the flux into a .txt file with bins below 100 MeV zeroed. The output text is of the form "`left_edge_0`    `bin_height_0`\n`left_edge_1    bin_height_1`\n...". The histograms do not have to be normalized.

### `make_flux_hist_root.C`
Macro for translating a .txt histogram output from `make_beam_energy_txt.C` into a .root histogram. The input text is of the form "`left_edge_0`    `bin_height_0`\n`left_edge_1    bin_height_1`\n...". The histograms do not have to be normalized.

### `run_neut.sh`
Runs NEUT from within Convenient. Is called by `Convenient/NEUT/conveniently_run_neut.sh`.

### `set_neut_variables.sh`
Sets variables for running NEUT from within Convenient.
