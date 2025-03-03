// Author: Colin Weber (webe1077@umn.edu)
// Date: 8 September 2023
// Updated: 20 August 2024
// Purpose: To demonstrate how to compare cross sections from data and 
// different generators using CONVENIENT. The worked examples are for a 
// single-differential cross section in pion angle, a single-differential 
// cross section in Enu, a double-differential cross section in pion angle 
// and Enu, and a triple-differential cross section in Eavail, pion angle, 
// and neutrino energy. We arbitrarily assume a pion 3-momentum threshold of 
// 0.2 GeV for detection by the detector. These variables and cuts were 
// chosen to fully illustrate the use of CONVENIENT, rather than for their 
// physics utility.
//
// Command: cafe xsec_analysis_macro.C

// Includes
// ROOT includes
#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "THnSparse.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TTreeReaderArray.h"
#include "TString.h"
#include "TParticlePDG.h"
#include "TDatabasePDG.h"
#include "TVector3.h"
#include "TColor.h"

// C++ includes
#include <cmath>
#include <math.h>
#include <vector>
#include <iostream>
#include <tuple>

// Define all global constants.
const float MUON_MASS = TDatabasePDG::Instance()->GetParticle(13)->Mass(); // GeV
const float PIP_MASS = TDatabasePDG::Instance()->GetParticle(211)->Mass(); // GeV
const float PROTON_MASS = TDatabasePDG::Instance()->GetParticle(2212)->Mass(); // GeV
const float PIP_THRESHOLD_P = 0.2; // GeV
const float PIP_THRESHOLD_E = sqrt(pow(pip_threshold_p, 2) + \
	pow(PIP_MASS, 2)); // GeV

// Define colors to use for plotting. For a list of valid colors, see 
// https://root.cern.ch/doc/master/classTColor.html.
const std::vector<int> myColors = {kOrange+7, kRed, kMagenta, kViolet-7, \
	kSpring-7, kPink-9, kRed+4, kBlue, kCyan};

// Define phase space cuts. This can be done here or in a separate header 
// file. If the cuts are used by many macros, then it's best to have them 
// elsewhere.
bool phaseSpaceCut(float pi_E){
	if(pi_E > PIP_THRESHOLD_E) return true;
	else return false;
}

// Define other functions to use.
// As with phase space cuts, if these are to be used by many other macros, 
// it is better to define them in a header file.
float calcTheta(std::vector<float> particle) {
	TVector3 momentum(particle[1], particle[2], particle[3]);
	return momentum.Theta();
} 

float calcKE(std::vector<float> particle, float mass) {
	return particle[0] - mass;
}

float calcMomentum(std::vector<float> particle) {
	TVector3 momentum(particle[1], particle[2], particle[3]);
	return momentum.Mag();
}

float calcMass(std::vector<float> particle) {
	float mass = sqrt(pow(particle[0], 2) - pow(calcMomentum(particle), 2));
	return mass;
}

void print_generator_unknown_string() {
	std::cout << "Generator unknown." << std::endl;
	std::cout << "Generator strings should be one of" << std::endl;
	std::cout << "'GENIE', 'NuWro', 'NEUT', or 'GiBUU'" << std::endl;
}

// Function to calculate the number of files from each generator, and hence 
// the factor to scale each cross section by to account for combining 
// several Convenient files into one cross section.
std::vector<std::tuple<std::string, TFile*, double>> set_file_list_scales(std::vector<std::tuple<std::string, TFile*, double>> v) {
	int n_entries = v.size();
	double n_genie = 0.0;
	double n_nuwro = 0.0;
	double n_neut = 0.0;
	double n_gibuu = 0.0;
	for(int i = 0; i < n_entries; i++) {
		std::string gen = std::get<0>(v[i]);
		if(gen == "GENIE") {
			n_genie += 1.0;
		}
		else if(gen == "NuWro") {
			n_nuwro += 1.0;
		}
		else if(gen == "NEUT") {
			n_neut += 1.0;
		}
		else if(gen == "GiBUU") {
			n_gibuu += 1.0;
		}
		else {
			print_generator_unknown_string();
		}
	}
	double genie_scale = 1.0 / n_genie;
	double nuwro_scale = 1.0 / n_nuwro;
	double neut_scale = 1.0 / n_neut;
	double gibuu_scale = 1.0 / n_gibuu;
	for(int i = 0; i < n_entries; i++) {
		std::string gen = std::get<0>(v[i]);
		if(gen == "GENIE") {
			std::get<2>(v[i]) = genie_scale;
		}
		else if(gen == "NuWro") {
			std::get<2>(v[i]) = nuwro_scale;
		}
		else if(gen == "NEUT") {
			std::get<2>(v[i]) = neut_scale;
		}
		else if(gen == "GiBUU") {
			std::get<2>(v[i]) = gibuu_scale;
		}
		else {
			print_generator_unknown_string();
		}
	}
	return v;
}

