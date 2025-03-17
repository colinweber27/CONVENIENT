// Author: Colin M Weber (webe1077@umn.edu)
// Date: 3 April 2024
// Purpose: To read in a ROOT file with a neutrino flux and output a text 
// file with the spectrum translated into GiBUU beam energy format.
//
// GiBUU beam energy format is left edge[tab] value\n, where each new line 
// is a new bin.

// Command: root -q "make_beam_energy_dat.C(
// 		\"flux_file\", \"flux_histo\", \"output_name.dat\")"

// Parameters
// 	flux_file
// 		The path to the ROOT file containing the neutrino flux as a 1D 
// 		histogram.
// 	flux_hist
// 		The name of the neutrino flux in flux_file.
// 	output_name.dat
// 		The name of the output file. Must end in .dat.

// Outputs
// 	new_file
// 		The output file containing a neutrino flux in the format readable by 
// 		GiBUU.
 
// Includes
// C++ includes
#include <string>
#include <algorithm>
#include <iostream>
// ROOT includes
#include <TFile.h>
#include <TH1D.h>

void make_beam_energy_dat(std::string flux_file_str, \
	std::string flux_histo_string, std::string output) {
	// Read in the flux file
	TFile* flux_file = new TFile(flux_file_str.c_str());

	// Read in the energy spectrum from the flux file
	TH1D* flux_histo = (TH1D*)flux_file->Get(flux_histo_string.c_str());
	
	// Initialize the beam energy string, which will be constructed 
	// throughout the rest of the macro.
	std::string beam_energy = "";

	// Get the number of bins in the histogram, which we use for looping 
	// through the bins.
	int n_bins = flux_histo->GetNbinsX();

	// Loop through the bins, and append their height to the string
	for (int bin = 1; bin <= n_bins; bin++)
	{
		float low_edge = flux_histo->GetBinLowEdge(bin);
		std::string low_edge_str = std::to_string(low_edge);
		low_edge_str.erase(low_edge_str.find_last_not_of('0') + 1, std::string::npos);
		const char* back = &low_edge_str.back();
		if(std::strncmp(".", back, 1) == 0) {
			low_edge_str.erase(low_edge_str.find("."), std::string::npos);
		}
		beam_energy += low_edge_str;
		beam_energy += "\t";
		float bin_height = flux_histo->GetBinContent(bin) / 1e12;
		beam_energy += std::to_string(bin_height);
		beam_energy += "\n";
	}

	// Create the new file
	std::ofstream new_file(output.c_str());

	// Write the beam energy to the file
	new_file << beam_energy;

	// Save and close the new file
	new_file.close();

	// Close the original file
	flux_file->Close();
}
