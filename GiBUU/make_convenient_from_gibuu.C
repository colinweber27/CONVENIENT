// Author: Kevin Vockerodt (k.j.vockerodt@qmul.co.uk), based strongly on
// CONVENIENT code by Colin Weber
// Date: 11 January  2024
// Purpose: To read in GiBUU FinalEvent.dat files and save in CONVENIENT 
// format.

// Includes
// ROOT includes
#include "TCanvas.h"
#include "TTree.h"
#include "TFile.h" 
#include "TH1.h"
#include "TPRegexp.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TTreeReaderArray.h"

// C++ includes
#include <fstream>
#include <iostream>
#include <string>
#include <vector>


// This function will take as input a vector of a vector of floats, and 
// output the same vector, but sorted.
std::vector<std::vector<float>> sort_by_energy(
	std::vector<std::vector<float>> four_momenta){
	// Sort the vector of vectors in place using the sort method 
	// Format is (first object, last object, compare function)
  	std::sort(
		// Iterate over all four-momenta
    	four_momenta.begin(), four_momenta.end(), 
    	[](const std::vector<float>& a, const std::vector<float>& b) {
		// Compare the four-momenta by energy, the 0th entry.
      		return (a[0] > b[0]); 
    	}
  	);
  // Return the sorted vector
  return four_momenta; 
}


// Create a structure to hold all the information outputted by GiBUU
struct FSparticle{
	int run;
  	int event;
  	int pdg;
  	int charge;
  	double weight; 
		// This is the particle 'perturbative weight'
		// NB: GiBUU sets this to 0 for the struck nucleon
  	std::vector<float> position;		// (x, y, z)
  	std::vector<float> fourmomentum;	// (E, px, py, pz)
  	int history;
  	int production_ID;
		// GiBUU's version of interaction mode
  	float Enu;
};


// Define an 'event' type to store all FS particles in an event
typedef std::vector<FSparticle> event;


// Define a vector of events to store all the information before populating 
// TTrees
std::vector<event> AllEvents;


// Define a function to map GiBUU's particle ID to a standard PDG value
int mapPIDtoPDG(int PID, int charge){
		// (anti)muon:
	if(PID == 902) return -13 * charge;
		// nucleon: (anti)proton or neutron
  	if(PID == 1) return (charge == 0) ? 2112 : 2212 * charge;
		// pion
  	if(PID == 101) return (charge == 0) ? 111 : 211 * charge;	  
		// nuebar
	if(PID == -911) return -12;
  		// numubar
	if(PID == -912) return -14;
  		//nutaubar
	if(PID == -913) return -16;
		// nue
  	if(PID == 911) return 12;
		//numu
  	if(PID == 912) return 14;
		//nutau
  	if(PID == 913) return 16;
		// gamma
  	if(PID == 999) return 22;
		// electron / positron
  	if(PID == 901) return -11 * charge;
		// K+, K0
  	if(PID == 110) return (charge == 1)  ?  321 :  311;
		// K-, K0bar
  	if(PID == 111) return (charge == -1) ? -321 : -311;
		// Lambda
  	if(PID == 32) return 3122;
		// Sigma
 	if(PID == 33){	
    	if(charge == 0) return 3212;
    	else return (charge == 1) ? 3222 : 3112;
  	}
		// Tau
  	if(PID == 903) return -15 * charge;
		// If no match, return 0 for 'other' particle
  	return 0;
}


// Define a function to map target Z and target A to the target PDG
int make_target_pdg(int z, int a) {
	// PDG format is 10LZZZAAAI, where L is the number of strange quarks and 
	// I is the isomer number. The number of protons, Z, and the number of 
	// nucleons, A, must be expressed as three digit numbers with leading 
	// 0's as necessary. We assume that all nuclei are in their ground state 
	// with all strange quarks decayed, such that L = I = 0.
	
	// Initialize the PDG code to its minimum value
	int pdg = 1000000000;

	// Add the Z values
	pdg += (z * 10000);

	// Add the A values
	pdg += (a * 10);

	return pdg;
}


// Calculate the four-momentum squared
float four_momentum_sq(std::vector<float> v) {
	return pow(v[0], 2) - pow(v[1], 2) - pow(v[2], 2) - pow(v[3], 2);
}


