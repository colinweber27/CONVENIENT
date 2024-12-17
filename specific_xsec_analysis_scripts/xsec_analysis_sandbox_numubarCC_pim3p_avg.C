// Author: Colin Weber (webe1077@umn.edu)
// Date: 29 November 2023
// Purpose: To calculate a numubar CC pi- cross section in outgoing pion 
// 3-momentum. 
//
// Command: cafe xsec_analysis_sandbox_numubarCC_pim3p_avg.C

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

void xsec_analysis_sandbox_numubarCC_pim3p_avg() {
	// Open CONVENIENT outputs to analyze.
	TFile* genie_file = TFile::Open("genie_convenient_demo.root");
	// Organize files for access later
	TFile* file_list[1] = {genie_file};
	// Do not modify this line
	int n_files = sizeof(file_list) / sizeof(file_list[0]);

	// Do not modify this section
	TH1::AddDirectory(kFALSE);
	TH1* histo_vec[n_files];

	// Declare constants for later use
	double pim_threshold_E = 5.0 // GeV

	// Set binning
	int genie_data_n_bins_piE = 20;
	double genie_data_piE_low = 0;
	double genie_data_piE_high = 5;

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
			TTreeReaderArray<std::vector<float>> pims(reader, "FS_PiMs");
			TTreeReaderValue<int> n_pims(reader, "n_FS_PiMs");

			// Grab leaves holding the NUISANCE normalization factor
			TTreeReaderValue<double> fscalefactor(reader, "fScaleFactor");

		// Single differential, flux-averaged cross section //

		// Initialize histograms
		// Format is (name, title, n_bins, lower limit, upper limit)
		TH1D* h1 = new TH1D("h1", "Leading PiM 3-momentum spectrum", \
			genie_data_n_bins_piE, genie_data_piE_low, genie_data_piE_high);
		
		// Loop over events and fill the histogram
		bool first_entry = true;
		double scale_factor;
		while (reader.Next()) {
			// Select neurtino type
			if(*nu_in_flavor != -14) continue;
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
					// If there is a pi- and its energy is less than the 
					// threshold:
					if(*n_pims > 0 && pims[0][0] < pim_threshold_E) {
						// Calculate pi- 3-momentum and fill histogram
						float pim_px = pims[0][1];
						float pim_py = pims[0][2];
						float pim_pz = pims[0][3];
						float pim_p = sqrt(pow(pim_px, 2) + \
							pow(pim_py, 2) + pow(pim_pz, 2));
						h1->Fill(pim_p);
					}
				}
			}
		}
		// Scale by the scale factor from NUISANCE
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
	auto legend = new TLegend(0.25, 0.70, 0.45, 0.85);

	// Loop over the histograms in the histo_vec to draw
	for(int i = 0; i < n_files; i++) {
		// Create the histo, set color, and don't display stats box
		TH1* h = histo_vec[i];
		h->SetLineColor(myColors[i]);
		h->SetStats(0);
		// If it's the first time we're drawing, add axis labels and title
		if(i == 0) {
			h->Draw("HIST");
			h->GetXaxis()->SetTitle("|p_{#pi}| (GeV)");
			h->GetYaxis()->SetTitle(\
				"d#sigma/d|p_{#pi}| (cm^{2} GeV^{-1} nucleon^{-1})");
			h->SetTitle("Numubar CC Cross Section");
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
	
	// Save the cavas with the overlaid histos and close the file
	c->Print("numubarCC_pim3p_avg.png");
	histo_file.Close();
}
