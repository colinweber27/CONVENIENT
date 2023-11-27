// Author: Colin Weber (webe1077@umn.edu)
// Date: 8 September 2023
// Purpose: To compare cross sections from data and different generators
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

// C++ includes
#include <vector>
#include <iostream>
#include <cmath>
#include <random>


void xsec_analysis_macro() { // Define the macro
	// Read in data
	// This section is analysis-specific, so it is not written here in 
	// general terms. Individual analyses will have to write this section as
	// needed.
	
	// Read in files from the tunes we want to compare
	// Initialize pointers-to-TFile to point to the outputs from Convenient
	TFile* genie_file = TFile::Open("GENIE:N18_10j_02_11a.20230823174457.2.gntp.ghep.root.FHC.convenient_output.root");
	TFile* nuwro_file = TFile::Open("NuWro:20230821params.txt.20230824080707.2.eventsout.root.FHC.convenient_output.root");

	// Read in fake data
	// The use of fake data is largely for development purposes, but may be 
	// recycled and used for other analyses. It is so far implemented only 
	// in the single-differenial, flux-integrated xsec analysis section.
	// Read in the file that has the fake data
	TFile* fake_data_file = TFile::Open("GENIE:AR23_20i_00_000.20230823174457.2.gntp.ghep.root.FHC.convenient_output.root");

	// Modify this section to organize each data file into an array. The 
	// array will be looped over, using each file to calculate a cross 
	// section.
	// Always set the data file last.
	TFile* file_list[3] = {genie_file, nuwro_file, fake_data_file};
	// The following line does not need to be changed.
	int n_files = sizeof(file_list) / sizeof(file_list[0]);

	// This section does not need to be changed
	// Initialize a vector to push the generated xsec histograms to.
	// Since all ROOT histogram classes are derived from TH1, we initialize 
	// the vector to hold TH1*.
	// We also set the Add Directory option to be false so that each new 
	// pointer points to a unique histogram.
	TH1::AddDirectory(kFALSE);
	TH1* histo_vec[n_files];

	// This section needs to be changed depending on the analysis.
	// Declare constants for later use, such as cuts on some energy 
	// threshold.
	double pip_threshold_p = 0.2; // GeV
	double pip_mass = 0.1396; // GeV
	double pip_threshold_E = sqrt(pow(pip_threshold_p, 2) + \
		pow(pip_mass, 2));

	// This section, Set Binning, is very analysis dependend and may have 
	// to be heavily modified.
	// Set binning
	//
	// Binning variables in general is totally up to individual analyses for 
	// a flux-averaged cross section calculation. The binning of the data 
	// should be optimized, and then the binning of the MC should match the 
	// data binning. There is an exception for when we calculate flux-
	// integrated cross sections using MC data, however. The NUISANCE 
	// prescription for doing this requires that one of the axes must be 
	// incident neutrino energy, and the binning of this axis must be 
	// identical to the binning of the flux spectrum corresponding to those 
	// events. If incident neutrino energy is also one of the differential 
	// cross section variables, then that axis can be binned as desired, but 
	// the flux axis must still match the flux binning. 
	//
	// When initializing histograms, the format is (name, title, x n_bins, 
	// x lower limit, x upper limit, y n_bins, y lower limit, y upper limit) 
	// for constant binning, or (name, title, x n_bins, array of bin edges, 
	// y n_bins, array of bin edges) for variable binning.
	//
	// This example demonstrates how to set the data binning for a single-
	// differential analysis in which we want constant binning. It can be 
	// extended to additional numbers of differential xsec variables and/or 
	// variable binning.
	// 
	// Set number of bins
	int fake_data_n_bins_p = 50; 

	// Set histogram minimum. In this example, we want it to be the pion 
	// momentum threshold - 5%.
	double fake_data_p_low = pip_threshold_p * 0.95;
	
	// Set the histogram maximum. In this example, we want it to be the 
	// max pion momentum + 5%.
	// Declare a TTreeReader to help read the tree
	TTreeReader fake_data_reader("generator_data", fake_data_file);

	// Declare TTreeReader arrays and values to hold the desired data 
	// from the trees. Format is (reader, leaf).
	// Grab info on CC/NC and neutrino type
	TTreeReaderValue<bool> flagCC(fake_data_reader, "flagCC");
	TTreeReaderValue<int> nu_in_flavor(fake_data_reader, "NuIn.Flavor");
	
	// Grab leaves for final state particles that make up our 
	// topology of interest. It is from these values that we can 
	// extract final state particle kinematics.
	TTreeReaderArray<std::vector<float>> pips(fake_data_reader, "FS_PiPs");
	TTreeReaderValue<int> n_pips(fake_data_reader, "n_FS_PiPs");

	// Grab leaves holding the NUISANCE normalization factor
	TTreeReaderValue<double> fscalefactor(fake_data_reader, "fScaleFactor");

	// Loop over the events to find the maximum momentum
	bool fake_data_firstEntry = true;
	double fake_data_scale_factor;
	double max_p = 0.0;
	while(fake_data_reader.Next()) {
		// Select neutrino type
		if(*nu_in_flavor != 14) continue;
		else {
			if (fake_data_firstEntry) {
				fake_data_scale_factor = *fscalefactor;
				fake_data_firstEntry = false;
			}	
			// Select NC/CC events
			if(! *flagCC) continue;
			else {
				if(*n_pips > 0 && pips[0][0] > pip_threshold_E) {
					float pip_px = pips[0][1];
					float pip_py = pips[0][2];
					float pip_pz = pips[0][3];
					float pip_p = sqrt(pow(pip_px, 2) + pow(pip_py, 2) + \
						pow(pip_pz, 2));
					if(pip_p > max_p) {
						max_p = pip_p;
					}
				}
			}
		}
	}
	double fake_data_p_high = max_p * 1.05;
	// fake_data_n_bins_p, fake_data_p_low, and fake_data_p_high will be 
	// used when initializing histograms with 3-momenta as a differential 
	// cross section variable.
	// Other binning parameters can be declared similarly
	int fake_data_n_bins_cos = 30;
	double fake_data_cos_low = -1;
	double fake_data_cos_high = 1;
	int fake_data_n_bins_E = 50;
	double fake_data_E_low = 0;
	double fake_data_E_high = 20;

	// This section does not need to be changed.
	// Initialize list of colors to use for plotting histograms
	std::vector<int> myColors = {kMagenta+1, kCyan+1, kBlue+1, kOrange+2, \
		kGreen+1, kRed+1, kGray};

	// Parts of this section are analysis-dependent
	// Loop over the data files.
	for(int file_index = 0; file_index < n_files; file_index++) {
		// Declare the file to use
		TFile* data_file = file_list[file_index];

		// Declare a TTreeReader to help read the tree
		TTreeReader reader("generator_data", data_file);

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
			TTreeReaderArray<std::vector<float>> pips(reader, "FS_PiPs");
			TTreeReaderValue<int> n_pips(reader, "n_FS_PiPs");

			// Grab leaves holding the NUISANCE normalization factor
			TTreeReaderValue<double> fscalefactor(reader, "fScaleFactor");

		// The rest of the macro is broken down into sections, one for each 
		// combination of cross section flux treatment (averaged or 
		// integrated) and number of differential cross section variables 
		// (1, 2, or 3). Simply uncomment the section that corresponds to 
		// the type of analysis you're doing, and modify lines only within 
		// that section. Within a section, the data files are looped over 
		// and a cross section is calculated for each. The only things that 
		// will need to be changed are the lines governing the filling of 
		// histograms. Plotting of the cross sections is handled after 
		// creating all histograms, although example code is only written to 
		// handle single-differential cross sections. 
		
//########################################################################//
	// Flux-averaged cross sections //
//#######################################################################//
		// To do flux-averaged cross sections, the general procedure is:
			// 1. For an n-differential cross section, create an n-dim. 
			// histogram of the differential variables
			// 2. Multiply by the NUISANCE scale factor to normalize 
			// generators to each other. If we stop here, we have the cross 
			// section as a function of the differential variables.
			// 3. Divide by the bin widths of each variable to get the 
			// flux-averaged differential cross section

		// Single differential, flux-averaged cross section //

		/*
		// Initialize histograms
		// Format is (name, title, n_bins, lower limit, upper limit)
		TH1D* h1 = new TH1D("h1", "Leading PiP 3-momentum spectrum", \
			fake_data_n_bins_p, fake_data_p_low, fake_data_p_high);
		
		// Loop over events and fill the histogram
		bool first_entry = true;
		double scale_factor;
		while (reader.Next()) {
			// Select neurtino type
			if(*nu_in_flavor != 14) continue;
			else {
				// Scale factors are the same for each event, so we only 
				// need to set the scale factor once. They depend on the 
				// total inclusive cross section, which is dependent on the 
				// generator and neutrino flavor. 
				if (first_entry) {
					scale_factor = *fscalefactor;
					first_entry = false;
				}
				// Select NC/CC events
				if(! *flagCC) continue;
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
					// If there is at least one pip, and the most energetic 
					// one is above the threshold, then get its 3-momentum 
					// and fill a histogram with it.
					if(*n_pips > 0 && pips[0][0] > pip_threshold_E) {
						float pip_px = pips[0][1];
						float pip_py = pips[0][2];
						float pip_pz = pips[0][3];
						float pip_p = sqrt(pow(pip_px, 2) + \
							pow(pip_py, 2) + pow(pip_pz, 2));
					
						h1->Fill(pip_p);
					}
				}
			}
		}
		
		// Multiply histograms by their scale factors to get cross sections 
		// in units of cm^2 / nucleon
		h1->Scale(scale_factor);

		// Divide histograms by their bin widths to get differential cross 
		// sections
		for(int i = 1; i <= h1->GetNbinsX(); i++) {
			h1->SetBinContent(i, h1->GetBinContent(i) / h1->GetBinWidth(i));
		}

		// Append histogram to the vector
		histo_vec[file_index] = h1;
		*/

		// Double-differential, flux-averaged cross sections //
	
		/*
 		// Initialize histograms
		// Format is (name, title, x n_bins, x lower limit, x upper limit, 
		// y n_bins, y lower limit, y upper limit)
		TH2D* h2 = new TH2D("h2", "Leading PiP cos, 3-momentum spectrum", \
			fake_data_n_bins_cos, fake_data_cos_low, fake_data_cos_high, \
			fake_data_n_bins_p, fake_data_p_low, fake_data_p_high);

		// Loop over events and fill histograms
		bool firstEntry = true;
		double scale_factor;
		while(reader.Next()) {
			// Select neutrino type
			if(*nu_in_flavor != 14) continue;
			else {
				// Scale factors are the same for each event, so we only 
				// need to set the scale factor once. They depend on the 
				// total inclusive cross section, which is dependent on the 
				// generator and neutrino flavor. 
				if (firstEntry) {
					scale_factor = *fscalefactor;
					firstEntry = false;
				}	
				// Select NC/CC events
				if(! *flagCC) continue;
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
					// If there is at least one pip, and the most energetic 
					// one is above the threshold, then get its 3-momentum 
					// and cos wrt the incoming neutrino direction (set to 
					// be (0, 0, 1)), and fill a histogram with these values.
					if(*n_pips > 0 && pips[0][0] > pip_threshold_E) {
						float pip_px = pips[0][1];
						float pip_py = pips[0][2];
						float pip_pz = pips[0][3];
						float pip_p = sqrt(pow(pip_px, 2) + \
							pow(pip_py, 2) + pow(pip_pz, 2));

						float pip_cos = pip_pz / pip_p;
					
						h2->Fill(pip_cos, pip_p);
					}
				}
			}
		}

		// Multiply histograms by their scale factors to get cross sections 
		// in units of cm^2 / nucleon
		h2->Scale(scale_factor);

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
		histo_vec[file_index] = h2;
		*/

		// Triple-differential, flux-averaged cross sections //

		/*
		// Initialize histograms
		// Format is (name, title, x n_bins, x lower limit, x upper limit, 
		// y n_bins, y lower limit, y upper limit)
		TH3D* h3 = new TH3D("h3", "Leading PiP cos, 3-momentum, nu E \
			spectrum", \
			fake_data_n_bins_cos, fake_data_cos_low, fake_data_cos_high, \
			fake_data_n_bins_p, fake_data_p_low, fake_data_p_high, \
			fake_data_n_bins_E, fake_data_E_low, fake_data_E_high);
	
		// Loop over events and fill histograms
		bool firstEntry = true;
		double scale_factor;
		while(reader.Next()) {
			// Select neutrino type
			if(*nu_in_flavor != 14) continue;
			else {
				// Scale factors are the same for each event, so we only 
				// need to set the scale factor once. They depend on the 
				// total inclusive cross section, which is dependent on the 
				// generator and neutrino flavor. 
				if (firstEntry) {
					scale_factor = *fscalefactor;
					firstEntry = false;
				}	
				// Select NC/CC events
				if(! *flagCC) continue;
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
					// If there is at least one pip, and the most energetic 
					// one is above the threshold, then get its 3-momentum 
					// and cos wrt the initial neutrino direction (set to be 
					// (0, 0, 1)). Get the neutrino energy as well. Finally, 
					// fill a histogram with these variables.
					if(*n_pips > 0 && pips[0][0] > pip_threshold_E) {
						float pip_px = pips[0][1];
						float pip_py = pips[0][2];
						float pip_pz = pips[0][3];
						float pip_p = sqrt(pow(pip_px, 2) + \
							pow(pip_py, 2) + pow(pip_pz, 2));

						float pip_cos = pip_pz / pip_p;

						float nu_E = *nu_in_e;

					h3->Fill(pip_cos, pip_p, nu_E);
					}
				}
			}
		}
		
		// Multiply histograms by their scale factors to get cross sections 
		// in units of cm^2 / nucleon
		h3->Scale(scale_factor);

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
		histo_vec[file_index] = h3;
		*/
		
//########################################################################//
		// Flux-integrated cross sections //
//########################################################################//
		// To do flux-integrated cross sections, the general procedure is:
			// 1. For an n-differential cross section, create an (n+1)-dim. 
			// spectrum of the differential variables and the incident 
			// neutrino energy.
			// 2. Multiply by the NUISANCE scale factor to normalize 
			// generators to each other.
			// 3. Grab the flux histogram used to create the NUISANCE file, 
			// normalize it, and then divide the neutrino energy axis by the 
			// normalized flux histogram. This removes the integrated flux 
			// factor in the NUISANCE scale factor, and ensures that we are 
			// consistent in treating each energy bim separately before 
			// integrating out the flux altogether.
			// 4. Integrate over the incident neutrino energies to get an 
			// n-dim. spectrum. If we stop here, we have a flux-integrated 
			// cross section as a function of the differential variables.
			// 5. Divide by the bin widths of each variable to get a 
			// flux-integrated differential cross section.

		// Single differential, flux-integrated cross section //

		
		// Access the flux and grab the flux binning for constructing the 
		// flux axis of the histogram. This is needed for a flux-integrated 
		// cross section calculation. 
		TH1D* flux = (TH1D*) data_file->Get("FlatTree_FLUX");
		int n_flux_bins = flux->GetNbinsX();
		double flux_bins_low = flux->GetBinLowEdge(1);
		double flux_bins_high = flux->GetBinLowEdge(n_flux_bins) + \
			flux->GetBinWidth(n_flux_bins);	

		// Initialize histograms
		// Format is (name, title, x n_bins, x lower limit, x upper limit, 
		// y n_bins, y lower limit, y upper limit) for constant binning.
		// Format is (name, title, x n_bins, array of bin edges, y n_bins, 
		// array of bin edges) for variable binning.
		TH2D* h2 = new TH2D("h2", "Leading PiP 3-momentum spectrum", \
			fake_data_n_bins_p, fake_data_p_low, fake_data_p_high, \
			n_flux_bins, flux_bins_low, flux_bins_high);

		// Loop over events and fill histograms
		bool firstEntry = true;
		double scale_factor;
		while(reader.Next()) {
			// Select neutrino type
			if(*nu_in_flavor != 14) continue;
			else {
				// Scale factors are the same for each event, so we only 
				// need to set the scale factor once. They depend on the 
				// total inclusive cross section, which is dependent on the 
				// generator and neutrino flavor. 
				if (firstEntry) {
					scale_factor = *fscalefactor;
					firstEntry = false;
				}	
				// Select NC/CC events
				if(! *flagCC) continue;
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
					// If there is at least one pip, and the most energetic 
					// one is above the threshold, then get its 3-momentum. 
					// Get the neutrino energy as well. Finally, fill a 
					// histogram with these variables
					if(*n_pips > 0 && pips[0][0] > pip_threshold_E) {
						float pip_px = pips[0][1];
						float pip_py = pips[0][2];
						float pip_pz = pips[0][3];
						float pip_p = sqrt(pow(pip_px, 2) + \
							pow(pip_py, 2) + pow(pip_pz, 2));

						float nu_E = *nu_in_e;
					
						h2->Fill(pip_p, nu_E);
					}
				}
			}
		}
		
		// Multiply histograms by their scale factors to get cross sections 
		// in units of cm^2 / nucleon
		h2->Scale(scale_factor);
	
		// Normalize fluxes, and divide the histograms' energy axes by the 
		// flux
		flux->Scale(1.0 / flux->Integral());

		int n_xbins = h2->GetXaxis()->GetNbins();
		for(int i = 1; i <= n_xbins; i++) {
			for(int j = 1; j <= n_flux_bins; j++) {
				h2->SetBinContent(i, j, h2->GetBinContent(i, j) / \
					flux->GetBinContent(j));
			}
		}
	
		// Integrate out the energy by projecting onto the x axis, and 
		// assign to the histogram vector.
		histo_vec[file_index] = h2->ProjectionX("h1");

		// Divide histograms by their bin widths to get differential cross 
		// sections
		for(int i = 1; i <= histo_vec[file_index]->GetNbinsX(); i++) {
			histo_vec[file_index]->SetBinContent(i, \
				histo_vec[file_index]->GetBinContent(i) / \
				histo_vec[file_index]->GetBinWidth(i));
		}
		

		// Double-differential, flux-integrated cross sections //

		/*
		// Access the flux and grab the flux binning for constructing the 
		// flux axis of the histogram. This is needed for a flux-integrated 
		// cross section calculation. 
		TH1D* flux = (TH1D*) data_file->Get("FlatTree_FLUX");
		int n_flux_bins = flux->GetNbinsX();
		double flux_bins_low = flux->GetBinLowEdge(1);
		double flux_bins_high = flux->GetBinLowEdge(n_flux_bins) + \
			flux->GetBinWidth(n_flux_bins);	

		// Initialize histograms
		// Format is (name, title, x n_bins, x lower limit, x upper limit, 
		// y n_bins, y lower limit, y upper limit, z n_bins, z lower limit, 
		// z upper limit) for constant binning.
		// Format is (name, title, x n_bins, array of bin edges, y n_bins, 
		// array of bin edges, z n_bins, array of bin edges) for variable 
		// binning.
		TH3D* h3 = new TH3D("h3", "Leading PiP 3-momentum, cos spectrum", \
			fake_data_n_bins_p, fake_data_p_low, fake_data_p_high, \
			fake_data_n_bins_cos, fake_data_cos_low, fake_data_cos_high, \
			n_flux_bins, flux_bins_low, flux_bins_high);

		// Loop over events and fill histograms
		bool firstEntry = true;
		double scale_factor;
		while(reader.Next()) {
			// Select neutrino type
			if(*nu_in_flavor != 14) continue;
			else {
				// Scale factors are the same for each event, so we only 
				// need to set the scale factor once. They depend on the 
				// total inclusive cross section, which is dependent on the 
				// generator and neutrino flavor. 
				if (firstEntry) {
					scale_factor = *fscalefactor;
					firstEntry = false;
				}	
				// Select NC/CC events
				if(! *flagCC) continue;
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
					// If there is at least one pip, and the most energetic 
					// one is above the threshold, then get its 3-momentum. 
					// Get the neutrino energy as well. Finally, fill a 
					// histogram with these variables
					if(*n_pips > 0 && pips[0][0] > pip_threshold_E) {
						float pip_px = pips[0][1];
						float pip_py = pips[0][2];
						float pip_pz = pips[0][3];
						float pip_p = sqrt(pow(pip_px, 2) + \
							pow(pip_py, 2) + pow(pip_pz, 2));

						float nu_E = *nu_in_e;

						float pip_cos = pip_pz / pip_p;
					
						h3->Fill(pip_p, pip_cos, nu_E);
					}
				}
			}
		}
		
		// Multiply histograms by their scale factors to get cross sections 
		// in units of cm^2 / nucleon
		h3->Scale(scale_factor);
	
		// Normalize fluxes, and divide the histograms' energy axes by the 
		// flux
		flux->Scale(1.0 / flux->Integral());

		int n_xbins = h3->GetXaxis()->GetNbins();
		int n_ybins = h3->GetYaxis()->GetNbins();
		for(int i = 1; i <= n_xbins; i++) {
			for(int j = 1; j <= n_ybins; j++) {
				for(int k = 1; k <= n_flux_bins; k++) {
					h3->SetBinContent(i, j, k, h3->GetBinContent(i, j, k) / \
						flux->GetBinContent(k));
				}
			}
		}
	
		// Integrate out the energy by projecting onto the xy plane, and 
		// attach to the histogram vector.
		histo_vec[file_index] = h3->Project3D("yx");

		// Divide histograms by their bin widths to get differential cross 
		// sections
		for(int i = 1; i <= n_xbins; i++) {
			for(int j = 1; j <= n_ybins; j++) {
				histo_vec[file_index]->SetBinContent(i, j, \
					histo_vec[file_index]->GetBinContent(i, j) / \
					histo_vec[file_index]->GetXaxis()->GetBinWidth(i) / \
					histo_vec[file_index]->GetYaxis()->GetBinWidth(j));
			}
		}
		*/
		
		// Triple-differential, flux-integrated cross sections //

		/*
		// Access the flux and grab the flux binning for constructing the 
		// flux axis of the histogram. This is needed for a flux-integrated 
		// cross section calculation. 
		TH1D* flux = (TH1D*) data_file->Get("FlatTree_FLUX");
		int n_flux_bins = flux->GetNbinsX();
		double flux_bins_low = flux->GetBinLowEdge(1);
		double flux_bins_high = flux->GetBinLowEdge(n_flux_bins) + \
			flux->GetBinWidth(n_flux_bins);	

		// Initialize histograms
		// With 4D histograms (recall, for an n-dim flux-integrated cross 
		// section, we need an n+1 dim histogram), format is
		// THnSparseD* (name, title, n_dim, array of n_bins, array of mins, 
		// array of maxs)
		int n_bins[4] = {fake_data_n_bins_p, fake_data_n_bins_cos, \
			fake_data_n_bins_E, n_flux_bins};
		double mins[4] = {fake_data_p_low, fake_data_cos_low, \
			fake_data_E_low, flux_bins_low};
		double maxs[4] = {fake_data_p_high, fake_data_cos_high, \
			fake_data_E_high, flux_bins_high};
		THnSparseD* h4 = new THnSparseD("h4", \
			"Leading PiP cos, 3-momentum, Nu E spectrum", \
			4, n_bins, mins, maxs);

		// Loop over events and fill histograms
		bool firstEntry = true;
		double scale_factor;
		while(reader.Next()) {
			// Select neutrino type
			if(*nu_in_flavor != 14) continue;
			else {
				// Scale factors are the same for each event, so we only 
				// need to set the scale factor once. They depend on the 
				// total inclusive cross section, which is dependent on the 
				// generator and neutrino flavor. 
				if (firstEntry) {
					scale_factor = *fscalefactor;
					firstEntry = false;
				}	
				// Select NC/CC events
				if(! *flagCC) continue;
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
					// If there is at least one pip, and the most energetic 
					// one is above the threshold, then get its 3-momentum. 
					// Get the neutrino energy as well. Finally, fill a 
					// histogram with these variables
					if(*n_pips > 0 && pips[0][0] > pip_threshold_E) {
						float pip_px = pips[0][1];
						float pip_py = pips[0][2];
						float pip_pz = pips[0][3];
						float pip_p = sqrt(pow(pip_px, 2) + \
							pow(pip_py, 2) + pow(pip_pz, 2));

						float nu_E = *nu_in_e;

						float pip_cos = pip_pz / pip_p;

						double fill_vec[4] = {pip_p, pip_cos, nu_E, nu_E};
					
						h4->Fill(fill_vec);
					}
				}
			}
		}
		
		// Multiply histograms by their scale factors to get cross sections 
		// in units of cm^2 / nucleon
		h4->Scale(scale_factor);
	
		// Normalize fluxes, and divide the histograms' energy axes by the 
		// flux
		flux->Scale(1.0 / flux->Integral());

		for(int i = 1; i <= fake_data_n_bins_p; i++) {
			for(int j = 1; j <= fake_data_n_bins_cos; j++) {
				for(int k = 1; k <= fake_data_n_bins_E; k++) {
					for(int l = 1; l <= n_flux_bins; l++) {
						int coord[4] = {i, j, k, l};
						float bin = h4->GetBin(coord);
						h4->SetBinContent(bin, \
						h4->GetBinContent(bin) / \
						flux->GetBinContent(l));
					}
				}
			}
		}
	
		// Integrate out the energy by projecting onto the xy plane, and 
		// attach to the histogram vector.
		histo_vec[file_index] = h4->Projection(0, 1, 2);

		// Divide histograms by their bin widths to get differential cross 
		// sections
		for(int i = 1; i <= fake_data_n_bins_p; i++) {
			for(int j = 1; j <= fake_data_n_bins_cos; j++) {
				for(int k = 1; k <= fake_data_n_bins_E; k++) {
					histo_vec[file_index]->SetBinContent(i, j, k, \
						histo_vec[file_index]->GetBinContent(i, j, k) / \
						histo_vec[file_index]->GetXaxis()->GetBinWidth(i) / \
						histo_vec[file_index]->GetYaxis()->GetBinWidth(j) / \
						histo_vec[file_index]->GetZaxis()->GetBinWidth(k));
				}
			}
		}
		*/
	}

	// Single differential cross sections can be visualized with the 
	// following code
	TCanvas* c = new TCanvas("c", "xsec compare");
	TFile histo_file("xsec_histos.root", "RECREATE");
	
	// Initialize the legend as well
	auto legend = new TLegend(0.25, 0.70, 0.45, 0.85);

	// Add error bars to the data file
	for(int i = 1; i <= histo_vec[n_files - 1]->GetNbinsX(); i++) {
		histo_vec[n_files - 1]->SetBinError(i, \
			histo_vec[n_files - 1]->GetBinContent(i) * 0.1);
	}	

	// Loop over the histograms in the histo_vec and draw
	for(int i = 0; i < n_files; i++) {
		TH1* h = histo_vec[i];
		h->SetLineColor(myColors[i]);
		h->SetStats(0);
		if(i == 0) {
			h->Draw("HIST");
			h->GetXaxis()->SetTitle("3-momentum (GeV)");
			h->GetYaxis()->SetTitle(\
				"d#sigma/d|p_{#pi +}| (cm^{2} GeV^{-1} nucleon^{-1})");
			h->SetTitle("NumuCC 1#pi^{+} d#sigma/d|p_{#pi +}|");
		}
		else if (i == (n_files - 1)) {
			h->Draw("SAME e1p");
		}
		else {
			h->Draw("HIST SAME");
		}
		h->Write();
		legend->AddEntry(h, TString::Format("file %i", i));		
	}
	legend->Draw();
	
	c->Print("Generic_xsec_analysis_p.png");
	histo_file.Close();
}
