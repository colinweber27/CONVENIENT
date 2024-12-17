// Author: Colin Weber (webe1077@umn.edu)
// Date: 3 July 2023
// Purpose: To read the processed output of an event generator
//
// Command: root -q output_root_reader.C

// Includes
// ROOT includes
#include "TFile.h"
#include "TTree.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TTreeReaderArray.h"

// C++ includes
#include <vector>
#include <iostream>


void output_root_reader() { // Define function name
	// Initialize a pointer-to-TFile to open the processed ROOT file
	TFile* file = TFile::Open("no_inttype_new_usable_genie_from_nuisance.root");
	// Initialize a pointer-to-tree to point to the tree corresponding to 
	// the processor of interest.
	// TTree* tree = (TTree*) file->Get("genie_data");
	
	// Declare a TTreeReader to help read the tree. 
	// Format is (tree_name, file containing tree)
	TTreeReader reader("genie_data", file);
	// Declare TTreeReader arrays and values to hold the desired data from 
	// the tree. Format is (reader, leaf)
	TTreeReaderArray<std::vector<float>> pi0s(reader, "FS_Pi0s");
	TTreeReaderValue<int> n_pi0s(reader, "n_FS_Pi0s");
	TTreeReaderValue<int> nu_in_flavor(reader, "NuIn.Flavor");
	TTreeReaderValue<float> nu_in_energy(reader, "NuIn.E_in");
	TTreeReaderValue<int> target_pdg(reader, "target_PDG");

	// Loop over events and display information about each one
	while (reader.Next()) {
		// Display the target pdg
		std::cout << "Target PDG: " << *target_pdg << std::endl;

		// Display the incoming neutrino flavor and energy
		std::cout << "Flavor: " << *nu_in_flavor << std::endl;
		std::cout << "Incident Neutrino Energy: " << *nu_in_energy << \
			"GeV" << std::endl;
		
		// Display the number of FS Pi0s
		std::cout << "# FS Pi0s: " << *n_pi0s << std::endl;
		// Loop over all FS Pi0s and print the energy
		if (*n_pi0s == 0) {
			std::cout << "No Pi0s in this event.\n" << std::endl;
		}
		else {
			for (int j=0; j<*n_pi0s; ++j) {
				std::cout << "Event " << reader.GetCurrentEntry() << \
					", Pi0 " << j << ", Energy = " << pi0s[j][0] \
					<< "GeV\n" << std::endl;
			}
		}
	}
};
//
// Save for now, in the event we go back to more internal structure with FS 
// Particles.
/* struct FSParticles { 
Structure to hold info on FS Particles.
Each event contributes a vector of vectors.
The inner vector is the particle 4-momentum, and one of these is 
appended to the outer vector for each FS particle. The length of the 
outer vector is therefore the number of FS particles of that type in 
that event.
	std::vector<std::vector<float>> Protons;
	std::vector<std::vector<float>> Neutrons;
	std::vector<std::vector<float>> Gammas;
	std::vector<std::vector<float>> Pi0s;
	std::vector<std::vector<float>> PiPs;
	std::vector<std::vector<float>> PiMs;
	std::vector<std::vector<float>> KPs;
	std::vector<std::vector<float>> KMs;
	std::vector<std::vector<float>> K0s;
	std::vector<std::vector<float>> K0Bars;
	std::vector<std::vector<float>> Muons;
	std::vector<std::vector<float>> Electrons;
	std::vector<std::vector<float>> Taus;
	std::vector<std::vector<float>> NuMus;
	std::vector<std::vector<float>> NuEs;
	std::vector<std::vector<float>> NuTaus;
	std::vector<std::vector<float>> Others;
};*/
//
