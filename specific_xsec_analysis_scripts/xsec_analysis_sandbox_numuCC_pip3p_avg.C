// Author: Colin Weber (webe1077@umn.edu)
// Date: 29 November 2023
// Purpose: To calculate a numuCC pi+ cross section in outgoing pion 
// 3-momentum.
//
// Command: cafe xsec_analysis_sandbox_numuCC_pip3p_avg.C

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

void xsec_analysis_sandbox_numuCC_pip3p_avg() {
	// Open CONVENIENT outputs to analyze
	TFile* genie_file = TFile::Open("/nova/app/users/colweber/Convenient/generator_outputs_500MeV_fluxbinwidth/GENIE/FHC/numu_only/GENIE:N18_10j_02_11a.20230823174457.2.gntp.ghep.root.FHC.convenient_output.root");
	TFile* nuwro_file = TFile::Open("/nova/app/users/colweber/Convenient/generator_outputs_500MeV_fluxbinwidth/NuWro/FHC/numu_only/NuWro:20230821params.txt.20230824080707.2.eventsout.root.FHC.convenient_output.root");
	TFile* fake_data_file = TFile::Open("/nova/app/users/colweber/Convenient/generator_outputs_500MeV_fluxbinwidth/GENIE/FHC/numu_only/GENIE:AR23_20i_00_000.20230823174457.2.gntp.ghep.root.FHC.convenient_output.root");
	// Organize files for access later
	TFile* file_list[3] = {genie_file, nuwro_file, fake_data_file};
	// This line does not need to be changed
	int n_files = sizeof(file_list) / sizeof(file_list[0]);

	// This section does not need to be changed
	TH1::AddDirectory(kFALSE);
	TH1* histo_vec[n_files];

	// Declare constants for later use
	double pip_threshold_p = 0.2; // GeV
	double pip_mass = 0.1396; // GeV
	double pip_threshold_E = 0.4; // GeV	

	// Set binning
	int genie_data_n_bins_pip = 50;
	double genie_data_pip_low = 0;
	double genie_data_pip_high = 0.375;

	// To have more-interesting colors in plotting
	std::vector<int> myColors = {kMagenta+1, kCyan+1, kBlue+1, kOrange+2, \
		kGreen+1, kRed+1, kGray};

	// Loop over the CONVENIENT outputs
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

		// Single differential, flux-averaged cross section //

		// Initialize histograms
		// Format is (name, title, n_bins, lower limit, upper limit)
		TH1D* h1 = new TH1D("h1", "Leading PiP 3-momentum spectrum", \
			genie_data_n_bins_pip, genie_data_pip_low, genie_data_pip_high);
		
		// Loop over events and fill the histogram
		bool first_entry = true;
		double scale_factor;
		while (reader.Next()) {
			// Select neutrino type. 14 = muon neutrino
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
					// If there is a pi+ and its energy is less than the 
					// threshold: 
					if(*n_pips > 0 && pips[0][0] < pip_threshold_E) {
						// Calculate pi+ 3-momentum and fill histogram
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
		// Scale by the scale factor from NUISANCE.
		h1->Scale(scale_factor);

		// Divide histograms by their bin widths to get differential cross 
		// sections
		for(int i = 1; i <= h1->GetNbinsX(); i++) {
			h1->SetBinContent(i, h1->GetBinContent(i) / h1->GetBinWidth(i));
		}

		// Append histogram to the vector
		histo_vec[file_index] = h1;
	}

	// Single differential cross sections can be visualized with the 
	// following code
	// Create a new canvas
	TCanvas* c = new TCanvas("c", "xsec compare");
	// Create a file to store the histograms in
	TFile histo_file("xsec_histos.root", "RECREATE");
	
	// Initialize the legend. Format is (xmin, ymin, xmax, ymax)
	auto legend = new TLegend(0.65, 0.30, 0.85, 0.45);

	// Add 10% error bars to the data file
	for(int i = 1; i <= histo_vec[n_files - 1]->GetNbinsX(); i++) {
		histo_vec[n_files - 1]->SetBinError(i, \
			histo_vec[n_files - 1]->GetBinContent(i) * 0.1);
	}

	// Loop over the histograms in the histo_vec to draw
	for(int i = 0; i < n_files; i++) {
		// Create the histo, set color, and don't display stats box
		TH1* h = histo_vec[i];
		h->SetLineColor(myColors[i]);
		h->SetStats(0);
		// If it's the first time we're drawing, add axis labels and title
		if(i == 0) {
			h->Draw("HIST");
			h->GetXaxis()->SetTitle("|p_{#pi^{+}}| (GeV)");
			h->GetYaxis()->SetTitle(\
				"d#sigma/d|p_{#pi^{+}}| (cm^{2} GeV^{-1} nucleon^{-1})");
			h->SetTitle("Numu CC Cross Section");
			// Add entry to the legend
			legend->AddEntry(h, "GENIE");
		}
		// If it's the data file, draw with a different format
		else if (i == (n_files - 1)) {
			legend->AddEntry(h, "Fake Data");
			h->Draw("SAME e1p");
		}
		// For all other files, draw with the same format as the first one.
		else {
			legend->AddEntry(h, "NuWro");
			h->Draw("HIST SAME");
		}
		// Write histo to root file
		h->Write();
	}
	// Draw legend
	legend->Draw();
	
	// Save the canvas with the overlaid histograms and close the file
	c->Print("numuCC_pip3p_avg.png");
	histo_file.Close();
}
