// Author: Colin Weber (webe1077@umn.edu)
// Date: 11 July 2023
// Purpose: To read in a NUISANCE file and output a file 
// (convenient_output.root) that includes only a minimal set of variables 
// needed for xsec analyses.

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

std::vector<std::vector<float>> sort_by_energy(\
	std::vector<std::vector<float>> momentum_vector) {
	/* Sort the momentum vector of vectors along the first axis of the inner 
	vector in decreasing order. Can be used to order a group of four-vectors
	by decreasing energy. 

	Parameters
	----------
	std::vector<std::vector<float>> momentum vector
		A vector containing the vectors to sort.

	Returns
	-------
	std::vector<std::vector<float>> momentum vector
		The vector of vectors, sorted along the first axis of the inner 
		vectors in decreasing order. */


	// Sort the vector of vectors in place using the sort method. 
	// Format is (first object, last object, compare function). 
	std::sort(\
		// Iterate over all four-momenta
		momentum_vector.begin(), momentum_vector.end(), \
		// Compare the four-momenta by energy, the 0th entry.
		[](const std::vector<float>& a, const std::vector<float>& b) {
			return (a[0] > b[0]);
		}
	);
	// Return the sorted vector
	return momentum_vector; 
}

void make_convenient_from_nuisance(TString input="", TString outname="convenient_output.root", double element_abundance_weight=1) {
	/* Takes a NUISANCE output and pares it down to a Convenient output file 
	with a minimal set of variables needed for xsec analyses. Returns 
	nothing, but outputs a Convenient file.*/

	// Current working directory
	TString cwd(get_current_dir_name());
	
	// Construct input file path and name using the above
	TString old_file_str(cwd + "/" + input);
		
	// Initialize a pointer-to-file that points to the NUISANCE output file
	TFile* old_file = TFile::Open(old_file_str);

	// Initialize a TTreeReader to read the tree from the file.
	// Format is (tree name, pointer-to-file that contains tree)
	TTreeReader reader("FlatTree_VARS", old_file);
	
	// Initialize a new file that will hold the output tree
	std::unique_ptr<TFile> new_file(TFile::Open(\
		outname, "RECREATE"));
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
	TTreeReaderValue<float> gen_event_weight(reader, "Weight");
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
	TTreeReaderValue<int> Mode(reader, "Mode");

	// Initialize the structure that will hold the incoming neutrino 
	// information, and point a branch from the new tree to it.
	// Branch argument format is (title, pointer to struct that will fill 
	// the tree, {string telling ROOT what to name the branches within the 
	// structure, and the types that will fill those branches})
	float Enu;
	newtree->Branch("Enu", &Enu);
	int PDG_nu;
	newtree->Branch("PDGnu", &PDG_nu);

	// Initialize the object that will hold the PDG code of the hit nucleus, 
	// and point a branch to it.
	int target_nucleus;
	newtree->Branch("target_PDG", &target_nucleus);

	// Initialize the object that will hold the event weight, and point a 
	// branch to it. The event weights are the event-by-event weight given 
	// by the generator (the generator event weight, or gen_event_weight), 
	// multiplied by any weights needed to account for relative elemental 
	// abundances (the elemental_abundance_weight that is inputted). The 
	// gen_event_weights for NEUT, GENIE, and NuWro are 1, and for GiBUU the 
	// gen_event_weight is the perweight. The relative abundance weight 
	// depends on the target used to generate events, and the desired output.
	double event_weight;
	newtree->Branch("EventWeight", &event_weight);

	// Initialize the object that will hold the generator scale factor, and 
	// point a branch to it. The generator scale factor is the same for all 
	// events of a given run of a generator. For NEUT, GENIE, and NuWro, it 
	// is equivalent to the fScaleFactor given by NUISANCE. For GiBUU, it is 
	// 1E-38.
	double gen_scale_factor;
	newtree->Branch("GenScaleFactor", &gen_scale_factor);

	// Initialize the objects that will hold the interaction type info, and 
	// point branches to them.
	bool flagCC;
	bool flagNC;
	int NEUT_int_type;
	newtree->Branch("flagCC", &flagCC);
	newtree->Branch("flagNC", &flagNC);
	newtree->Branch("NEUT_int_type", &NEUT_int_type);

	// Initialize vectors of vectors that will hold the FS particle 
	// 4-momenta, and point branches to each of the vectors of vectors.
	std::vector<std::vector<float>> fs_protons;
	std::vector<std::vector<float>> fs_antiprotons;
	std::vector<std::vector<float>> fs_neutrons;
	std::vector<std::vector<float>> fs_antineutrons;
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
	newtree->Branch("FS_Antiprotons", &fs_antiprotons);
	newtree->Branch("FS_Neutrons", &fs_neutrons);
	newtree->Branch("FS_Antineutrons", &fs_antineutrons);
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
	int nfsantiprotons;
	int nfsneutrons;
	int nfsantineutrons;
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
	newtree->Branch("n_FS_Antiprotons", &nfsantiprotons);
	newtree->Branch("n_FS_Neutrons", &nfsneutrons);
	newtree->Branch("n_FS_Antineutrons", &nfsantineutrons);
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
		PDG_nu = *PDGnu;
		Enu = *Enu_true;

		// Assign target nucleus PDG
		target_nucleus = *tgt;

		// Assign event weight
		event_weight = *gen_event_weight * element_abundance_weight;

		// Assign generator scale factor
		gen_scale_factor = *fScaleFactor;

		// Assign interaction type flags
		flagCC = *flagCCINC;
		flagNC = *flagNCINC;
		NEUT_int_type = *Mode;

		// Initialize vectors to be filled with info on FS particles
		std::vector<std::vector<float>> protons;
		std::vector<std::vector<float>> antiprotons;
		std::vector<std::vector<float>> neutrons;
		std::vector<std::vector<float>> antineutrons;
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
		int n_antiprotons = 0;
		int n_neutrons = 0;
		int n_antineutrons = 0;
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
			// Grab PDG and kinematic data, and organize into a 4-vector
			int pdg_part = pdg[j];
			float e_part = E[j];
			float px_part = px[j];
			float py_part = py[j];
			float pz_part = pz[j];
			std::vector<float> p4 {e_part, px_part, py_part, pz_part};

			// Use PDG to assign 4-vector to appropriate particle vector
			if((pdg_part) == 2212 || (pdg_part == 1000010010)) {
				protons.push_back(p4); n_protons++;
			}
			else if(pdg_part == -2212) {
				antiprotons.push_back(p4); n_antiprotons++;
			}
			else if(pdg_part == 2112) {neutrons.push_back(p4); n_neutrons++;}
			else if(pdg_part == -2112) {
				antineutrons.push_back(p4); n_antineutrons++;}
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
			else {
				float pdg_part_float = pdg_part * 1.0;
				std::vector<float> p5 {e_part, px_part, py_part, pz_part, \
					pdg_part_float};
				others.push_back(p5); n_others++;
			}
		}

		// Sort the vectors of vectors by energy
		std::vector<std::vector<float>> sorted_protons = \
			sort_by_energy(protons);
		std::vector<std::vector<float>> sorted_antiprotons = \
			sort_by_energy(antiprotons);
		std::vector<std::vector<float>> sorted_neutrons = \
			sort_by_energy(neutrons);
		std::vector<std::vector<float>> sorted_antineutrons = \
			sort_by_energy(antineutrons);
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
		fs_antiprotons = sorted_antiprotons;
		fs_neutrons = sorted_neutrons;
		fs_antineutrons = sorted_antineutrons;
		fs_gammas = sorted_gammas;
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
		nfsantiprotons = n_antiprotons;
		nfsneutrons = n_neutrons;
		nfsantineutrons = n_antineutrons;
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

	// Close the old file so it doesn't remain open (and, god forbid, 
	// writeable!) if we stay in ROOT
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