void make_convenient_from_gibuu(
	TString infile="", TString fluxfile="", TString pdg_str='0', 
	TString CC_NC="CC", TString target_Z_str='0', TString target_A_str='0', 
	TString outfile="", TString target_fraction_str="1.0") {

	// Parse input arguments
	int pdg = pdg_str.Atoi();
	bool isCC;
	if (CC_NC == "CC") {
		isCC = true;
	}
	else {
		isCC = false;
	}
	int target_Z = target_Z_str.Atoi();
	int target_A = target_A_str.Atoi();
	double target_fraction = target_fraction_str.Atof();
  	
	// Read the input file
	std::fstream fIn;
  	fIn.open(infile, std::ios::in);
  	if(fIn.is_open()){
    	std::string line;
		// First line is just the column headings so we can ignore it
    	getline(fIn, line);	

    	int current_event;
    	bool first_step = true;
    	event event_buffer = {};
    	while(getline(fIn, line)){
       		TStringToken splitstring(line, " ");
       		std::vector<std::string> value = {};
       		while(!splitstring.AtEnd()){
         		splitstring.NextToken();
         		value.push_back((std::string)splitstring);
       		}
       		int run_number   = std::stoi(value[0]);
       		int event_number = std::stoi(value[1]);
       		int charge       = std::stoi(value[3]);
			// Convert GiBUU Particle ID to a PDG value
       		int pdg          				= mapPIDtoPDG(
				std::stoi(value[2]), charge);
			double weight  					= std::stod(value[4]);
       		std::vector<float> position     = {
				std::stof(value[5]), std::stof(value[6]), 
				std::stof(value[7])};
       		std::vector<float> fourmomentum = {
				std::stof(value[8]), std::stof(value[9]), 
				std::stof(value[10]), std::stof(value[11])};

       		int history       				= std::stoi(value[12]);
			// GiBUU's version of interaction mode
       		int production_ID 				= std::stoi(value[13]);
			float Enu        				= std::stof(value[14]);
 
       		if(first_step){
         		current_event = event_number;
         		first_step = false;
       		}

       		// Store FS particle information in a vector
       		FSparticle particle = {
				run_number, event_number, pdg, charge, weight, position, 
				fourmomentum, history, production_ID, Enu};
       		// Check whether we're still reading the same event.  
       		// If not, push back all FS particles so far into one event and 
       		// clear buffer
       		if((particle.event != current_event)){
         		AllEvents.push_back(event_buffer);
         		current_event = particle.event;
         		event_buffer.clear();
       		}
      		event_buffer.push_back(particle);
		}
		// Save the event with all its FS particles
		AllEvents.push_back(event_buffer);

    	std::cout << Form("\nSaved %zu events\n", AllEvents.size());
		// Event number we want to look at
		int e = AllEvents.size() - 1;
    	std::cout << "There are a total of " << e+1 << " events.";
    	std::cout << Form("\nFinal Event (number %d):\n\n", e+1);
    	for(int i = 0; i < AllEvents[e].size(); i++){
      		std::cout << "Run " 		<< AllEvents[e][i].run 		<< '\n';
      		std::cout << "Event " 		<< AllEvents[e][i].event 	<< '\n';
      		std::cout << "PDG " 		<< AllEvents[e][i].pdg 		<< '\n';
      		std::cout << "Charge "		<< AllEvents[e][i].charge 	<< '\n';
      		std::cout << "Weight " 		<< AllEvents[e][i].weight 	<< '\n';
      		std::cout << "Position: (" 	<< AllEvents[e][i].position[0] << 
				", " << AllEvents[e][i].position[1] << 
				", " << AllEvents[e][i].position[2] << ")\n";
      		std::cout << "4-Momentum: (" << AllEvents[e][i].fourmomentum[0] 
				<< ", " << AllEvents[e][i].fourmomentum[1] << 
				", " << AllEvents[e][i].fourmomentum[2] << 
				", " << AllEvents[e][i].fourmomentum[3] << ")\n";
      		std::cout << "History " 	<< AllEvents[e][i].history 	<< '\n';
      		std::cout << "Production ID " 
				<< AllEvents[e][i].production_ID << '\n';
      		std::cout << "Enu " 		<< AllEvents[e][i].Enu 	<< "\n\n";
    	}
  	}
  	else std::cout << "File " << infile << " does not exist!  Aborting.'\n";

  	// Define variables to store in TTree
  	int nfsp;

  	TFile* fOut = TFile::Open(outfile,"RECREATE");

  	// Read from flux text file (with energy bins of equal widths), make 
  	// histogram and save

  	TH1D* hFlux;

  	std::fstream fFlux_data;
  	fFlux_data.open(fluxfile, std::ios::in);
  	if(fFlux_data.is_open()){
    	std::string line;
    	std::vector<float> vecEnergy;
    	std::vector<float> vecFlux;
    	while(getline(fFlux_data, line)){
      		TStringToken splitstring(line, "\t");
      		splitstring.NextToken();
      		std::string energy = (std::string)splitstring;
      		vecEnergy.push_back(std::stof(energy));

      		splitstring.NextToken();
      		std::string flux = (std::string)splitstring;
      		vecFlux.push_back(std::stod(flux));
    	}
    	int nbins = vecEnergy.size();

    	hFlux = new TH1D("FlatTree_FLUX", "FlatTree_FLUX", nbins, 0, 
			vecEnergy[nbins-1]);

    	for(int bin = 1; bin <= nbins; bin++){
    		hFlux->SetBinContent(bin, vecFlux[bin-1]);
    	}

    	hFlux->GetXaxis()->SetRangeUser(0, vecEnergy[0] + vecEnergy[nbins-1]);
		TCanvas* cFlux = new TCanvas();

    	hFlux->Draw("hist");
    	hFlux->Write("FlatTree_FLUX");

    	// cFlux->Print("flux_from_dat_file.png");

    	// cFlux->Close();
	}
  	else std::cout << "Cannot open flux file " << fluxfile << '\n';

	// Get the target nucleus
	int TargetNucleus = make_target_pdg(target_Z, target_A);

  	TTree* tree = new TTree("generator_data", "generator_data");

  	int mode;
  	tree->Branch("Mode", &mode);

  	float E_nu;
	int PDG_nu;
  	tree->Branch("Enu", &E_nu);
	tree->Granch("PDGnu", &PDG_nu);

  	// Initialize the object that will hold the PDG code of the hit nucleus, 
  	// and point a branch to it.
  	int target_nucleus;
  	tree->Branch("target_PDG", &target_nucleus);

  	double event_weight;
  	tree->Branch("EventWeight", &event_weight);

  	double gen_scale_factor;
  	tree->Branch("GenScaleFactor", &gen_scale_factor);

  	bool flagCC, flagNC;
  	tree->Branch("flagCC", &flagCC);
  	tree->Branch("flagNC", &flagNC);

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

 	tree->Branch("FS_Protons",     	&fs_protons);
	tree->Branch("FS_Antiprotons", 	&fs_antiprotons);
  	tree->Branch("FS_Neutrons",    	&fs_neutrons);
	tree->Branch("FS_Antineutrons", &fs_antineutrons);
  	tree->Branch("FS_Gammas",      	&fs_gammas);
  	tree->Branch("FS_Pi0s",        	&fs_pi0s);
  	tree->Branch("FS_PiPs",        	&fs_pips);
  	tree->Branch("FS_PiMs",        	&fs_pims);
  	tree->Branch("FS_Muons",       	&fs_muons);
  	tree->Branch("FS_Electrons",   	&fs_electrons);
  	tree->Branch("FS_Taus",        	&fs_taus);
  	tree->Branch("FS_Nus",         	&fs_nus);
	tree->Branch("FS_Others",      	&fs_others);

  	// Initialise integers to count the numbers of particles in each event, 
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

  	tree->Branch("n_FS_Protons",     	&nfsprotons);
	tree->Branch("n_FS_Antiprotons", 	&nfsantiprotons);
  	tree->Branch("n_FS_Neutrons",    	&nfsneutrons);
  	tree->Branch("n_FS_Antineutrons", 	&nfsantineutrons);
	tree->Branch("n_FS_Gammas",      	&nfsgammas);
  	tree->Branch("n_FS_Pi0s",        	&nfspi0s);
  	tree->Branch("n_FS_PiPs",        	&nfspips);
  	tree->Branch("n_FS_PiMs",        	&nfspims);
  	tree->Branch("n_FS_Muons",       	&nfsmuons);
  	tree->Branch("n_FS_Electrons",   	&nfselectrons);
  	tree->Branch("n_FS_Taus",        	&nfstaus);
  	tree->Branch("n_FS_Nus",         	&nfsnus);
	tree->Branch("n_FS_Others",      	&nfsothers);

  	const int nEvents = AllEvents.size();

  	const double flux_integral = hFlux->Integral();
  	std::cout << "Flux: " << flux_integral << '\n'; 

  	gen_scale_factor = 1e-38;
  	std::cout << "Generator Scale Factor: " << gen_scale_factor << '\n';

	// Loop over all events
  	for(int event = 0; event < nEvents; event++){
    	PDG_nu = pdg;
    	E_nu = AllEvents[event][0].Enu;

    	// Store the Interaction Mode
    	mode = AllEvents[event][0].production_ID;

		// Store the target nucleus
		target_nucleus = TargetNucleus;

    	// Calculate the total event weight
    	event_weight = target_fraction * (double)AllEvents[event][0].weight / (AllEvents[nEvents-1][0].run);

    	if(isCC){
      		flagCC = true;
      		flagNC = false;
    	}
    	else{
      		flagCC = false;
      		flagNC = true;
    	}

    	// Initialise vectors to be filled with info on FS particles
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
    	for (int j = 0; j < AllEvents[event].size(); j++){
			// Ignore a particle with weight 0 (i.e. the struck nucleon, 
			// which is always the 2nd 'FS' particle store)
      		if(AllEvents[event][j].weight == 0.) continue;
      		// Grab PDG and kinematic data, and organize into a 4-vector
      		int pdg_part = AllEvents[event][j].pdg;
      		std::vector<float> p4 = AllEvents[event][j].fourmomentum;

      		// Use PDG to assign 4-vector to appropriate particle vector
      		if(pdg_part == 2212) {
				// Check to make sure the proton is not bound. If it is, it 
				// will have p^2 < 0.938^2 GeV
				if (four_momentum_sq(p4) < pow(0.938, 2)) continue;
				else { 
					protons.push_back(p4); n_protons++;
				}
			}
			else if (pdg_part == -2212) {
				if (four_momentum_sq(p4) < pow(0.938, 2)) continue;
				else {
					antiprotons.push_back(p4); n_antiprotons++;
				}
			}
      		else if(pdg_part == 2112) {
				if (four_momentum_sq(p4) < pow(0.938, 2)) continue;
				else {
					neutrons.push_back(p4); n_neutrons++;}
      			}
			else if(pdg_part == -2112) {
				if (four_momentum_sq(p4) < pow(0.938, 2)) continue;
				else {
					antineutrons.push_back(p4); n_antineutrons++;
				}
			}
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
				std::vector<float> p5 {p4[0], p4[1], p4[2], p4[3], \
					pdg_part_float};
				others.push_back(p5); n_others++;
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
      		fs_protons     = sorted_protons;
			fs_antiprotons = sorted_antiprotons;
      		fs_neutrons    = sorted_neutrons;
			fs_antineutrons = sorted_antineutrons;
      		fs_gammas      = sorted_gammas;
      		fs_pi0s        = sorted_pi0s;
      		fs_pips        = sorted_piPs;
      		fs_pims        = sorted_piMs;
      		fs_muons       = sorted_muons;
      		fs_electrons   = sorted_electrons;
      		fs_taus        = sorted_taus;
      		fs_nus         = sorted_nus;
		    fs_others      = sorted_others;

      		// Assign particle counters to the appropriate leaves
      		nfsprotons     = n_protons;
			nfsantiprotons = n_antiprotons;
      		nfsneutrons    = n_neutrons;
			nfsantineutrons = n_antineutrons;
      		nfsgammas      = n_gammas;
      		nfspi0s        = n_pi0s;
      		nfspips        = n_piPs;
      		nfspims        = n_piMs;
      		nfsmuons       = n_muons;
      		nfselectrons   = n_electrons;
      		nfstaus        = n_taus;
      		nfsnus         = n_nus;
      		nfsothers      = n_others;
    	}
    	// Fill the tree
    	tree->Fill();
  	}

  	// After looping through all events, write the tree to the new file
  	fOut->Write();


  	fOut->Close();
  	fFlux_data.close();

  	fIn.close();
}
