// Includes
// ROOT includes
#include "TTree.h" // For working with trees
#include "TFile.h" // For writing files
#include "TH1.h" // For working with 1D histograms
#include "TTreeReader.h" // For creating a TTreeReader
#include "TTreeReaderValue.h" // For using the TTreeReader to read ordinary
							  // data types
#include "TTreeReaderArray.h" // For using the TTreeReader to read arrays

// C++ includes
#include <iostream> // For writing files
#include <vector> // For using vectors

// Define structures to be used for structuring the output file and TTree as desired
struct NuIn { // The structure that will hold the incoming neutrino info
	int flavor_PDG; // Incoming neutrino flavor, in the form of a PDG code
	float E_in; // Incoming neutrino energy, in units of GeV
};


// Declare a function to sort the particles in each event in order of
// decreasing energy
// The function will take as input a vector of a vector of floats, and 
// output the same vector, but sorted
std::vector<std::vector<float>> sort_by_energy(\
	std::vector<std::vector<float>> four_momenta) {
	// Sort the vector of vectors in place using the sort method. 
	// Format is (first object, last object, compare function). 
	std::sort(\
		// Iterate over all four-momenta
		four_momenta.begin(), four_momenta.end(), \
		// Compare the four-momenta by energy, the 0th entry.
		[](const std::vector<float>& a, const std::vector<float>& b) {
			return (a[0] > b[0]);
		}
	);
	// Return the sorted vector
	return four_momenta; 
}