// addtoEavail is used to construct Eavail from the final state particle' 
// 4-momenta. It follows the NOvA method implemented in 
// CAFAna/Vars/TruthVars.cxx
float addtoEavail(std::vector<float> particle, int PID) {
	if (PID == 2212) {return calcKE(particle, PROTON_MASS);}
	else if (abs(PID) == 211) {return calcKE(particle, PIP_MASS);}
	else if ((PID == 111) || (PID == 11) || (PID == 22)) {
		return particle[0];
	}
	else if (PID >= 2000000000) {return 0.0;}
	else if (PID >= 1000000000) {return 0.0;}
	else if (PID >= 2000 && PID != 2212 && PID != 2112) {
		return particle[0] - PROTON_MASS;
	}
	else if (PID <= -2000) {
		return particle[0] + PROTON_MASS;
	}
	else if (PID != 2112 && (abs(PID) < 11 || abs(PID) > 16)) {
		return particle[0];
	}
	else return 0.0;
}

void xsec_analysis_macro() {
	// Read in data
	// This section is analysis-specific, so it is not written here. 
	// Individual analyses will have to write this section as
	// needed.
	
	// Read in files for the generators and models we want to compare
	// The file ConvenientOutputs_NOvA.txt lists the Convenient outputs 
	// available for use, organized by generator configuration, flux, and 
	// neutrino flavor. These outputs are located in 
	// /exp/nova/data/users/colweber/ConvenientOutputs_NOvA
	TFile* genie_file_1 = TFile::Open("GENIE:N18_10j_02_11a.2.FHC.numu.convenient_output.root");
	TFile* genie_file_2 = TFile::Open("GENIE:N18_10j_02_11a.3.FHC.numu.convenient_output.root");
	TFile* nuwro_file = TFile::Open("NuWro:Defaultparams.txt.2.FHC.numu.convenient_output.root");

	// Modify this section to organize the data files into a vector of 
	// tuples. The first element of each tuple is the file alias, and should 
	// be one of:
	// 		'GENIE'
	// 		'NuWro'
	// 		'NEUT'
	// 		'GiBUU'
	// The second element is the file itself, and the third element is (at 
	// first) a null double that will be set as 1 / the number of files of 
	// that generator that are being analyzed. The scaling is handled by the 
	// function 'set_file_list_scales'.
	double genie_scale;
	double nuwro_scale;
	std::vector<std::tuple<const std::string, TFile*, double>> file_list_unscaled_genie = {
		{"GENIE",	genie_file_1, genie_scale},
		{"GENIE", 	genie_file_2, genie_scale}
	}
	std::vector<std::tuple<const std::string, TFile*, double>> file_list_unscaled_nuwro = {
		{"NuWro",	nuwro_file, nuwro_file}	
	}
	std::vector<std::tuple<const std::string, TFile*, double>> file_list_genie = set_file_list_scales(file_list_unscaled_genie);
	std::vector<std::tuple<const std::string, TFile*, double>> file_list_nuwro = set_file_list_scales(file_list_unscaled_nuwro);
	// Finally, create a vector of these vectors
	std::vector<std::vector<std::tuple<const std::string, TFile*, double>>> file_list = {file_list_genie, file_list_nuwro};	

	// The following line does not need to be changed.
	int n_gens = sizeof(file_list) / sizeof(file_list[0]);

	// This section does need to be changed
	// Initialize a vector to push the generated xsec histograms to.
	// Since all ROOT histogram classes are derived from TH1, we initialize 
	// the vector to hold TH1*.
	// We also set the Add Directory option to be false so that each new 
	// pointer points to a unique histogram.
	TH1::AddDirectory(kFALSE);
	std::vector<std::tuple<TH1D*, std::string>> 1D_PiTheta_histo_vec;
	std::vector<std::tuple<TH1D*, std::string>> 1D_Enu_histo_vec;
	std::vector<std::tuple<TH2D*, std::string>> 2D_histo_vec;
	std::vector<std::tuple<TH3D*, std::string>> 3D_histo_vec;

	// This section, Set Binning, is very analysis-dependent and may have 
	// to be heavily modified.
	//
	// Set binning
	//
	// Binning in general is totally up to individual analyses for a 
	// flux-averaged cross section calculation. The binning of the data 
	// should be optimized, and then the binning of the MC should match the 
	// binning of the data. Often the binning variables will be imported 
	// from another file. 
	//
	// When initializing histograms, the format is (name, title, x n_bins, 
	// x lower limit, x upper limit, y n_bins, y lower limit, y upper limit) 
	// for constant binning, or (name, title, x n_bins, array of bin edges, 
	// y n_bins, array of bin edges) for variable binning. What we define 
	// here are the arrays of bin edges and the number of bins.
	//
	// This example demonstrates how to set the data binning for a triple-
	// differential analysis in which we want variable binning and the bin 
	// edges are known from a previous analysis.
	std::vector<float> PiTheta_binning = {-1.0, 0.5, 0.74, 0.80, 0.85, \
		0.88, 0.91, 0.94, 0.96, 0.98, 0.99, 1.0};
	std::vector<double> Enu_binning = {-10.0, 0.0, 0.50, 0.75, 1.0, 1.25, \
		1.50, 1.75, 2.0, 2.50, 3.0, 4.0, 120.0};
	std::vector<float> Eavail_binning = {0.0, 0.10, 0.30, 0.60, 1.0, 2.0, \
		120.0};
	int PiTheta_nbins = PiTheta_binning.size();
	int Enu_nbins = Enu_binning.size();
	int Eavail_nbins = Eavail_binning.size();
	// It is also often useful to have the ranges we will plot within, since 
	// we will often not plot the overflow bins.
	float PiTheta_range[2] = {0.5, 1.0};
	double Enu_range[2] = {0.0, 4.0};
	float Eavail_range[2] = {0.0, 2.0};

	// Loop over the MC files and fill the histograms.
	for(int gen_index = 0; gen_index < n_gen; gen_index++) {
		// Get the vector of files for a generator
		std::vector<std::tuple<std::string, TFile*, double>> gen_file_list = file_list[gen_index];
		// Get the generator
		std::string alias = std::get<0>(gen_file_list[0]);
		// Get the number of files for that generator
		int n_files = gen_file_list.size();
		// Initialize objects to hold the file and histograms from each file
		TFile* files[n_files];
		TH1D* h1_PiTheta_tmp[n_files];
		TH1D* h1_Enu_tmp[n_files];
		TH2D* h2_tmp[n_files];
		TH3D* h3_tmp[n_files];

		// Initialize each histogram in the array
		for(int i = 0; i < n_files; i++) {
			h1_PiTheta_tmp[i] = new TH1D(\
				Form("h1_%s_PiTheta_%i", alias.c_str(), i), \
				Form("%s_PiTheta_%i", alias.c_str(), i), PiTheta_nbins, \
				&PiTheta_binning[0]);
			h1_Enu_tmp[i] = new TH1D(\
				Form("h1_%s_Enu_%i", alias.c_str(), i), \
				Form("%s_Enu_%i", alias.c_str(), i), Enu_nbins, \
				&Enu_binning[0]);
			h2_tmp[i] = new TH2D(\
				Form("h2_%s_%i", alias.c_str(), i), \
				Form("h2_%s_%i", alias.c_str(), i), Enu_nbins, \
				&Enu_binning[0], PiTheta_nbins, &PiTheta_binning[0]);
			h3_tmp[i] = new TH3D(\
				Form("h3_%s_%i", alias.c_str(), i), \
				Form("h3_%s_%i", alias.c_str(), i), Enu_nbins, \
				&Enu_binning[0], PiTheta_nbins, &PiTheta_binning[0], \
				Eavail_nbins, &Eavail_binning[0]);
		}
		
	// Loop over the files in the generator file list
	for(int file_index = 0; file_index < n_files; file_index++) {
		// Get the file and file alias. Don't change this.
		files[file_index] = std::get<1>(gen_file_list[file_index]);

		// Declare a TTreeReader to help read the tree. We get the file from 
		// the pair using the .second function. Don't change this.
		TTreeReader reader("generator_data", file);

		// Declare TTreeReader arrays and values to hold the desired data 
		// from the trees. Format is (reader, leaf). This will have to be 
		// altered depending on what data is desired.
			// Grab info on CC/NC and neutrino type
			TTreeReaderValue<bool> flagCC(reader, "flagCC");
			TTreeReaderValue<int> nu_in_flavor(reader, "NuIn.Flavor");
			TTreeReaderValue<float> nu_in_e(reader, "NuIn.E_in");

			// Grab leaves for final state particles that make up our 
			// topology of interest. It is from these values that we can 
			// extract final state particle kinematics.
			TTreeReaderArray<std::vector<float>> antiprotons(\
				reader, "FS_Antiprotons");
			TTreeReaderArray<std::vector<float>> protons(\
				reader, "FS_Protons");
			TTreeReaderArray<std::vector<float>> neutrons(\
				reader, "FS_Neutrons");
			TTreeReaderArray<std::vector<float>> gammas(\
				reader, "FS_Gammas");
			TTreeReaderArray<std::vector<float>> pi0s(\
				reader, "FS_Pi0s");
			TTreeReaderArray<std::vector<float>> pips(\
				reader, "FS_PiPs");
			TTreeReaderArray<std::vector<float>> pims(\
				reader, "FS_PiMs");
			TTreeReaderArray<std::vector<float>> electrons(\
				reader, "FS_Electrons");
			TTreeReaderArray<std::vector<float>> others(reader, "FS_Others");

			TTreeReaderValue<int> n_antiprotons(reader, "n_FS_Antiprotons");
			TTreeReaderValue<int> n_protons(reader, "n_FS_Protons");
			TTreeReaderValue<int> n_neutrons(reader, \
				"n_FS_Neutrons");
			TTreeReaderValue<int> n_gammas(reader, "n_FS_Gammas");
			TTreeReaderValue<int> n_pi0s(reader, "n_FS_Pi0s");
			TTreeReaderValue<int> n_pips(reader, "n_FS_PiPs");
			TTreeReaderValue<int> n_pims(reader, "n_FS_PiMs");
			TTreeReaderValue<int> n_electrons(reader, "n_FS_Electrons");
			TTreeReaderValue<int> n_others(reader, "n_FS_Others");	
		
			// Grab leaves holding the generator scale factor and 
			// event weight. The generator scale factor 
			// partially converts from an event histogram to a cross 
			// section, and is the same for all events in a given run for a 
			// given generator. For GENIE, NuWro, and NEUT, the generator 
			// scale factor is equivalent to NUISANCE's fScaleFactor. More 
			// details on fScaleFactor ("W") can be found in the NUISANCE 
			// arxiv paper at arXiv:1612.07393 [hep-ex]. For GiBUU, the 
			// generator scale factor is 1*10^{-38}. The event weight is 
			// calculated such that an event weight * generator scale factor 
			// applied to each event gives a correctly normalized cross 
			// section up to accounting for relative elemental abundances. 
			// The event weight can be unique for each event. Relative 
			// elemental abundances are accounted for when filling the 
			// histogram. 
			TTreeReaderValue<double> genscalefactor(reader, \
				"GenScaleFactor");
			TTreeReaderValue<double> event_weight(reader, \
				"EventWeight");

			// Convenient keeps info on the underlying interaction type. For 
			// GENIE, NuWro, and NEUT, this info is copied from NUISANCE and 
			// stored in the branch "NEUT_int_type". For GiBUU, we keep the 
			// original GiBUU code and store it in the branch "Mode". The 
			// GiBUU code dictionary is at 
			// https://gibuu.hepforge.org/trac/wiki/LesHouches. 
			std::string int_type_tree_name;
			if(alias.substr(0, 5) == "GiBUU") {
				int_type_tree_name = "Mode"; 
			}
			else {
				int_type_tree_name = "NEUT_int_type";
			}
			TTreeReaderValue<int> int_type(reader, \
				int_type_tree_name.c_str());	

		// The rest of this macro is broken down into a section for single-, 
		// double-, and triple-differential cross section calculations. 
		// Simply uncomment the section that corresponds to the type of 
		// analysis you're doing, and modify lines only within that section. 
		// Within each section, the big idea is the same: loop over the data 
		// and MC files and calculate a cross section. The only things that 
		// will need to be changed are the lines governing the filling of 
		// histograms, because these are where the different differential 
		// variabels are calculated. Plotting of the cross sections is 
		// handled after creating all histograms, and example plotting code 
		// is written to handle single-differential cross sections. Note 
		// that the steps of multiplying by the integrated flux and then 
		// dividing out the flux on a bin-by-bin basis are only needed if 
		// one of the differential cross section variables is neutrino 
		// energy. 
	
//########################################################################//
	// Flux-averaged cross sections //
//#######################################################################//
		
		// To do flux-averaged cross sections, the general procedure is:
			// 1. For an n-differential cross section, create an n-dim. 
			// histogram of the differential variables. The histogram should 
			// already have been declared in the outermost for-loop that 
			// loops over the different generators.
			// 2. Weight each event by the generator scale factor, event 
			// weight, and 1 / the number of files that are being 
			// combined for each generator to convert to a cross section.
			// The relative elemental abundances in the ND have already 
			// been accounted for in the generator scale factor. If we 
			// stop here, we have the cross section as a 
			// function of the differential variables.
				// i. The scale factor has to be treated differently if one 
				// of the differential cross section variables is incoming 
				// neutrino energy. In this case, the event histogram is 
				// multiplied by the integrated flux, and then the flux is 
				// divided out on a bin-by-bin basis along the axis with 
				// neutrino energy.
			// 3. Divide by the bin widths of each variable to get the 
			// flux-averaged differential cross section


		// Single differential, flux-averaged cross section in pion angle //

		// Assumes we are filling a histogram h1.
		// Grab the scale factor due to combining files
		/* double file_scale_factor = std::get<2>(gen_file_list[file_index]);
		
		// Loop over events and fill the histogram
		while (reader.Next()) {
			// Select neutrino type and CC/NC:
			// Ex: if the neutrino is not numu, or it is not a CC event, 
			// skip the event.
			if((*nu_in_flavor != 14) || (! *flagCC)) continue;
			else {
				// Here's where differential xsec variables are defined 
				// and used to make cuts. Kinematic variables are stored 
				// in each event as vectors of vectors, where each 
				// interior vector is a 4-momentum (E, px, py, pz), and 
				// each particle in the event contributes one of these 
				// to the external vector. The external vector is 
				// ordered by decreasing energy. So, to access the 
				// second most-energetic particle's py, call 
				// particle[1][2]. All other differential xsec variables 
				// can be constructed with the information in the files.
				// In this case:
				// If there is at least one pi+, and the most energetic 
				// one is above the threshold, then calculate the pion angle 
				// and fill the histogram with it, weighted by the generator 
				// scale factor and event weight.
				if(*n_pips > 0) {
					float pip_E = pips[0][0];
					if(phaseSpaceCut(pip_E)) {	
						h1_PiTheta_tmp[file_index]->Fill(calcTheta(pips[0], \
							*genscalefactor * *event_weight * \
							file_scale_factor);
					} // Brace for satisfying phase space cut
				} // Brace for satisfying n pion > 0
			} // Brace for satisfying numuCC
		} // Brace for looping through events
	} // Brace for looping through files
	// Sum up all the histograms in the histo vector
	TH1D* h1_PiTheta = new TH1D(\
		Form("h1_%s_PiTheta", alias.c_str()), \
		Form("%s_PiTheta", alias.c_str()), PiTheta_nbins, \
		&PiTheta_binning[0])
	for(int i = 0; i < n_files; i++) {
		h1_PiTheta->Add(h1_PiTheta_tmp[i]);
	}
	// Divide by bin width
	for(int i = 1; i <= h1_PiTheta->GetXaxis()->GetNbins(); i++) {
		h1_PiTheta->SetBinContent(i, h1_PiTheta->GetBinContent(i) / \
		h1_PiTheta->GetXaxis()->GetBinWidth(i));
	}

	// Append histogram to the vector
	1D_PiTheta_histo_vec.push_back(std::make_tuple(h1_PiTheta, alias));
	} // Brace for generator loop */

		// Single differential, flux-averaged cross section in Enu //
		
		// Assumes we are filling a histogram h1

/*		// Grab the scale factor due to combining files
		double file_scale_factor = std::get<2>(file_list[file_index]);
		
		// Loop over events and fill the histogram
		while (reader.Next()) {
			// Select neutrino type and CC/NC:
			// Ex: if the neutrino is not numu, or it is not a CC event, 
			// skip the event.
			if((*nu_in_flavor != 14) || (! *flagCC)) continue;
			else {
				// Here's where differential xsec variables are defined 
				// and used to make cuts. Kinematic variables are stored 
				// in each event as vectors of vectors, where each 
				// interior vector is a 4-momentum (E, px, py, pz), and 
				// each particle in the event contributes one of these 
				// to the external vector. The external vector is 
				// ordered by decreasing energy. So, to access the 
				// second most-energetic particle's py, call 
				// particle[1][2]. All other differential xsec variables 
				// can be constructed with the information in the files.
				// In this case:
				// If there is at least one pi+, and the most energetic 
				// one is above the threshold, then get the neutrino energy 
				// and fill the histogram with it, weighted by the generator 
				// scale factor and event weight.
				if(*n_pips > 0) {
					float pip_E = pips[0][0];
					if(phaseSpaceCut(pip_E)) {	
						h1_Enu_tmp[file_index]->Fill(*nu_in_E, \
							*genscalefactor * *event_weight * \
							file_scale_factor);
					} // Brace for phase space cut
				} // Brace for n_pi > 0 cut
			} // Brace for numuCC cut
		} // Brace for event loop
	} Brace for file loop within a generator
	// Sum up all the histograms in the histo vector
	TH1D* h1_Enu = new TH1D(\
		("h1" + alias + "_Enu").c_str(), (alias + "_Enu").c_str(), \
		Enu_nbins, &Enu_binning[0]);
	for(int i = 0; i < n_files; i++) {
		h1_Enu->Add(h1_Enu_tmp[i]);
	}
	// Divide out flux on a bin-by-bin basis
	// 1. Extract flux from the CONVENIENT output (the "generator 
	// flux"), rename, and get nbins
	TH1D* gen_flux = (TH1D*) files[0]->Get("FlatTree_FLUX");
	gen_flux->SetName(Form("generator_flux_%s", alias.c_str()));
	int gen_flux_nbins = gen_flux->GetNbinsX();

	// 2. Calculate the integrated generator flux, and scale the 
	// histogram by the integrated generator flux. 
	double gen_flux_int = gen_flux->Integral();
	h1_Enu->Scale(gen_flux_int);

	// 3. Get the "signal flux". This is the generator flux rebinned 
	// according to the Enu binning.
	TH1D* sig_flux = new TH1D("sig_flux_" + alias, "sig_flux", \
		Enu_nbins, &Enu_binning[0]);
	for(int i = 1; i <= gen_flux_nbins; i++) {
		sig_flux->Fill(gen_flux->GetBinCenter(i), \
			gen_flux->GetBinContent(i));
	}
		
	// 4. Divide out flux on a bin-by-bin basis
	h1_Enu->Divide(sig_flux);

	// 5. For single differential cross sections in Enu, we also 
	// typically divide by the neutrino energy
	for(int i = 1; i <= h1_Enu->GetXaxis()->GetNbins(); i++) {
		h1_Enu->SetBinContent(i, h1_Enu->GetBinContent(i) / \
			h1_Enu->GetXaxis()->GetBinCenter(i));
	}

	// Append histogram to the vector
	1D_Enu_histo_vec.push_back(std::make_tuple(h1_Enu, alias));
	} // Move on to the next generator */
		
		// Double-differential, flux-averaged cross sections //
	
 		// Assumes we have declared a histogram h2
/*		// Grab the scale factor due to combining files
		double file_scale_factor = std::get<2>(file_list[file_index]);

		// Loop over events and fill histograms
		while(reader.Next()) {
			// Select neutrino type and CC/NC
			// Ex: If the neutrino is not numu, or it is not a CC event, 
			// skip the event.
			if((*nu_in_flavor != 14) || (! *flagCC)) continue;
			else {
				// Here's where differential xsec variables are defined 
				// and used to make cuts. Kinematic variables are stored 
				// in each event as vectors of vectors, where each 
				// interior vector is a 4-momentum (E, px, py, pz), and 
				// each particle in the event contributes one of these 
				// to the external vector. The external vector is 
				// ordered by decreasing energy. So, to access the 
				// second most-energetic particle's py, call 
				// particle[1][2]. All other differential xsec variables 
				// can be constructed with the information in the files.
				// In this case:
				// If there is at least one pi+, and the most energetic 
				// one is above the threshold, then fill the histogram with 
				// the neutrino energy and pion angle, weighted by the 
				// generator scale factor and event weight.
				if(*n_pips > 0) {
					float pip_E = pips[0][0];
					if(phaseSpaceCut(pip_E)) {
						float PiTheta = calcTheta(pips[0]);		
						h2_tmp[file_index]->Fill(*nu_in_E, PiTheta, \
							*genscalefactor * *event_weight * \
							file_scale_factor);
					} // Brace for phase space cut
				} // Brace for n_pi > 0
			} // Brace for numuCC
		} // Brace for event loop
	} Brace for looping over files

	// Sum up all the histograms in the histo vector
	TH2D* h2 = new TH2D(\
		Form("h2_%s", alias.c_str()), Form("h2_%s", alias.c_str()), \
		Enu_nbins, &Enu_binning[0], PiTheta_nbins, &PiTheta_binning[0]);
	for(int i = 0; i < n_files; i++) {
		h2->Add(h2_tmp[i]);
	}
		
	// Divide out flux on a bin-by-bin basis along the Enu axis
	// 1. Extract flux from the CONVENIENT output (the "generator 
	// flux"), rename, and get nbins
	TH1D* gen_flux = (TH1D*) files[0]->Get("FlatTree_FLUX");
	gen_flux->SetName(Form("generator_flux_%s", alias.c_str()));
	int gen_flux_nbins = gen_flux->GetNbinsX();

	// 2. Calculate the integrated generator flux, and scale the 
	// histogram by the integrated generator flux. 
	double gen_flux_int = gen_flux->Integral();
	h2->Scale(gen_flux_int);

	// 3. Get the "signal flux". This is the generator flux rebinned 
	// according to the Enu binning.
	TH1D* sig_flux = new TH1D("sig_flux", "sig_flux", Enu_nbins, \
		&Enu_binning[0]);
	for(int i = 1; i <= gen_flux_nbins; i++) {
		sig_flux->Fill(gen_flux->GetBinCenter(i), \
			gen_flux->GetBinContent(i));
	}
		
	// 4. Divide out flux on a bin-by-bin basis
	for(int i = 1; i <= h2->GetXaxis()->GetNbins(); i++) {
		float sig_flux_bin_content = sig_flux->GetBinContent(i);
		for(int j = 1; j <= h2->GetYaxis()->GetNbins(); j++) {
			h2->SetBinContent(i, j, h2->GetBinContent(i, j) / \
				sig_flux_bin_content);
		}
	}

	// 5. This is typically where we would scale by the neutrino energy, 
	// but since this is a double-differential measurement we will 
	// instead divide all bins by their bin areas, and not divide by 
	// energy. The result this example will give is therefore 
	// unconventional. Moreover, the variables used would never be 
	// paired together (Enu isn't even measured directly, for one). 
	// Nevertheless, they illustrate how to use Convenient well. 

	// Divide histograms by their bin widths to get differential cross 
	// sections
	for(int i = 1; i <= h2->GetXaxis()->GetNbins(); i++) {
		for(int j = 1; j <= h2->GetYaxis()->GetNbins(); j++) {
			h2->SetBinContent(i, j, h2->GetBinContent(i, j) / \
			h2->GetXaxis()->GetBinWidth(i) / \
			h2->GetYaxis()->GetBinWidth(j));
		}
	}

		// Append the histogram to the vector
	h2_histo_vec.push_back(std::make_tuple(h2, alias));
	} // Move on to the next generator */
			
		// Triple-differential, flux-averaged cross sections //

		// Assumes we have delcared a histogram h3

/*		// Grab the scale factor due to combining files
		double file_scale_factor = std::get<2>(file_list[file_index]);

		// Loop over events and fill histograms
		while(reader.Next()) {
			// Select neutrino type and CC/NC
			// Ex: If the neutrino is not numu, or it is not a CC event, 
			// skip the event.
			if((*nu_in_flavor != 14) || (! *flagCC)) continue;
			else {
				// Here's where differential xsec variables are defined 
				// and used to make cuts. Kinematic variables are stored 
				// in each event as vectors of vectors, where each 
				// interior vector is a 4-momentum (E, px, py, pz), and 
				// each particle in the event contributes one of these 
				// to the external vector. The external vector is 
				// ordered by decreasing energy. So, to access the 
				// second most-energetic particle's py, call 
				// particle[1][2]. All other differential xsec variables 
				// can be constructed with the information in the files.
				// In this case:
				// If there is at least one pi+, and the most energetic 
				// one is above the threshold, then fill the histogram with 
				// the neutrino energy, pion angle, and Eavail, weighted by 
				// the generator scale factor and event weight.
				if(*n_pips > 0) {
					float pip_E = pips[0][0];
					if(phaseSpacecut(pip_E)) {
						float PiTheta = calcTheta(pips[0]);
						// Construct Eavail, from TruthVars.cxx in CAFAna
						float Eav = 0.0;
						for(int i = 0; i < *n_protons; i++) {
							std::vector<float> particle = {protons[i][0], \
								protons[i][1], protons[i][2], protons[i][3]};
						Eav += addtoEavail(particle, 2212);
						}
						for(int i = 0; i < *n_pips; i++) {
							std::vector<float> particle = {pips[i][0], \
								pips[i][1], pips[i][2], pips[i][3]};
							Eav += addtoEavail(particle, 211);
						}
						for(int i = 0; i < *n_pims; i++) {
							std::vector<float> particle = {pims[i][0], \
								pims[i][1], pims[i][2], pims[i][3]};
							Eav += addtoEavail(particle, -211);
						}
						for(int i = 0; i < *n_pi0s; i++) {
							std::vector<float> particle = {pi0s[i][0], \
								pi0s[i][1], pi0s[i][2], pi0s[i][3]};
							Eav += addtoEavail(particle, 111);
						}
						for(int i = 0; i < *n_electrons; i++) {
							std::vector<float> particle = {electrons[i][0], \
								electrons[i][1], electrons[i][2], \
								electrons[i][3]};
							Eav += addtoEavail(particle, 111);
							// We store electrons and positrons here. 
							// They are treated equivalently in the 
							// Eavail calculation
						}
						for(int i = 0; i < *n_gammas; i++) {
							std::vector<float> particle = {gammas[i][0], \
								gammas[i][1], gammas[i][2], gammas[i][3]};
							Eav += addtoEavail(particle, 22);
						}
						for(int i = 0; i < *n_antiprotons; i++) {
							std::vector<float> particle = {\
								antiprotons[i][0], antiprotons[i][1], \
								antiprotons[i][2], antiprotons[i][3]};
							Eav += addtoEavail(particle, -2212);
						}
						for(int i = 0; i < *n_antineutrons; i++) {
							std::vector<float> particle = {\
								antineutrons[i][0], antineutrons[i][1], \
								antineutrons[i][2], antineutrons[i][3]};
							Eav += addtoEavail(particle, -2112);
						}
						for(int i = 0; i < *n_others; i++) {
							std::vector<float> particle = {others[i][0], \
								others[i][1], others[i][2], others[i][3]};
							Eav += addtoEavail(particle, others[i][4]);
						}							
						h3_tmp[file_index]->Fill(*nu_in_E, PiTheta, Eav, \
							*genscalefactor * *event_weight * \
							file_scale_factor);
					} // Brace for phase space cut
				} // Brace for n_pi < 0 cut
			} // Brace for numuCC cut
		} // Brace for looping over events
	} // Brace for looping over files
	// Sum up all the histograms in the histo vector
	TH3D* h3 = new TH3D(\
		Form("h3_%s_%i", alias.c_str(), i), \
		Form("h3_%s_%i", alias.c_str(), i), Enu_nbins, \
		&Enu_binning[0], PiTheta_nbins, &PiTheta_binning[0], \
		Eavail_nbins, &Eavail_binning[0]);
	for(int i = 0; i < n_files; i++) {
		h3->Add(h3_tmp[i]);
	}

	// Divide out flux on a bin-by-bin basis along the Enu axis
	// 1. Extract flux from the CONVENIENT output (the "generator 
	// flux"), rename, and get nbins
	TH1D* gen_flux = (TH1D*) files[0]->Get("FlatTree_FLUX");
	gen_flux->SetName(Form("generator_flux_%s", alias.c_str()));
	int gen_flux_nbins = gen_flux->GetNbinsX();

	// 2. Calculate the integrated generator flux, and scale the 
	// histogram by the integrated generator flux. 
	double gen_flux_int = gen_flux->Integral();
	h3->Scale(gen_flux_int);

	// 3. Get the "signal flux". This is the generator flux rebinned 
	// according to the Enu binning.
	TH1D* sig_flux = new TH1D("sig_flux", "sig_flux", Enu_nbins, \
		&Enu_binning[0]);
	for(int i = 1; i <= gen_flux_nbins; i++) {
		sig_flux->Fill(gen_flux->GetBinCenter(i), \
			gen_flux->GetBinContent(i));
	}
		
	// 4. Divide out flux on a bin-by-bin basis
	for(int i = 1; i <= h3->GetXaxis()->GetNbins(); i++) {
		float sig_flux_bin_content = sig_flux->GetBinContent(i);
		for(int j = 1; j <= h3->GetYaxis()->GetNbins(); j++) {
			for(int k = 1; k <= h3->GetZaxis()->GetNbins(); k++) {
				h3->SetBinContent(i, j, k, h3->GetBinContent(i, j, k) / \
				sig_flux_bin_content);
			}
		}
	}

	// 5. This is typically where we would scale by the neutrino energy, 
	// but since this is a double-differential measurement we will 
	// instead divide all bins by their bin areas, and not divide by 
	// energy. The result this example will give is therefore 
	// unconventional. Moreover, the variables used would never be 
	// paired together (Enu isn't even measured directly, for one). 
	// Nevertheless, they illustrate how to use Convenient well. 

	// Divide histograms by their bin widths to get differential cross 
	// sections
	for(int i = 1; i <= h3->GetXaxis()->GetNbins(); i++) {
		for(int j = 1; j <= h3->GetYaxis()->GetNbins(); j++) {
			for(int k = 1; k <= h3->GetZaxis()->GetNbins(); k++) {
				h3->SetBinContent(i, j, k, h3->GetBinContent(i, j, k) / \
				h3->GetXaxis()->GetBinWidth(i) / \
				h3->GetYaxis()->GetBinWidth(j) / \
				h3->GetZaxis()->GetBinWidth(k));
			}
		}
	}

	// Append the histogram to the vector
	h3_histo_vec.push_back(std::make_tuple(h3, alias));
	} // Move on to the next generator */

	// Xsec Visualization

	// Single differential cross sections can be visualized with the 
	// following code. Since visualization is incredibly analysis-specific, 
	// further examples have not been written.

	/* // Initialize a new canvas
	TCanvas* c = new TCanvas("c", "xsec compare");
	// Create a file to store the histograms
	TFile histo_file("xsec_histos.root", "RECREATE");
	
	// Initialize the legend. Format is (xmin, ymin, xmax, ymax)
	auto legend = new TLegend(0.25, 0.70, 0.45, 0.85);

	// Loop over the histograms in the histo_vec and draw
	for(int i = 0; i < n_files; i++) {
		// Extrac the histo from the list, set color, and don't display stats
		TH1* h = histo_vec[i];
		h->SetLineColor(myColors[i]);
		h->SetStats(0);
		// If it's the first histogram on the cavas, add axis labels and 
		// title
		if(i == 0) {
			h->Draw("HIST");
			h->GetXaxis()->SetTitle("E_{#nu} (GeV)");
			h->GetYaxis()->SetTitle(\
				"#sigma / E_{#nu} (cm^{2} GeV^{-1} nucleon^{-1})");
			h->SetTitle("Total NumuCC xsec vs. E_{#nu}");
			// Add appropriate legend entry
			legend->AddEntry(h, "GENIE");
		}
		// If it's the data file, draw with a different format
		else if (i == (n_files - 1)) {
			legend->AddEntry(h, "Fake Data");
			h->Draw("SAME e1p");
		}
		// For all other MC files, draw with the same format
		else {
			legend->AddEntry(h, "NuWro");
			h->Draw("HIST SAME");
		}
		// Write histo to root file
		h->Write();
	}
	// Draw legend
	legend->Draw();
	
	// Save the canvas with the overlaid histograms and close the root file
	c->Print("Generic_xsec_analysis_Enu.png");
	histo_file.Close();*/
}
