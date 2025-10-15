**Author: Colin Weber (webe1077@umn.edu)
Date: 26 March 2024
Purpose: Describethe contents of the directory Convenient/NuWro**

## Summary
Contains scripts for running NuWro within Convenient.

### `*params.txt`
NuWro parameters files that can be used as input for a NuWro run. The wildcard is used to briefly summarize the parameters set. `Defaultparams.txt` is the default configuration.

### `beam_energies`
Directory containing text files that are used to input a neutrino flux into NuWro. They are constructed by `make_beam_energy.C` by translating a ROOT histogram containing the flux into a .txt file properly formatted for NuWro. The NuWro format is of the form "`left_edge` `right_edge` `bin1_height` `bin2_height`...", where `bin1_height` is the first bin height of the histogram read in as input. The histograms do not have to be normalized.

### `conveniently_run_nuwro.sh`
Wrapper for calling `run_nuwro.sh`. Is called by `Convenient/run.sh`.

### `convert_genie_comps_to_nuwro.sh`
Reads in a .txt file containing a detector composition in GENIE format, and outputs a .txt file containing the detector composition in NuWro format. This function is implemented in `run_nuwro.sh`, but in that case, the output is piped to the params file.

### `make_beam_energy.C`
Macro for translating a ROOT histogram containing the flux into a .txt file properly formatted for NuWro. The NuWro format is of the form "`left_edge` `right_edge` `bin1_height` `bin2_height`...", where `bin1_height` is the first bin height of the histogram read in as input. The histograms do not have to be normalized.

### `run_nuwro.sh`
Runs NuWro from within Convenient. Is called by `CONVENIENT/NuWro/conveniently_run_nuwro.sh`

### `set_nuwro_variables.sh`
Sets variables for running NuWro from within CONVENIENT.