void make_usable_root_via_nuisance() {
	// Import environment variables to construct input file path
	// NUISANCE output format
	TString nuisflat_format(getenv("NUISFLAT_FORMAT"));
	// File that NUISANCE processed
	TString nuisflat_input(getenv("NUISFLAT_INPUT"));
	// Current working directory
	TString cwd(get_current_dir_name());
	
	// Construct input file path and name using the above
	TString old_file_str(cwd + "/" + nuisflat_input + "." + \
		nuisflat_format + ".root");
		
	// Initialize a pointer-to-file that points to the NUISANCE output file
	TFile* old_file = TFile::Open(old_file_str);
	// delete old_file_str;
	// Initialize a TTreeReader to read the tree from the file.
	// Format is (tree name, pointer-to-file that contains tree)
	TTreeReader reader("FlatTree_VARS", old_file);
	
	// Initialize a new file that will hold the output tree
	std::unique_ptr<TFile> new_file(TFile::Open(\
		"convenient_output.root", "RECREATE"));
	// Initialize a tree to hold the processed data
	auto newtree = std::make_unique<TTree>("generator_data", \
										   "generator_data");

	// Initialize TTreeReaderValues to read the data from the old tree.
	// Use one TTreeReaderValue/Array for each leaf to be read.
	// Neutrino PDG
	TTreeReaderValue<int> PDGnu(reader, "PDGnu");
	// Neutrino energy
	TTreeReaderValue<float> Enu_true(reader, "Enu_true");
	// Target PDG
	TTreeReaderValue<int> tgt(reader, "tgt");
	// Event weight
	TTreeReaderValue<float> Weight(reader, "Weight");
	// fScaleFactor weight
	TTreeReaderValue<double> fScaleFactor(reader, "fScaleFactor");
	// PDG of FS particles
	TTreeReaderArray<int> pdg(reader, "pdg");
	// Energy of FS particles
	TTreeReaderArray<float> E(reader, "E");
	// 3-momenta of FS particles
	TTreeReaderArray<float> px(reader, "px");
	TTreeReaderArray<float> py(reader, "py");
	TTreeReaderArray<float> pz(reader, "pz");
	// Number of FS particles in each event
	TTreeReaderValue<int> nfsp(reader, "nfsp");
	// Interaction type flags
	TTreeReaderValue<bool> flagCCINC(reader, "flagCCINC");
	TTreeReaderValue<bool> flagNCINC(reader, "flagNCINC");

	// Initialize the structure that will hold the incoming neutrino 
	// information, and point a branch from the new tree to it.
	// Branch argument format is (title, pointer to struct that will fill 
	// the tree, {string telling ROOT what to name the branches within the 
	// structure, and the types that will fill those branches})
	NuIn nu_in;
	newtree->Branch("NuIn", &nu_in, "Flavor/I:E_in/F");

	// Initialize the object that will hold the PDG code of the hit nucleus, 
	// and point a branch to it.
	int target_nucleus;
	newtree->Branch("target_PDG", &target_nucleus);

	// Initialize the object that will hold the event weight, and point a 
	// branch to it.
	float event_weight;
	newtree->Branch("EventWeight", &event_weight);

	// Initialize the object that will hold the event weight, and point a 
	// branch to it.
	double scale_factor;
	newtree->Branch("fScaleFactor", &scale_factor);

	// Initialize the objects that will hold the interaction type flags, and 
	// point branches to them.
	bool flagCC;
	bool flagNC;
	newtree->Branch("flagCC", &flagCC);
	newtree->Branch("flagNC", &flagNC);

	// Initialize vectors of vectors that will hold the FS particle 
	// 4-momenta, and point branches to each of the vectors of vectors.
	std::vector<std::vector<float>> fs_protons;
	std::vector<std::vector<float>> fs_neutrons;
	std::vector<std::vector<float>> fs_gammas;
	std::vector<std::vector<float>> fs_pi0s;
	std::vector<std::vector<float>> fs_pips;
	std::vector<std::vector<float>> fs_pims;
	std::vector<std::vector<float>> fs_muons;
	std::vector<std::vector<float>> fs_electrons;
	std::vector<std::vector<float>> fs_taus;
	std::vector<std::vector<float>> fs_nus;
	std::vector<std::vector<float>> fs_others;
	newtree->Branch("FS_Protons", &fs_protons);
	newtree->Branch("FS_Neutrons", &fs_neutrons);
	newtree->Branch("FS_Gammas", &fs_gammas);
	newtree->Branch("FS_Pi0s", &fs_pi0s);
	newtree->Branch("FS_PiPs", &fs_pips);
	newtree->Branch("FS_PiMs", &fs_pims);
	newtree->Branch("FS_Muons", &fs_muons);
	newtree->Branch("FS_Electrons", &fs_electrons);
	newtree->Branch("FS_Taus", &fs_taus);
	newtree->Branch("FS_Nus", &fs_nus);
	newtree->Branch("FS_Others", &fs_others);

	// Initialize integers to count the numbers of particles in each event 
	// for speed
	int nfsprotons;
	int nfsneutrons;
	int nfsgammas;
	int nfspi0s;
	int nfspips;
	int nfspims;
	int nfsmuons;
	int nfselectrons;
	int nfstaus;
	int nfsnus;
	int nfsothers;
	newtree->Branch("n_FS_Protons", &nfsprotons);
	newtree->Branch("n_FS_Neutrons", &nfsneutrons);
	newtree->Branch("n_FS_Gammas", &nfsgammas);
	newtree->Branch("n_FS_Pi0s", &nfspi0s);
	newtree->Branch("n_FS_PiPs", &nfspips);
	newtree->Branch("n_FS_PiMs", &nfspims);
	newtree->Branch("n_FS_Muons", &nfsmuons);
	newtree->Branch("n_FS_Electrons", &nfselectrons);
	newtree->Branch("n_FS_Taus", &nfstaus);
	newtree->Branch("n_FS_Nus", &nfsnus);
	newtree->Branch("n_FS_Others", &nfsothers);

	// Loop over the events in the NUISANCE output
	while (reader.Next()) {
		// Assign relevant info to the NuIn structure		
		nu_in.flavor_PDG = *PDGnu;
		nu_in.E_in = *Enu_true;

		// Assign target nucleus PDG
		target_nucleus = *tgt;

		// Assign event weight
		event_weight = *Weight;

		// Assign scale factor
		scale_factor = *fScaleFactor;

		// Assign interaction type flags
		flagCC = *flagCCINC;
		flagNC = *flagNCINC;

		// Initialize vectors to be filled with info on FS particles
		std::vector<std::vector<float>> protons;
		std::vector<std::vector<float>> neutrons;	
		std::vector<std::vector<float>> gammas;
		std::vector<std::vector<float>> pi0s;
		std::vector<std::vector<float>> piPs;
		std::vector<std::vector<float>> piMs;
		std::vector<std::vector<float>> muons;
		std::vector<std::vector<float>> electrons;
		std::vector<std::vector<float>> taus;
		std::vector<std::vector<float>> nus;
		std::vector<std::vector<float>> others;
		// Initialize integers to count the number of particles of each type
		int n_protons = 0;	
		int n_neutrons = 0;
		int n_gammas = 0;
		int n_pi0s = 0;
		int n_piPs = 0;
		int n_piMs = 0;
		int n_muons = 0;
		int n_electrons = 0;
		int n_taus = 0;
		int n_nus = 0;
		int n_others = 0;

		// Fill info on FS particles
		// Loop over indices less than the number of FS particles
		for (int j = 0; j < *nfsp; j++) {
			// We don't care about hadronic systems/blobs or FS nuclei
			if(pdg[j] > 9999999) continue;

			// Grab PDG and kinematic data, and organize into a 4-vector
			int pdg_part = pdg[j];
			float e_part = E[j];
			float px_part = px[j];
			float py_part = py[j];
			float pz_part = pz[j];
			std::vector<float> p4 {e_part, px_part, py_part, pz_part};

			// Use PDG to assign 4-vector to appropriate particle vector
			if(pdg_part == 2212) {protons.push_back(p4); n_protons++;}
			else if(pdg_part == 2112) {neutrons.push_back(p4); n_neutrons++;}
			else if(pdg_part == 22) {gammas.push_back(p4); n_gammas++;}
			else if(pdg_part == 111) {pi0s.push_back(p4); n_pi0s++;}
			else if(pdg_part == 211) {piPs.push_back(p4); n_piPs++;}
			else if(pdg_part == -211) {piMs.push_back(p4); n_piMs++;}
			else if(abs(pdg_part) == 13) {muons.push_back(p4); n_muons++;}
			else if(abs(pdg_part) == 11) {
				electrons.push_back(p4); n_electrons++;
			}
			else if(abs(pdg_part) == 15) {taus.push_back(p4); n_taus++;}
			else if(abs(pdg_part) == 14 || abs(pdg_part) == 12 || \
				abs(pdg_part) == 16) {nus.push_back(p4); n_nus++;}
			else {others.push_back(p4); n_others++;}
		}

		// Sort the vectors of vectors by energy
		std::vector<std::vector<float>> sorted_protons = \
			sort_by_energy(protons);
		std::vector<std::vector<float>> sorted_neutrons = \
			sort_by_energy(neutrons);
		std::vector<std::vector<float>> sorted_gammas = \
			sort_by_energy(gammas);
		std::vector<std::vector<float>> sorted_pi0s = \
			sort_by_energy(pi0s);
		std::vector<std::vector<float>> sorted_piPs = \
			sort_by_energy(piPs);
		std::vector<std::vector<float>> sorted_piMs = \
			sort_by_energy(piMs);
		std::vector<std::vector<float>> sorted_muons = \
			sort_by_energy(muons);
		std::vector<std::vector<float>> sorted_electrons = \
			sort_by_energy(electrons);
		std::vector<std::vector<float>> sorted_taus = \
			sort_by_energy(taus);
		std::vector<std::vector<float>> sorted_nus = \
			sort_by_energy(nus);
		std::vector<std::vector<float>> sorted_others = \
			sort_by_energy(others);

		// Assign vector of vectors to the appropriate spot in the 
		// FSParticles structure
		fs_protons = sorted_protons;
		fs_neutrons = sorted_neutrons;
		fs_gammas = gammas;
		fs_pi0s = sorted_pi0s;
		fs_pips = sorted_piPs;
		fs_pims = sorted_piMs;
		fs_muons = sorted_muons;
		fs_electrons = sorted_electrons;
		fs_taus = sorted_taus;
		fs_nus = sorted_nus;
		fs_others = sorted_others;
		// Assign particle counters to the appropriate leaves
		nfsprotons = n_protons;
		nfsneutrons = n_neutrons;
		nfsgammas = n_gammas;
		nfspi0s = n_pi0s;
		nfspips = n_piPs;
		nfspims = n_piMs;
		nfsmuons = n_muons;
		nfselectrons = n_electrons;
		nfstaus = n_taus;
		nfsnus = n_nus;
		nfsothers = n_others;

		// Fill the tree
		newtree->Fill();
		// We do not need to clear other variables because they will be 
		// reinitialized the next time through the loop.
	}
	// After looping through all events, write the tree to the new file
	newtree->Write();

	// Finally, grab the flux histogram from the NUISANCE file, which we will
	// use for calculating cross sections.
	TH1D* flux = (TH1D*) old_file->Get("FlatTree_FLUX");
	flux->Write();

	// Close the old file so it doesn't remain open (and, god forbid, writeable!) if we stay in ROOT
	old_file->Close();	
}
//
//
// Save for now, in the event that we bring back interaction types
/*	bool flagCCINC;
	old_tree->SetBranchAddress("flagCCINC", &flagCCINC);
	bool flagNCINC;
	old_tree->SetBranchAddress("flagNCINC", &flagNCINC);

	bool flagCCQELike;
	old_tree->SetBranchAddress("flagCCQELike", &flagCCQELike);

	bool flagCCQE;
	old_tree->SetBranchAddress("flagCCQE", &flagCCQE);

	bool flagNCEL;
	old_tree->SetBranchAddress("flagNCEL", &flagNCEL);

	bool flagCCcoh;
	old_tree->SetBranchAddress("flagCCcoh", &flagCCcoh);

	bool flagNCcoh;
	old_tree->SetBranchAddress("flagNCcoh", &flagNCcoh);

	int Mode;
	old_tree->SetBranchAddress("Mode", &Mode);
	std::vector<int> res_mode_list {17, 22, 23, 38, 39, 42, 43, 44, 45};
	std::vector<int> dis_mode_list {46};*/
//
/*  std::cout << flagCCINC << std::endl;
	std::cout << "Mode: " << Mode << "\n" << std::endl;
	if(flagCCINC) {
		if(flagCCQE) int_type = 10;
		else if(flagCCQELike && ~flagCCQE) int_type = 11;
		else if(std::find(res_mode_list.begin(), res_mode_list.end(), Mode) != res_mode_list.end()) int_type = 12;
		else if(std::find(dis_mode_list.begin(), dis_mode_list.end(), Mode) != dis_mode_list.end()) int_type = 13;
		else if(flagCCcoh) int_type = 14;
		else int_type = 19;
	}
	else if(flagNCINC) {
		if(flagNCEL) int_type = 20;
		// No NC MEC possible b/c not implemented in NEUT
		else if(std::find(res_mode_list.begin(), res_mode_list.end(), Mode) != res_mode_list.end()) int_type = 22;
		else if(std::find(dis_mode_list.begin(), dis_mode_list.end(), Mode) != dis_mode_list.end()) int_type = 23;
		else if(flagNCcoh) int_type = 24;
		else int_type = 29;
	}
	else {
		throw runtime_error(Form("Encountered interaction type in \
			Event %d that is neither CC or NC.", i));
	} */
//
