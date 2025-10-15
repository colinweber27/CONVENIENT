# Introduction
When simulating neutrino-nucleus interactions in a neutrino event generator, various models are introduced at several different stages in the simulation. A nuclear model determines the initial momenta of all nucleons in the nucleus. Cross section models for different interaction types determine what interaction takes place, and the final state kinematics of the initially produced particles. Other models account for hadronization. Finally, there is a model for final state interactions.

These models should be thought of as tools to allow us to do simulations, first and foremost. The underlying physics is unaware of these models, and continues to work just fine without our having developed event generators. For this reason, we need to be careful to not let model-dependence creep into our cross section results. Something that can aid against this is comparing results to MC generated with other physics models and generators.

Unfortunately, this is easier said than done. Each generator is unique, doing some things well while lacking in other areas. This makes it difficult to even unify the output file format, let alone dig into the models used to generate events. Each generators takes inputs differently as well, adding another wrinkle to the mess. Even compiling each generator can be a challenge.

To overcome these difficulties, I have developed Convenient. Convenient is best thought of as a program to generate events using different generators with different configurations and tunes. The data it outputs maximizes events per byte by minimizing the data redundancy in the events. (For example, it is redundant to keep the primary lepton's scattering angle if its 3-momentum and identity are also kept.) Convenient also includes a macro for extracting differential cross sections from the generated events. These capabiliies will allow analyzers to compare their results to different models, thereby promoting model-independence.

# Setup
### Only needs to be done if you want to generate events
CONVENIENT is designed to work on the NOvA GPVMs with novasoft release `S25-01-06`. This ensures that the various dependencies are properly set up. The first step is therefore to set up novaosft:
```
source /cvmfs/nova.opensciencegrid.org/novasoft/setup/setup_nova.sh "$@"
cd /exp/nova/app/users/$USER
spack load nova-grid-utils
sl7-nova -r S25-01-06
export BEARER_TOKEN_FILE=/tmp/bt_token_nova_Analysis_$(id -u)
```
CONVENIENT can then be built with the following commands:
```
git clone -b unbuilt https://github.com/colinweber27/CONVENIENT.git
cd CONVENIENT
```
This command will ask you a series of questions about which generators you want to enable. Each prompt should be answered with either "ON" or "OFF". GENIE is enabled by default. If you want to enable NEUT, you will also be prompted to enter your GitHub username that is associated with NEUT, in addition to another personal access token. You may also select which generator version to build. NUISANCE is set up automatically based on the generators enabled.
```
source build_convenient.sh
```
This last command should be run each time Convenient is used from a new shell.
```
source setup_convenient.sh
```

# Step-by-step guide for generating events
So you want to generate some neutrino nucleus interactions using a bunch of different tunes and configurations from a bunch of different generators? You've come to the right place.

First, you should figure out how many events you want and with what generators/tunes/configurations. 

## GENIE
The different models that go into each GENIE tune are listed at [the GENIE webpage.](http://www.genie-mc.org/) Not all GENIE tunes are supported by Convenient, but the most relevant ones for NOvA are. Currently supported tunes are

- DefaultPlusMECWithNC
- G18_01a_02_11a
- G18_02a_02_11a
- G18_02b_02_11a
- G18_10a_02_11a
- G18_10a_02_11b
- G18_10j_00_000
- N18_10j_02_11a
- N18_10j_02_11a-resfixfix
- AR23_20i_00_000
- G21_11a_00_000

If you try to run an unsupported tune, Convenient will exit and return an error. If you would like a new tune added, please message me. 
	
## NuWro
NuWro has several different models that can be swapped in and out by modifying the parameters file that is inputed to the generator. For NOvA, we generally run NuWro with the default parameters. The models that are included in this default configuration are listed in Slide 50 of NOvA DocDB 61949-v3. For examples and documentation, please consult the following sources:
- `Convenient/NuWro/20230921params.txt` is an example of a 
	parameters file ready to go
- [The section of the NuWro User's Guide describing the different parameters and their use.](https://nuwro.github.io/user-guide/getting-started/parameters/)
	
## NEUT
- In order to work with NEUT, you need to get permission to download the source code from [the private NEUT repository.](https://github.com/neut-devel) Contact any of the developers at the bottom of this page and give them your GitHub username to get read-only access.
- NEUT is more difficult to work with in terms of swapping new models in and out, so for NOvA analyses we generally run it in the default configuration. The models that are included in this configuration are listed in Slide 50 of NOvA DocDB 61949-v3. The card file, which configures the generator and also contains model information, is located at `Convenient/NEUT/Defaultparams.card`.

## GiBUU
- GiBUU is also more difficult to work with in terms of swapping different models, so we use the default in most NOvA analyses. Infromation on the GiBUU physics content is listed in Slide 50 of NOvA DocDB 61949-v3, or available [on the GiBUU homepage.](https://gibuu.hepforge.org/trac/wiki) See too the default jobcard at `Convenient/GiBUU/Defaultparams.job`.

## Other models
- If you want events with other models or generators, please message me.

If you only want a few events for one of the supported tunes, chances are these events already exist in the `/exp/nova/data/users/colweber/ConvenientOutputs_NOvA` directory. If you can't find the events in there, double check with me to see if they exist elsewhere.

If you want many events, or events for an unsupported tune or novel generator configuration, please message me. For now, it's easiest if I generate those events and direct you to the output. If, however, you want more control over the process, or your task is too large for my bandwidth, it is not too hard to generate the events yourself.
1. I assume if you've gotten this far, you've checked to see if your desired tune/generator configuration exists. If it does, move on. If not, please reread this guide from the top of this section.
2. Open the script `Convenient/set_run_variables.sh`. This is the only script that needs to be modified before generating events.
3. Modify the run variables as appropriate. In particular,
   - `N_EVENTS` is where you set the number of events to generate
   - `HC` determines the NuMI horn current polarity. Remember that FHC selects neutrinos, and RHC selects antineutrinos.
   - `FLUXES` determines the flux to use. Options are any folder in the directory `Convenient/flux/NuMI/$HC`. This may be a comma-separated list of fluxes to use in consecutive runs of Convenient.
   - `FLUX_FILE_NAMES` is a comma-separated list of flux files to use in consecutive runs of Convenient. The *i*th flux file in the list must be located in the *i*th directory in the `FLUXES` list.
    - `FLUX_HISTOS` is a comma-separated list of histograms containing the actual neutrino fluxes to use as input. The *i*th `FLUX_HISTO` entry must be within the ith flux file name in `FLUX_FILE_NAMES`.
    - `NEUTRINO_PDG` is the PDG code for the neutrino flavor you'd like to be incident on the target.
    - `GIBUU_CC_NC` is either "CC" or "NC" and is only used when generating GiBUU events.
    - `TARGETS` is the path to the text file containing the desired target composition. For more info on Convenient target compositions, please see `Convenient/targets/README.md`.
    - `RUNS` determines the tunes/generators/configurations to use. It may be a comma-separated list, where each item is a tune/generator/configuration string to use in consecutive Convenient runs.
    - `SEED` detemines the MC seed to use. It may be a comma-separated list, where each item is a seed to use in consecutive Convenient runs.
4. Set the run variables by running `source set_run_variables.sh` from within the root Convenient directory.
5. Run Convenient with the command `source run.sh	.
6. Outputs will be found in `/exp/nova/data/users/$USER/ConvenientOutputs[_NOvA]`.

## What info does CONVENIENT keep?
1. Neutrino energy (`float Enu_true`) and PDG (`int PDGnu`). Momentum is assumed to be in the z-direction.
2. Target PDG (`int target_PDG`).
3. CC/NC flag (bool flagCC / bool flagNC).
4. NEUT Interaction type code (int NEUT_int_type).
5. 4-momenta of all final state particles, which are identified as (anti-)protons, neutrons, photons, pi+, pi-, pi0, (anti-)muons, electrons/positrons, (anti-)taus, neutrinos, or other. Particles identified as other have a pdg code attached as a 5th element of the 4-momentum vector. Branches for these variable are e.g. `std::vector<std::vector<float>> FS_Protons`.
6. The multiplicity of each final state particle (e.g. `int n_FS_Protons`).
7. Flux used to generate events (`TH1 FlatTree_FLUX`).
8. Generator scale factor, which partially converts from an event histogram to a cross section, and is the same for all events in a given run for a given generator (`double GenScaleFactor`). For GENIE, NuWro, and NEUT, this is calculated by NUISANCE. For GiBUU, it is 1E-38.
9. An event weight, which includes all other factors not included in the generator scale factor, and not accounting for the relative elemntal abundances in a specific detector (`double EventWeight`). It can be unique for each event. 

# Step-by-step guide for running analyses
So you want to calculate a cross section for a bunch of models and generators? You've come to the right place.

This is done with the `xsec_analysis_macro.C` file. This file has been heavily-commented, so direct your attention there first to learn how to use it to perform an analysis. The sections that will need to be modified for an individual analysis are:
1. Global constants, such as masses or detector thresholds
2. Phase space cuts and other useful functions
3. The data files and MC files (starting at Line 113)
   a. Analyzers should use Convenient files for their MC generator predictions. Convenient files are currently located at `/exp/nova/data/users/colweber/ConvenientOutputs[_NOvA]`. To see what files are available, look at `Convenient/ConvenientOutputsList.txt[_NOvA]`.
4. Binning
5. Selection of integrated/flux-averaged cross sections and number of differential cross section variables.
6. Defining xsec variables and filling of histograms
7. Plotting.

As always, message me with any questions.

# Organization
- BuildGenerators
  - Directory containing scripts used to build the generators themselves within Convenient.
- ConvenientOutputsList.txt
  - File listing all the Convenient outputs. Each file on this list corresponds to a single target nucleus.
- ConvenientOutputs_NOvAList.txt
  - File listing all the Convenient outputs w/ NOvA ND elemental abundance weights applied, organized as follows:
    - Generator:config
      - Flux
        - neutrino flavor
          - Num. of events for the generator config, flux, and neutrino flavor
          - File names matching the generator config, flux, and neutrino flavor
- documentation_generation_scripts
  - Directory containing shell scripts that generate the appropriate text in ConvenientOutputsList.txt for each Convenient run, and the .txt file accompanying each convenient_output file.
- flux
  - Directory containing flux files (which contain the flux histograms) to use as input to Convenient. Fluxes are organized according to the following structure:
    - Beam
      - Horn current polarity
        - Flux options (i.e. method of creation, weights, bin width)
          - .root files specifying the neutrino flavor they contain. 
          - Histogram names vary. The easiest way to find the histogram name within a desired flux file is to open that file in a TBrowser.
- Generators
  - Holds directories containing the source files for the generators in Convenient. It does not contain source files for GENIE, since Convenient uses the GENIE within novasoft.
- GENIE
  - Holds scripts for running GENIE from within the Convenient program.
- GiBUU
  - Holds scripts for running GiBUU from within the Convenient program.
- global_vars.sh
  - A shell variable to be sourced when setting up Convenient. Contains global variables and a series of setup functions to set up different dependencies using UPS.
- NEUT
  - Holds scripts for running NEUTfrom within the Convenient program.
- NUISANCE
  - Contains the NUISANCE files outputted from processing the generator outputs and scripts for running NUISANCE from within Convenient.
- NuWro
  - Holds scripts for running NuWro from within Convenient.
- output_root_reader.C
  - Macro for checking whether the Convenient outputs are giving expected results.
- run.sh
  - Shell script for generating events with Convenient. This does not need to be changed before generating events.
- set_run_variables.sh
  - Shell script for setting variables for a Convenient run. This script determines the Convenient inputs, so all elements should be checked prior to a run.
- setup_convenient.sh
  - Shell script for setting up Convenient before a run. It sources global_vars.sh and sources setup_generators.sh.
- setup_generators.sh
  - Shell script for setting environment variables needed to run the generators, and sourcing NUISANCE.
- specific_xsec_analysis_scripts
  - Directory containing more-specific versions of xsec_analysis_macro.C that I have used to generate various plots to demonstrate the utility of Convenient.
- targets
  - Directory containing information about the composition of the NOvA detectors to use as inputs to Convenient. See the README.md file within this folder for more information. 
- xsec_analysis_macro.C
  - A macro that extracts cross sections from Convenient outputs. This is the macro that analyzers should use when performing their own analyses.

# Final notes
Convenient, while currently serviceable, is still being developed. If any bugs are encountered, please message me to get them sorted out.


**Author: Colin M Weber (webe1077@umn.edu)**
**Date: 5 November 2023**
**Last Updated: 17 December 2024**
