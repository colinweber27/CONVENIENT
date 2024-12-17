// Author: Colin Weber (webe1077@umn.edu)
// Date: 29 November 2023
// Purpose: To calculate a numubar cross section in incoming neutrino 
// energy.  
//
// Command: cafe xsec_analysis_sandbox_numubarCC_Enu_avg.C

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

void xsec_analysis_sandbox_numubarCC_Enu_avg() {
	// Open CONVENIENT outputs to analyze
	TFile* genie_file = TFile::Open("genie_convenient_demo.root");
	// Organize files for access later
	TFile* file_list[1] = {genie_file};
	// Don't change
	int n_files = sizeof(file_list) / sizeof(file_list[0]);

	// Don't change
	TH1::AddDirectory(kFALSE);
	TH1* histo_vec[n_files];

	// Set binning
	int genie_data_n_bins_nuE = 100;
	double genie_data_nuE_low = 0;
	double genie_data_nuE_high = 10;

	// Let's use better colors for plotting
	std::vector<int> myColors = {kMagenta+1, kCyan+1, kBlue+1, kOrange+2, \
		kGreen+1, kRed+1, kGray};

	// Loop over CONVENIENT outputs
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

			// Grab leaves holding the NUISANCE normalization factor
			TTreeReaderValue<double> fscalefactor(reader, "fScaleFactor");

		// Single differential, flux-averaged cross section //

		// Grab the flux
		TH1D* flux = (TH1D*) data_file->Get("FlatTree_FLUX");
		// Rebin to match the bin width for the analysis
		flux->Rebin(2);
		double flux_bin_width = flux->GetBinWidth(0);
		int n_flux_bins = flux->GetNbinsX();
		double flux_bins_low = flux->GetBinLowEdge(1);
		double flux_bins_high = flux->GetBinLowEdge(n_flux_bins) + \
			flux_bin_width;
		double flux_int = flux->Integral("width");

		// Initialize histograms
		// Format is (name, title, n_bins, lower limit, upper limit)
		TH1D* h1 = new TH1D("h1", "E_{#nu} spectrum", \
			genie_data_n_bins_nuE, genie_data_nuE_low, genie_data_nuE_high);
		
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
					// Grab incoming neutrino energy and fill histogram
					float nu_E = *nu_in_e;
					h1->Fill(nu_E);
				}
			}
		}
		// Multiply by NUISANCE's scale factor
		h1->Scale(scale_factor);
		// In the denominator of NUISANCE's scale factor is the integrated 
		// flux for the flux used to generate the events. We want to get rid
		// of this factor because we only want to divide by the flux we are 
		// using in the analysis, which in general will not be the entire 
		// flux used to generate events. Therefore, multiply by the 
		// integrated flux.
		h1->Scale(flux_int);	

		// Construct the new flux to divide by. This flux will be the flux 
		// used to generate the events, with the bin width and range of the 
		// signal.
		// Initialize a new histogram for the signal flux with the same 
		// binning as the analysis binning 
		TH1D* sigflux = new TH1D("sigflux", "signal flux", \
			genie_data_n_bins_nuE, genie_data_nuE_low, genie_data_nuE_high);
		// If a bin in the signal flux corresponds with a bin in the 
		// original flux, then set the bin content of the signal flux bin to 
		// match the content of the original flux bin. This is the line that 
		// "trims" the original flux to the proper range.
		for(int xBin = 1; xBin<=flux->GetNbinsX(); ++xBin) {
			if (genie_data_nuE_low <= flux->GetXaxis()->GetBinCenter(xBin) \
				<= genie_data_nuE_high) {
				sigflux->SetBinContent(xBin, flux->GetBinContent(xBin));
			}
		}
		// Divide the cross section histogram by the signal flux. This 
		// accounts for variations in event rates due to there being more 
		// neutrinos of some energies than others. 
		h1->Divide(sigflux);

		// Divide histograms by the energy to get cross 
		// section per neutrino energy.
		for(int i = 1; i <= h1->GetNbinsX(); i++) {
			h1->SetBinContent(i, h1->GetBinContent(i) / h1->GetBinCenter(i));
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
			h->GetXaxis()->SetTitle("E_{#nu} (GeV)");
			h->GetYaxis()->SetTitle(\
				"#sigma/E_{#nu} (cm^{2} GeV^{-1} nucleon^{-1})");
			h->SetTitle("Numubar CC Cross Section");
			// Add entry to the legend
			legend->AddEntry(h, "GENIE");
		}
		// If it's the data file, draw with a different format
		else if (i == (n_files - 1)) {
			legend->AddEntry(h, "Fake Data");
			h->Draw("SAME e1p");
		}
		// For all other files, draw with the same format
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
	c->Print("numubarCC_Enu_avg.png");
	histo_file.Close();
}
